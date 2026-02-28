// client/client.h
#ifndef CLIENT_H
#define CLIENT_H

#include "../common/commands.h" // 包含命令协议定义
#include <openssl/ssl.h>

// 连接结构
typedef struct {
    int sockfd;
    SSL_CTX *ctx;
    SSL *ssl;
} Connection;

// 建立连接的函数
Connection* connect_to_server(const char *hostname, int port);

// 发送 TCP 命令的函数
int perform_tcp_command(Connection *conn, Command *cmd, Response *resp);

// 关闭连接的函数
void close_connection(Connection *conn);

#endif