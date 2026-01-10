#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "emp_list.h"

int count = 0; /* 定义此变量主要用于员工动态编号编号的处理
                   （才疏学浅只能想到这个方法） */

// 尾插法(优化版) - 加入tail指针使时间复杂度从 O(n) -> O(1)
int insert(Employee val)
{
    Node q = (Node)malloc(sizeof(struct node)); /* 创建一个新结点 */
    if (q == NULL)
        return -1;
    q->next = NULL;
    q->val = val;
    if (tail == NULL)
    { /* 空链表 */
        head = q;
        tail = q;
    }
    else
    {
        tail->next = q;
        tail = q;
    }
    return 1;
}

// 打印员工列表
void emp_list()
{
    if (head == NULL)
    {
        printf("暂无员工\n");
        return;
    }
    printf("\n--- 员工名单 ---\n");
    printf("%-4s %-15s %-6s %-12s %-20s\n", "编号", "姓名", "性别", "出生日期", "部门");
    printf("---------------------------------------------\n");
    Node p = head;
    while (p != NULL)
    {
        for (int i = 0; i < count; i++)
        {
            printf("%-4d %-15s %-6s %-12s %-20s\n",
                   i + 1,
                   p->val.name,
                   p->val.sex,
                   p->val.birth,
                   p->val.dept);
            p = p->next;
        }
        printf("---------------------------------------------\n\n");
    }
}

// 每次运行时 自动遍历员工数据
void emp_load()
{
    FILE *fp = fopen("emp_list.txt", "r");
    if (!fp)
    {
        printf("数据拉取失败，请检查文件位置\n");
        return;
    }

    // 释放旧链表
    Node current = head;
    while (current != NULL)
    {
        Node temp = current;
        current = current->next;
        free(temp);
    }

    head = NULL;
    tail = NULL;
    count = 0;

    Employee temp;
    while (fscanf(fp, "%49s %9s %10s %49s",
                  temp.name, temp.sex, temp.birth, temp.dept) == 4)
    {
        insert(temp);
    }
    fclose(fp);
}

// 保存更新的数据到 emp_list.txt
void emp_save()
{
    FILE *fp = fopen("emp_list.txt", "w");
    if (!fp)
    {
        printf("无法保存到文件！\n");
        return;
    }

    Node current = head;
    while (current != NULL)
    {
        fprintf(fp, "%s %s %s %s\n",
                current->val.name,
                current->val.sex,
                current->val.birth,
                current->val.dept);
        current = current->next;
    }
    fclose(fp);
    printf("数据已保存。\n");
}

// 删除员工
void emp_del()
{
    if (count == 0)
    {
        printf("当前无员工可删除\n");
        return;
    }

    int index;
    printf("请输入要删除的员工编号（1 ～ %d）: ", count);
    scanf("%d", &index);

    if (index < 1 || index > count)
    {
        printf("无效编号！\n");
        return;
    }

    Node prev = NULL;
    Node current = head;
    int i = 1;
    while (current != NULL && i < index)
    {
        prev = current;
        current = current->next;
        i++;
    }

    if (current == NULL)
    {
        printf("无效编号！\n");
        return;
    }

    char deleted_name[50];
    strcpy(deleted_name, current->val.name);

    if (prev == NULL)
    {
        // 删除头节点
        head = current->next;
        if (head = NULL)
        {
            tail = NULL;
        }
    }
    else
    {
        // 删除中间或尾节点
        prev->next = current->next;
        if (current == tail)
        {
            tail = prev;
        }
    }
    free(current);
    count--;

    printf("员工 [%s] 已删除。\n", deleted_name);
}

// 菜单
void menu()
{
    printf("\n=== 人事管理系统 ===\n");
    printf("1. list\n");
    printf("2. add\n");
    printf("3. del\n");
    printf("4. save\n");
    printf("5. exit\n");
    printf("0. quit\n");
    printf("请选择操作: ");
}