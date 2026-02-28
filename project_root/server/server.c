// server/server.c
#include "server.h"
#include "../common/database.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>

void handle_client_tcp(SSL *ssl) {
    Command cmd;
    Response resp;
    int bytes_read;

    // 循环处理多个命令
    while (1) {
        // 读取命令
        bytes_read = SSL_read(ssl, &cmd, sizeof(Command));
        if (bytes_read <= 0) {
            int ssl_error = SSL_get_error(ssl, bytes_read);
            // 如果是连接关闭，不打印错误
            if (ssl_error != SSL_ERROR_ZERO_RETURN) {
                fprintf(stderr, "SSL_read failed or connection closed. Error code: %d\n", ssl_error);
            }
            break;
        }

        printf("收到 TCP 命令: %d\n", cmd.type);

        // 初始化响应
        resp.success = 1;
        memset(resp.message, 0, sizeof(resp.message));

        // 处理命令
        switch (cmd.type) {
            case CMD_LIST:
                printf("处理 CMD_LIST 命令\n");
                db_list_employees(resp.message, sizeof(resp.message));
                break;
            case CMD_ADD:
                printf("处理 CMD_ADD 命令\n");
                db_add_employee(cmd.data.add.name, cmd.data.add.sex, cmd.data.add.birth, cmd.data.add.dept, resp.message, sizeof(resp.message));
                break;
            case CMD_DELETE:
                printf("处理 CMD_DELETE 命令\n");
                if (cmd.data.serial_number > 0) {
                    db_delete_employee_by_serial(cmd.data.serial_number, resp.message, sizeof(resp.message));
                } else {
                    resp.success = 0;
                    strcpy(resp.message, "错误：员工序号必须为正整数\n");
                }
                break;
            default:
                printf("处理 CMD_ERROR 命令\n");
                resp.success = 0;
                strcpy(resp.message, "错误：未知命令\n");
                break;
        }

        // 发送响应
        int bytes_written = SSL_write(ssl, &resp, sizeof(Response));
        if (bytes_written <= 0) {
            fprintf(stderr, "SSL_write failed\n");
            break;
        }
        
        printf("发送 TCP 响应完成。\n");
    }
    
    printf("客户端连接已关闭\n");
}