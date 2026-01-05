#ifndef HR_SYSTEM_H
#define HR_SYSTEM_H

#include <stdio.h>

typedef struct _Employee {
    int id;
    char name[50];
    char sex[50];
    char birth[50];
    char dept[50];
} Employee;

typedef struct HashNode {
    Employee* emp;
    struct HashNode* next;
} HashNode;

typedef struct {
    Employee* employees;
    int count; 
    int capacity; 

    HashNode** hash_table;
    int table_size;
} HRSystem;

HRSystem* create_hr_system(int init_capacity, int hash_size);
int add_employee(HRSystem* hr, int id, const char* name, const char* sex, const char* birth,const char* dept);
int delete_employee(HRSystem* hr, int id);
void print_all_employees(HRSystem* hr);
void destroy_hr_system(HRSystem* hr);
int save_to_file(HRSystem* hr, const char* filename);
int load_from_file(HRSystem* hr, const char* filename);

#endif
