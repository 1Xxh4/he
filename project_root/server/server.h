// server/server.h
#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <openssl/ssl.h>

// #include "../common/commands.h" // 移除这一行
#include "../common/database.h" // 添加这一行，以获取 RESPONSE_BUFFER_SIZE

#define PORT 8888
#define CERT_FILE "../server.crt"
#define KEY_FILE "../server.key"

extern SSL_CTX *g_ssl_ctx;

void handle_client_https(SSL *ssl);

#endif