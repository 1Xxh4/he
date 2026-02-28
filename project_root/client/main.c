// client/main.c
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_help() {
    printf("可用命令:\n");
    printf("  list                    - 列出所有员工\n");
    printf("  add <name> <sex> <birth> <dept> - 添加员工\n");
    printf("  del <id>                - 删除员工\n");
    printf("  exit                    - 退出程序\n");
}

int main(int argc, char *argv[]) {
    const char *hostname = "localhost"; // 服务端主机名或 IP
    int port = 8888;                    // 服务端端口
    Command cmd;
    Response resp;
    char input[256];
    char *args[6];
    int arg_count;

    // 建立连接
    Connection *conn = connect_to_server(hostname, port);
    if (!conn) {
        fprintf(stderr, "连接服务器失败\n");
        return 1;
    }

    // 如果有命令行参数，执行单个命令
    if (argc > 1) {
        if (strcmp(argv[1], "list") == 0) {
            printf("正在获取员工列表...\n");
            cmd.type = CMD_LIST;
            if (perform_tcp_command(conn, &cmd, &resp) == 0) {
                printf("\n--- 服务器响应 ---\n%s\n------------------\n", resp.message);
            } else {
                printf("发送命令失败\n");
            }
        }
        else if (strcmp(argv[1], "add") == 0) {
            if (argc != 6) {
                printf("错误: 添加员工需要 4 个参数 (name, sex, birth, dept)\n");
                close_connection(conn);
                return 1;
            }
            printf("正在添加员工: %s (%s) in %s, born %s...\n", argv[2], argv[3], argv[5], argv[4]);
            cmd.type = CMD_ADD;
            strncpy(cmd.data.add.name, argv[2], sizeof(cmd.data.add.name) - 1);
            strncpy(cmd.data.add.sex, argv[3], sizeof(cmd.data.add.sex) - 1);
            strncpy(cmd.data.add.birth, argv[4], sizeof(cmd.data.add.birth) - 1);
            strncpy(cmd.data.add.dept, argv[5], sizeof(cmd.data.add.dept) - 1);
            if (perform_tcp_command(conn, &cmd, &resp) == 0) {
                printf("\n--- 服务器响应 ---\n%s\n------------------\n", resp.message);
            } else {
                printf("发送命令失败\n");
            }
        }
        else if (strcmp(argv[1], "del") == 0) {
            if (argc != 3) {
                printf("错误: 删除员工需要 1 个参数 (id)\n");
                close_connection(conn);
                return 1;
            }
            printf("正在删除 ID 为 %s 的员工...\n", argv[2]);
            cmd.type = CMD_DELETE;
            cmd.data.serial_number = atoi(argv[2]);
            if (perform_tcp_command(conn, &cmd, &resp) == 0) {
                printf("\n--- 服务器响应 ---\n%s\n------------------\n", resp.message);
            } else {
                printf("发送命令失败\n");
            }
        }

        else {
            printf("未知命令: %s\n", argv[1]);
            print_help();
        }
    }
    // 否则从标准输入读取多个命令
    else {
        printf("已连接到服务器，输入命令开始操作 (输入 'help' 查看可用命令)\n");
        print_help();

        while (1) {
            printf("\n请输入命令: ");
            if (fgets(input, sizeof(input), stdin) == NULL) {
                break;
            }

            // 去除换行符
            input[strcspn(input, "\n")] = '\0';

            // 解析命令行参数
            arg_count = 0;
            char *token = strtok(input, " ");
            while (token && arg_count < 6) {
                args[arg_count++] = token;
                token = strtok(NULL, " ");
            }

            if (arg_count == 0) {
                continue;
            }

            if (strcmp(args[0], "exit") == 0) {
                printf("正在退出...\n");
                break;
            }

            else if (strcmp(args[0], "list") == 0) {
                printf("正在获取员工列表...\n");
                cmd.type = CMD_LIST;
                if (perform_tcp_command(conn, &cmd, &resp) == 0) {
                    printf("\n--- 服务器响应 ---\n%s\n------------------\n", resp.message);
                } else {
                    printf("发送命令失败\n");
                }
            }
            else if (strcmp(args[0], "add") == 0) {
                if (arg_count != 5) {
                    printf("错误: 添加员工需要 4 个参数 (name, sex, birth, dept)\n");
                    continue;
                }
                printf("正在添加员工: %s (%s) in %s, born %s...\n", args[1], args[2], args[4], args[3]);
                cmd.type = CMD_ADD;
                strncpy(cmd.data.add.name, args[1], sizeof(cmd.data.add.name) - 1);
                strncpy(cmd.data.add.sex, args[2], sizeof(cmd.data.add.sex) - 1);
                strncpy(cmd.data.add.birth, args[3], sizeof(cmd.data.add.birth) - 1);
                strncpy(cmd.data.add.dept, args[4], sizeof(cmd.data.add.dept) - 1);
                if (perform_tcp_command(conn, &cmd, &resp) == 0) {
                    printf("\n--- 服务器响应 ---\n%s\n------------------\n", resp.message);
                } else {
                    printf("发送命令失败\n");
                }
            }
            else if (strcmp(args[0], "del") == 0) {
                if (arg_count != 2) {
                    printf("错误: 删除员工需要 1 个参数 (id)\n");
                    continue;
                }
                printf("正在删除 ID 为 %s 的员工...\n", args[1]);
                cmd.type = CMD_DELETE;
                cmd.data.serial_number = atoi(args[1]);
                if (perform_tcp_command(conn, &cmd, &resp) == 0) {
                    printf("\n--- 服务器响应 ---\n%s\n------------------\n", resp.message);
                } else {
                    printf("发送命令失败\n");
                }
            }
            else {
                printf("未知命令: %s\n", args[0]);
                print_help();
            }
        }
    }

    // 关闭连接
    close_connection(conn);
    printf("已断开与服务器的连接\n");

    return 0;
}