// hr_system.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hr_system.h"

unsigned int hash_func(int id, int table_size) {
    return id % table_size;
}

HRSystem* create_hr_system(int init_capacity, int hash_size) {
    HRSystem* hr = (HRSystem*)calloc(1, sizeof(HRSystem));
    if (!hr) return NULL;

    hr->employees = (Employee*)malloc(init_capacity * sizeof(Employee));
    hr->capacity = init_capacity;
    hr->count = 0;

    hr->table_size = hash_size;
    hr->hash_table = (HashNode**)calloc(hash_size, sizeof(HashNode*));

    return hr;
}

int resize_employees(HRSystem* hr) {
    int new_cap = hr->capacity * 2;
    Employee* new_arr = (Employee*)realloc(hr->employees, new_cap * sizeof(Employee));
    if (!new_arr) return 0;
    hr->employees = new_arr;
    hr->capacity = new_cap;
    return 1;
}

int add_employee(HRSystem* hr, int id, const char* name, const char* sex, const char* birth, const char* dept) {

    for (int i = 0; i < hr->count; i++) {
        if (hr->employees[i].id == id) {
            printf("❌ 员工ID %d 已存在！\n", id);
            return 0;
        }
    }


    if (hr->count >= hr->capacity) {
        if (!resize_employees(hr)) {
            printf("❌ 内存不足，无法添加员工！\n");
            return 0;
        }
    }

    Employee* emp = &hr->employees[hr->count];
    emp->id = id;
    strncpy(emp->name, name, 49);
    emp->name[49] = '\0';
    strncpy(emp->sex, sex, 49);
    emp->sex[49] = '\0';
    strncpy(emp->birth, birth, 49);
    emp->birth[49] = '\0';
    strncpy(emp->dept, dept, 49);
    emp->dept[49] = '\0';
    hr->count++;

    unsigned int idx = hash_func(id, hr->table_size);
    HashNode* node = (HashNode*)malloc(sizeof(HashNode));
    node->emp = emp;
    node->next = hr->hash_table[idx];
    hr->hash_table[idx] = node;

    printf("✅ 成功添加员工: %s (ID: %d)\n", name, id);
    return 1;
}

// 删除员工
int delete_employee(HRSystem* hr, int id) {

    Employee* target = NULL;
    int target_index = -1;
    for (int i = 0; i < hr->count; i++) {
        if (hr->employees[i].id == id) {
            target = &hr->employees[i];
            target_index = i;
            break;
        }
    }
    if (!target) {
        printf("❌ 未找到ID为 %d 的员工\n", id);
        return 0;
    }


    unsigned int hidx = hash_func(id, hr->table_size);
    HashNode** pp = &hr->hash_table[hidx];
    while (*pp) {
        if ((*pp)->emp == target) {
            HashNode* to_free = *pp;
            *pp = (*pp)->next;
            free(to_free);
            break;
        }
        pp = &((*pp)->next);
    }

    if (target_index != hr->count - 1) {
        hr->employees[target_index] = hr->employees[hr->count - 1];

        int moved_id = hr->employees[target_index].id;
        unsigned int moved_hidx = hash_func(moved_id, hr->table_size);
        HashNode* node = hr->hash_table[moved_hidx];
        while (node) {
            if (node->emp->id == moved_id) {
                node->emp = &hr->employees[target_index]; // 更新指针
                break;
            }
            node = node->next;
        }
    }

    hr->count--;
    printf("✅ 成功删除员工 ID: %d\n", id);
    return 1;
}

void print_all_employees(HRSystem* hr) {
    if (hr->count == 0) {
        printf("📭 当前无员工\n");
        return;
    }
    printf("\n📋 所有员工:\n");
    printf("ID\t姓名\t性别\t出生日期\t部门\n");
    printf("----------------------------------------------\n");
    for (int i = 0; i < hr->count; i++) {
        printf("%d\t%-5s\t%-5s\t%-5s\t%s\n", 
               hr->employees[i].id, 
               hr->employees[i].name,
               hr->employees[i].sex,
               hr->employees[i].birth,   
               hr->employees[i].dept);
    }
}

void destroy_hr_system(HRSystem* hr) {
    if (!hr) return;

    for (int i = 0; i < hr->table_size; i++) {
        HashNode* curr = hr->hash_table[i];
        while (curr) {
            HashNode* next = curr->next;
            free(curr);
            curr = next;
        }
    }
    free(hr->hash_table);

    free(hr->employees);
    free(hr);
}

int save_to_file(HRSystem* hr, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("❌ 无法打开文件 %s 保存数据\n", filename);
        return 0;
    }

    for (int i = 0; i < hr->count; i++) {
        fprintf(file, "%d;%s;%s;%s;%s\n", 
                hr->employees[i].id, 
                hr->employees[i].name, 
                hr->employees[i].sex, 
                hr->employees[i].birth, 
                hr->employees[i].dept);
    }

    fclose(file);
    printf("✅ 数据已保存到 %s\n", filename);
    return 1;
}

int load_from_file(HRSystem* hr, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("ℹ️ 无保存数据文件 %s，开始空数据库\n", filename);
        return 1; // 不算错误
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {

        line[strcspn(line, "\n")] = '\0';

        int id;
        char name[50], sex[50], birth[50], dept[50];

        char *token = strtok(line, ";");
        if (!token || (id = atoi(token)) <= 0) continue;

        token = strtok(NULL, ";");
        if (!token || strlen(token) == 0) continue;
        strncpy(name, token, 49);
        name[49] = '\0';

        token = strtok(NULL, ";");
        if (!token || strlen(token) == 0) continue;
        strncpy(sex, token, 49);
        sex[49] = '\0';

        token = strtok(NULL, ";");
        if (!token || strlen(token) == 0) continue;
        strncpy(birth, token, 49);
        birth[49] = '\0';

        token = strtok(NULL, ";");
        if (!token || strlen(token) == 0) continue;
        strncpy(dept, token, 49);
        dept[49] = '\0';

        if (strtok(NULL, ";")) continue;

        add_employee(hr, id, name, sex, birth, dept);
    }

    fclose(file);
    printf("✅ 数据已从 %s 加载\n", filename);
    return 1;
}
