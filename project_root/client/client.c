// client/client.c
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// 建立连接的函数
Connection* connect_to_server(const char *hostname, int port) {
    Connection *conn = (Connection*)malloc(sizeof(Connection));
    if (!conn) {
        fprintf(stderr, "内存分配失败\n");
        return NULL;
    }

    struct addrinfo hints, *res, *p;

    // 1. 初始化 SSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD *method_ssl = TLS_client_method();
    conn->ctx = SSL_CTX_new(method_ssl);
    if (!conn->ctx) {
        ERR_print_errors_fp(stderr);
        free(conn);
        return NULL;
    }

    // 2. 创建 TCP 连接，使用传入的端口
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    char portstr[8];
    snprintf(portstr, sizeof(portstr), "%d", port);
    if (getaddrinfo(hostname, portstr, &hints, &res) != 0) {
        perror("getaddrinfo 失败");
        SSL_CTX_free(conn->ctx);
        free(conn);
        return NULL;
    }

    for(p = res; p != NULL; p = p->ai_next) {
        conn->sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (conn->sockfd == -1) {
            perror("socket 失败");
            continue;
        }
        if (connect(conn->sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect 失败");
            close(conn->sockfd);
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if (p == NULL) {
        fprintf(stderr, "无法连接到服务器\n");
        SSL_CTX_free(conn->ctx);
        free(conn);
        return NULL;
    }

    // 3. 创建 SSL 对象并关联
    conn->ssl = SSL_new(conn->ctx);
    if (!conn->ssl) {
        fprintf(stderr, "无法创建 SSL 对象\n");
        close(conn->sockfd);
        SSL_CTX_free(conn->ctx);
        free(conn);
        return NULL;
    }
    SSL_set_fd(conn->ssl, conn->sockfd);

    // 4. SSL 握手
    if (SSL_connect(conn->ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "SSL 握手失败\n");
        SSL_free(conn->ssl);
        close(conn->sockfd);
        SSL_CTX_free(conn->ctx);
        free(conn);
        return NULL;
    }

    printf("SSL 握手成功\n");
    return conn;
}

// 发送 TCP 命令的函数
int perform_tcp_command(Connection *conn, Command *cmd, Response *resp) {
    if (!conn || !conn->ssl) {
        fprintf(stderr, "连接未初始化\n");
        return 1;
    }

    // 初始化响应结构
    memset(resp, 0, sizeof(Response));

    // 发送命令
    int bytes_written = SSL_write(conn->ssl, cmd, sizeof(Command));
    if (bytes_written <= 0) {
        fprintf(stderr, "发送命令失败\n");
        return 1;
    }
    printf("已发送命令: %d\n", cmd->type);

    // 读取响应
    int bytes_read = 0;
    int total_bytes_read = 0;
    while (total_bytes_read < sizeof(Response)) {
        bytes_read = SSL_read(conn->ssl, (char*)resp + total_bytes_read, sizeof(Response) - total_bytes_read);
        if (bytes_read <= 0) {
            fprintf(stderr, "读取响应失败\n");
            return 1;
        }
        total_bytes_read += bytes_read;
    }

    return 0; // 成功
}

// 关闭连接的函数
void close_connection(Connection *conn) {
    if (conn) {
        if (conn->ssl) {
            SSL_shutdown(conn->ssl);
            SSL_free(conn->ssl);
        }
        if (conn->sockfd != -1) {
            close(conn->sockfd);
        }
        if (conn->ctx) {
            SSL_CTX_free(conn->ctx);
        }
        free(conn);
    }
}