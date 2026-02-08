#ifndef EMP_ARRAY_H
#define EMP_ARRAY_H

typedef struct {
    char name[50];
    char sex[10];
    char birth[11];
    char dept[50];
} Employee;

int ensure_capacity(int needed);
void emp_load();
void emp_save();
void emp_list();
void emp_add();
void emp_del();
void menu();



#endif
