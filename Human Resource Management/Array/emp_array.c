#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emp_array.h"


// 动态数组指针 + 状态变量
Employee *employees = NULL;
int emp_count = 0;
int capacity = 0;  // 当前分配的容量（以 Employee 为单位）

// 辅助函数：确保容量足够
int ensure_capacity(int needed) {
    if (needed <= capacity) return 1;

    // 策略：按 1.5 倍或至少 needed 扩容（避免频繁 realloc）
    int new_capacity = (capacity > 0) ? (capacity + capacity / 2) : 1;
    if (new_capacity < needed) new_capacity = needed;
 
    Employee *temp = realloc(employees, new_capacity * sizeof(Employee));
    if (!temp) {
        fprintf(stderr, "内存扩容失败！\n");
        return 0; // 失败
    }
    employees = temp;
    capacity = new_capacity;
    return 1; // 成功
}

void emp_load() {
    FILE *fp = fopen("emp_array.txt", "r");
    if (!fp) {
        printf("未找到 employees.txt，请检查文件位置\n");
        return;
    }

    emp_count = 0;
    capacity = 0;
    free(employees); // 释放旧内存（首次为 NULL 也安全）
    employees = NULL;

    Employee temp;
    while (fscanf(fp, "%49s %9s %10s %49s",
                  temp.name, temp.sex, temp.birth, temp.dept) == 4) {
        if (!ensure_capacity(emp_count + 1)) {
            printf("加载数据时内存不足，已加载部分数据。\n");
            break;
        }
        employees[emp_count] = temp;
        emp_count++;
    }
    fclose(fp);
}

void emp_save() {
    FILE *fp = fopen("emp_array.txt", "w");
    if (!fp) {
        printf("无法保存到文件！\n");
        return;
    }

    for (int i = 0; i < emp_count; i++) {
        fprintf(fp, "%s %s %s %s\n",
                employees[i].name,
                employees[i].sex,
                employees[i].birth,
                employees[i].dept);
    }
    fclose(fp);
    printf("数据已保存。\n");
}

void emp_list() {
    if (emp_count == 0) {
        printf("当前无员工记录。\n");
        return;
    }
    printf("\n--- 员工名单 ---\n");
    printf("%-4s %-15s %-6s %-12s %-20s\n", "编号", "姓名", "性别", "出生日期", "部门");
    printf("---------------------------------------------\n");
    for (int i = 0; i < emp_count; i++) {
        printf("%-4d %-15s %-6s %-12s %-20s\n",
               i + 1,
               employees[i].name,
               employees[i].sex,
               employees[i].birth,
               employees[i].dept);
    }
    printf("---------------------------------------------\n\n");
}

void emp_add() {
    Employee new_emp;
    printf("请输入姓名: ");
    scanf("%49s", new_emp.name);

    printf("请输入性别（男/女）: ");
    scanf("%9s", new_emp.sex);

    printf("请输入出生日期（格式 YYYY-MM-DD）: ");
    scanf("%10s", new_emp.birth);

    printf("请输入部门: ");
    scanf("%49s", new_emp.dept);

    if (!ensure_capacity(emp_count + 1)) {
        printf("内存不足，无法添加新员工！\n");
        return;
    }

    employees[emp_count] = new_emp;
    emp_count++;
    printf("员工 [%s] 添加成功！\n", new_emp.name);
}

void emp_del() {
    if (emp_count == 0) {
        printf("当前无员工可删除。\n");
        return;
    }

    int index;
    printf("请输入要删除的员工编号（1 ～ %d）: ", emp_count);
    scanf("%d", &index);

    if (index < 1 || index > emp_count) {
        printf("无效编号！\n");
        return;
    }

    char deleted_name[50];
    strcpy(deleted_name, employees[index - 1].name);

    // 前移覆盖
    for (int i = index - 1; i < emp_count - 1; i++) {
        employees[i] = employees[i + 1];
    }
    emp_count--;

    printf("员工 [%s] 已删除。\n", deleted_name);

    // 可选：当使用率过低时缩容（此处省略，避免频繁 realloc）
}

void menu() {
    printf("\n=== 人事管理系统 ===\n");
    printf("1. list\n");
    printf("2. add\n");
    printf("3. del\n");
    printf("4. save\n");
    printf("5. exit\n");
    printf("0. quit\n");
    printf("请选择操作: ");
}