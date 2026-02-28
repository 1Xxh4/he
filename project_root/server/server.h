// server/server.h
#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <openssl/ssl.h>

#include "../common/commands.h" // 添加这一行，以获取命令协议定义
#include "../common/database.h" // 包含数据库头文件

#define PORT 8888
#define CERT_FILE "server.crt"
#define KEY_FILE "server.key"

extern SSL_CTX *g_ssl_ctx;

void handle_client_tcp(SSL *ssl);

#endif