// common/commands.h
#ifndef COMMANDS_H
#define COMMANDS_H

// 命令类型定义
typedef enum {
    CMD_LIST = 1,    // 列出所有员工
    CMD_ADD,         // 添加员工
    CMD_DELETE,      // 删除员工
    CMD_ERROR        // 错误命令
} CommandType;

// 命令结构
typedef struct {
    CommandType type;         // 命令类型
    union {
        // 对于 CMD_ADD 命令
        struct {
            char name[50];    // 姓名
            char sex[10];     // 性别
            char birth[11];   // 出生日期 (YYYY-MM-DD)
            char dept[50];    // 部门
        } add;
        // 对于 CMD_DELETE 命令
        int serial_number;    // 员工序号
    } data;
} Command;

// 响应结构
typedef struct {
    int success;              // 操作是否成功 (1: 成功, 0: 失败)
    char message[20480];       // 响应消息
} Response;

// 命令和响应的最大长度
#define MAX_COMMAND_SIZE 256
#define MAX_RESPONSE_SIZE 20480

#endif