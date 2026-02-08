#include <stdio.h>
#include "emp_array.h"
#include "emp_array.c"

int main() {
    emp_load();

    int choice;
    while (1) {
        menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                emp_list();
                break;
            case 2:
                emp_add();
                break;
            case 3:
                emp_del();
                break;
            case 4:
                emp_save();
                break;
            case 5:
                free(employees);
                printf("感谢使用，再见！\n");
                exit(0);
            case 0:
                printf("是否保存更改？(Y/n): ");
                int c;
                while ((c = getchar()) != '\n' && c != EOF);

                c = getchar();
                if (c == '\n' || c == 'y' || c == 'Y') {
                        emp_save(); 
                } else {
                        printf("未保存更改。\n");
                }

                free(employees);
                printf("感谢使用，再见！\n");
                exit(0);
            default:
                printf("无效选项，请重新输入。\n");
        }
    }

    return 0;
}