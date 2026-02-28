// common/database.h
#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <stddef.h> // For size_t

// 将 RESPONSE_BUFFER_SIZE 移到这里定义
#define RESPONSE_BUFFER_SIZE 20480

extern sqlite3 *db;

/**
 * @brief 初始化数据库连接和表结构
 * @return 1 成功, 0 失败
 */
int db_init(void);

/**
 * @brief 关闭数据库连接
 */
void db_close(void);

/**
 * @brief 列出所有员工信息
 * @param result 存放结果的缓冲区
 * @param result_size 缓冲区大小
 * @return 0 成功, -1 失败
 */
int db_list_employees(char *result, size_t result_size);

/**
 * @brief 添加一名员工
 * @param name 姓名
 * @param sex 性别
 * @param birth 出生日期
 * @param dept 部门
 * @param result 存放结果信息的缓冲区
 * @param result_size 缓冲区大小
 * @return 0 成功, -1 失败
 */
int db_add_employee(const char* name, const char* sex, const char* birth, const char* dept, char *result, size_t result_size);

/**
 * @brief 根据逻辑序号删除员工
 * @param serial_number 逻辑序号 (从1开始)
 * @param result 存放结果信息的缓冲区
 * @param result_size 缓冲区大小
 * @return 0 成功, -1 失败
 */
int db_delete_employee_by_serial(int serial_number, char *result, size_t result_size);

#endif // DATABASE_H