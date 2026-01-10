#ifndef EMP_LIST_H
#define EMP_LIST_H

typedef struct
{
    char name[50];
    char sex[10];
    char birth[11];
    char dept[50];
} Employee;

typedef struct node *Node;
static Node head = NULL;
static Node tail = NULL;

struct node
{
    Employee val;
    Node next;
};

int insert(Employee val);
void emp_list();
void emp_load();
void emp_save();
void emp_del();
void menu();


#endif  //防止头文件重复包含