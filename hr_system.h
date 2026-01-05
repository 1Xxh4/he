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

// 哈希表节点（处理冲突用链地址法）
typedef struct HashNode {
    Employee* emp;           // 指向员工数据
    struct HashNode* next;
} HashNode;

// 人事系统结构
typedef struct {
    Employee* employees;     // 动态员工数组
    int count;               // 当前员工数
    int capacity;            // 当前容量

    HashNode** hash_table;   // 哈希索引表
    int table_size;          // 哈希表大小
} HRSystem;

// 函数声明
HRSystem* create_hr_system(int init_capacity, int hash_size);
int add_employee(HRSystem* hr, int id, const char* name, const char* sex, const char* birth,const char* dept);
int delete_employee(HRSystem* hr, int id);
void print_all_employees(HRSystem* hr);
void destroy_hr_system(HRSystem* hr);
int save_to_file(HRSystem* hr, const char* filename);
int load_from_file(HRSystem* hr, const char* filename);

#endif