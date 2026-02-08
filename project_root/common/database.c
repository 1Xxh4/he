// 数据库操作实现
#include "database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_NAME "employees.db"

sqlite3 *db = NULL;

// 内部辅助函数，用于检查字符串安全性
static int is_safe_string(const char* str) {
    for (int i = 0; str[i]; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c == '\'' || c == '\"' || c == ';' || c == '\\') {
            return 0; // 包含不安全字符
        }
    }
    return 1; // 安全
}

int db_init(void) {
    int rc = sqlite3_open(DB_NAME, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    const char *sql = 
        "CREATE TABLE IF NOT EXISTS employees ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "sex TEXT,"
        "birth TEXT,"
        "dept TEXT);";

    char *err_msg = NULL;
    rc = sqlite3_exec(db, sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "建表失败: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        db = NULL;
        return 0;
    }
    printf("数据库初始化完成。\n");
    return 1;
}

void db_close(void) {
    if (db) {
        sqlite3_close(db);
        db = NULL;
    }
}

int db_list_employees(char *result, size_t result_size) {
    const char *sql = "SELECT ROW_NUMBER() OVER (ORDER BY id) AS serial_num, name, sex, birth, dept FROM employees;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        snprintf(result, result_size, "内部错误: 无法准备查询语句 (%s)\n", sqlite3_errmsg(db));
        return -1;
    }

    int printed = snprintf(result, result_size, "序号\t姓名\t性别\t出生\t部门\n----------------------------------------\n");
    if (printed < 0 || printed >= result_size) {
        sqlite3_finalize(stmt);
        return -1;
    }
    size_t current_len = printed;

    int row_count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        row_count++;
        int serial_num = sqlite3_column_int(stmt, 0);
        const char *name = (const char*)sqlite3_column_text(stmt, 1);
        const char *sex = (const char*)sqlite3_column_text(stmt, 2);
        const char *birth = (const char*)sqlite3_column_text(stmt, 3);
        const char *dept = (const char*)sqlite3_column_text(stmt, 4);

        printed = snprintf(result + current_len, result_size - current_len,
                 "%d\t%s\t%s\t%s\t%s\n",
                 serial_num, name ? name : "NULL", sex ? sex : "NULL", birth ? birth : "NULL", dept ? dept : "NULL");
        
        if (printed < 0 || printed >= (result_size - current_len)) {
            sqlite3_finalize(stmt);
            return -1;
        }
        current_len += printed;
    }

    if (rc != SQLITE_DONE) {
        snprintf(result, result_size, "查询执行失败: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    if (row_count == 0) {
        strcpy(result, "无匹配记录。\n");
    }
    sqlite3_finalize(stmt);
    return 0;
}

int db_add_employee(const char* name, const char* sex, const char* birth, const char* dept, char *result, size_t result_size) {
    if (!is_safe_string(name) || !is_safe_string(sex) || !is_safe_string(birth) || !is_safe_string(dept)) {
         strcpy(result, "添加失败: 输入包含非法字符 (单引号, 双引号, 分号, 反斜杠)。\n");
         return -1;
    }

    const char *sql = "INSERT INTO employees (name, sex, birth, dept) VALUES (?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        snprintf(result, result_size, "内部错误: 无法准备插入语句 (%s)\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, sex, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, birth, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, dept, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        snprintf(result, result_size, "添加失败: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return -1;
    }

    strcpy(result, "添加成功。\n");
    sqlite3_finalize(stmt);
    return 0;
}

int db_delete_employee_by_serial(int serial_number, char *result, size_t result_size) {
    if (serial_number <= 0) {
        strcpy(result, "删除失败: 逻辑序号必须为正整数。\n");
        return -1;
    }

    const char *find_id_sql = "SELECT id FROM employees ORDER BY id LIMIT 1 OFFSET ?;";
    sqlite3_stmt *find_stmt;
    int rc = sqlite3_prepare_v2(db, find_id_sql, -1, &find_stmt, NULL);
    if (rc != SQLITE_OK) {
        snprintf(result, result_size, "内部错误: 无法准备查找ID语句 (%s)\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(find_stmt, 1, serial_number - 1);

    int physical_id_to_delete = -1;
    rc = sqlite3_step(find_stmt);
    if (rc == SQLITE_ROW) {
        physical_id_to_delete = sqlite3_column_int(find_stmt, 0);
    }
    sqlite3_finalize(find_stmt);

    if (physical_id_to_delete == -1) {
        strcpy(result, "删除失败: 未找到指定逻辑序号的员工。\n");
        return 0;
    }

    const char *delete_sql = "DELETE FROM employees WHERE id = ?;";
    sqlite3_stmt *delete_stmt;
    rc = sqlite3_prepare_v2(db, delete_sql, -1, &delete_stmt, NULL);
    if (rc != SQLITE_OK) {
        snprintf(result, result_size, "内部错误: 无法准备删除语句 (%s)\n", sqlite3_errmsg(db));
        return -1;
    }

    sqlite3_bind_int(delete_stmt, 1, physical_id_to_delete);

    rc = sqlite3_step(delete_stmt);
    if (rc != SQLITE_DONE) {
        snprintf(result, result_size, "删除失败: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(delete_stmt);
        return -1;
    }

    if (sqlite3_changes(db) == 0) {
        strcpy(result, "删除失败: 未找到指定ID的员工。\n");
    } else {
        snprintf(result, result_size, "已删除逻辑序号为 %d 的员工。\n", serial_number);
    }
    sqlite3_finalize(delete_stmt);
    return 0;
}