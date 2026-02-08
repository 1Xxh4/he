// client/main.c
#include "client.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("用法: %s <command> [args]\n", argv[0]);
        printf("命令:\n");
        printf("  list                    - 列出所有员工\n");
        printf("  add <name> <sex> <birth> <dept> - 添加员工\n");
        printf("  del <id>                - 删除员工\n");
        return 1;
    }

    const char *hostname = "localhost"; // 服务端主机名或 IP
    int port = 8888;                    // 服务端端口

    if (strcmp(argv[1], "list") == 0) {
        printf("正在获取员工列表...\n");
        perform_https_request(hostname, port, "/list", "GET");
    }
    else if (strcmp(argv[1], "add") == 0) {
        if (argc != 6) {
            printf("错误: 添加员工需要 4 个参数 (name, sex, birth, dept)\n");
            return 1;
        }
        char path[512];
        snprintf(path, sizeof(path), "/add?name=%s&sex=%s&birth=%s&dept=%s", argv[2], argv[3], argv[4], argv[5]);
        printf("正在添加员工: %s (%s) in %s, born %s...\n", argv[2], argv[3], argv[5], argv[4]);
        perform_https_request(hostname, port, path, "GET");
    }
    else if (strcmp(argv[1], "del") == 0) {
        if (argc != 3) {
            printf("错误: 删除员工需要 1 个参数 (id)\n");
            return 1;
        }
        char path[64];
        snprintf(path, sizeof(path), "/del/%s", argv[2]);
        printf("正在删除 ID 为 %s 的员工...\n", argv[2]);
        perform_https_request(hostname, port, path, "GET");
    }
    else {
        printf("未知命令: %s\n", argv[1]);
        return 1;
    }

    return 0;
}