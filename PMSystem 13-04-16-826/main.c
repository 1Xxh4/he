// main.c
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include "hr_system.h"

int main(int argc, char *argv[]) {
    // 设置locale以支持中文输出（兼容Mac和其他平台）
    setlocale(LC_ALL, "");

    HRSystem* hr = create_hr_system(4, 17); // 初始容量4，哈希表大小17（质数）

    // 加载数据
    load_from_file(hr, "employees.txt");

    int choice, id;
    char name[50],sex[50],birth[50], dept[50];

    while (1) {
        printf("\n=== 人事管理系统 ===\n");
        printf("1. list\n");
        printf("2. add\n");
        printf("3. del\n");
        printf("4. exit\n");
        printf("请选择: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); // 清空输入缓冲
            continue;
        }
        while (getchar() != '\n'); // 清空剩余输入

        switch (choice) {
            case 1:
                print_all_employees(hr);
                break;

            case 2:
                printf("请输入 ID、姓名、性别、出生日期、部门: ");
                char input[200];
                if (fgets(input, sizeof(input), stdin) != NULL) {
                    // 移除换行符
                    input[strcspn(input, "\n")] = 0;
                    if (sscanf(input, "%d %49s %49s %49s %49s", &id, name, sex, birth, dept) == 5) {
                        add_employee(hr, id, name, sex, birth, dept);
                    } else {
                        printf("❌ 输入格式错误！请确保输入5个字段，用空格分隔，无多余空格。\n");
                    }
                } else {
                    printf("❌ 输入读取失败！\n");
                }
                break;

            case 3:
                printf("请输入要删除的员工ID: ");
                if (scanf("%d", &id) == 1) {
                    delete_employee(hr, id);
                } else {
                    printf("❌ 无效ID！\n");
                    while (getchar() != '\n');
                }
                break;

            case 4:
                printf("👋 再见！\n");
                // 保存数据
                save_to_file(hr, "employees.txt");
                destroy_hr_system(hr);
                return 0;

            default:
                printf("❌ 无效选项，请重试。\n");
        }
    }
}