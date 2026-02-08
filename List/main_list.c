#include <stdio.h>
#include "emp_list.h"
#include "emp_list.c"

int main()
{
    emp_load();

    int choice;
    while (1)
    {
        printf("\n=== 人事管理系统 ===\n");
        printf("1. list\n");
        printf("2. add\n");
        printf("3. del\n");
        printf("4. save\n");
        printf("5. exit\n");
        printf("0. quit\n");
        printf("请选择操作: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            emp_list();
            break;
        case 2:
        {
            Employee new_emp;
            printf("请输入姓名: ");
            scanf("%49s", new_emp.name);
            printf("请输入性别（男/女）: ");
            scanf("%9s", new_emp.sex);
            printf("请输入出生日期（格式 YYYY-MM-DD）: ");
            scanf("%10s", new_emp.birth);
            printf("请输入部门: ");
            scanf("%49s", new_emp.dept);
            insert(new_emp);
            printf("员工 [%s] 添加成功！\n", new_emp.name);
            break;
        }
        case 3:
            emp_del();
            break;
        case 4:
            emp_save();
            break;
        case 5:
        {
            Node curr = head;
            while (curr != NULL)
            {
                Node temp = curr;
                curr = curr->next;
                free(temp);
            }
            printf("感谢使用，再见！\n");
            exit(0);
        }
        case 0:
            printf("是否保存更改？ Y/n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF)
                ;

            c = getchar();
            if (c == '\n' || c == 'y' || c == 'Y')
            {
                emp_save();
            }
            else
            {
                printf("未保存更改。\n");
            }

            Node current = head;
            while (current != NULL)
            {
                Node temp = current;
                current = current->next;
                free(temp);
            }
            printf("感谢使用，再见！\n");
            exit(0);
        default:
            printf("无效选项，请重新输入。\n");
        }
    }

    return 0;
}