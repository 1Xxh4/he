// server/main.c
#include "server.h"
#include "../common/database.h" // 路径调整
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

SSL_CTX *g_ssl_ctx = NULL;

SSL_CTX* create_ssl_context() {
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    method = TLS_server_method(); 
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "无法创建 SSL 上下文\n");
        return NULL;
    }

    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "加载证书文件失败: %s\n", CERT_FILE);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "加载私钥文件失败: %s\n", KEY_FILE);
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_check_private_key(ctx) <= 0) {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "私钥与证书不匹配\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    printf("SSL 上下文创建成功\n");
    return ctx;
}

int main() {
    if (!db_init()) {
        fprintf(stderr, "数据库初始化失败！\n");
        return 1;
    }

    g_ssl_ctx = create_ssl_context();
    if (!g_ssl_ctx) {
        fprintf(stderr, "SSL 初始化失败！\n");
        db_close();
        return 1;
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket 创建失败");
        SSL_CTX_free(g_ssl_ctx);
        db_close();
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("设置套接字选项失败");
        close(server_fd);
        SSL_CTX_free(g_ssl_ctx);
        db_close();
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind 失败");
        close(server_fd);
        SSL_CTX_free(g_ssl_ctx);
        db_close();
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen 失败");
        close(server_fd);
        SSL_CTX_free(g_ssl_ctx);
        db_close();
        return 1;
    }

    printf("HTTPS 人事服务端启动，监听端口 %d...\n", PORT);
    printf("示例请求: curl -k https://localhost:%d/list\n", PORT);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept 失败");
            continue;
        }

        printf("新 HTTPS 客户端连接: %s\n", inet_ntoa(client_addr.sin_addr));

        SSL *ssl = SSL_new(g_ssl_ctx);
        if (!ssl) {
            fprintf(stderr, "无法为新连接创建 SSL 对象\n");
            close(client_fd);
            continue;
        }

        SSL_set_fd(ssl, client_fd);

        int ssl_accept_result = SSL_accept(ssl);
        if (ssl_accept_result <= 0) {
            unsigned long ssl_err_code = ERR_get_error();
            fprintf(stderr, "SSL 握手失败: %s (Code: %lu)\n", 
                    ERR_error_string(ssl_err_code, NULL), ssl_err_code);
            SSL_free(ssl);
            close(client_fd);
            continue;
        }

        printf("SSL 握手成功\n");
        handle_client_https(ssl);

        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client_fd);
    }

    close(server_fd);
    SSL_CTX_free(g_ssl_ctx);
    db_close();
    return 0;
}