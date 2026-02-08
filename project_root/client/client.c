// client/client.c
#include "client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// 从 SSL 连接读取完整响应
int read_full_response(SSL *ssl, char *buffer, size_t buffer_size) {
    int total_bytes = 0;
    int header_end_found = 0;
    char *body_start = NULL;

    while (total_bytes < (int)buffer_size - 1) {
        int bytes_read = SSL_read(ssl, buffer + total_bytes, 1);
        if (bytes_read <= 0) {
            break; // 连接关闭或出错
        }
        total_bytes += bytes_read;

        // 查找响应头结束标志 \r\n\r\n
        if (total_bytes >= 4) {
            if (memcmp(buffer + total_bytes - 4, "\r\n\r\n", 4) == 0) {
                header_end_found = 1;
                body_start = buffer + total_bytes;
            }
        }
    }

    if (!header_end_found) {
        printf("警告: 未找到响应头结束标志，可能响应格式不正确。\n");
        buffer[total_bytes] = '\0';
        return total_bytes;
    }

    // 尝试读取 Content-Length 或者读到连接关闭
    // 这里简化处理，认为读完 header 后的内容就是 body
    int body_bytes = 0;
    while (total_bytes + body_bytes < (int)buffer_size - 1) {
        int bytes_read = SSL_read(ssl, buffer + total_bytes + body_bytes, 1);
        if (bytes_read <= 0) {
            break; // 连接关闭或出错
        }
        body_bytes += bytes_read;
    }
    buffer[total_bytes + body_bytes] = '\0';

    // 返回 body 部分的起始位置
    return body_start - buffer;
}

int perform_https_request(const char *hostname, int port, const char *path_and_query, const char *method) {
    int sockfd;
    struct addrinfo hints, *res, *p;
    SSL_CTX *ctx;
    SSL *ssl;
    char request[2048];
    char response[20480];
    int ret = 1; // 默认失败

    // 1. 初始化 SSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    const SSL_METHOD *method_ssl = TLS_client_method();
    ctx = SSL_CTX_new(method_ssl);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        return ret;
    }

    // 2. 创建 TCP 连接，使用传入的端口
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    char portstr[8];
    snprintf(portstr, sizeof(portstr), "%d", port);
    if (getaddrinfo(hostname, portstr, &hints, &res) != 0) {
        perror("getaddrinfo 失败");
        goto cleanup;
    }

    for(p = res; p != NULL; p = p->ai_next) {
        sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sockfd == -1) {
            perror("socket 失败");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect 失败");
            close(sockfd);
            continue;
        }
        break;
    }
    freeaddrinfo(res);
    if (p == NULL) {
        fprintf(stderr, "无法连接到服务器\n");
        goto cleanup;
    }

    // 3. 创建 SSL 对象并关联
    ssl = SSL_new(ctx);
    if (!ssl) {
        fprintf(stderr, "无法创建 SSL 对象\n");
        close(sockfd);
        goto cleanup;
    }
    SSL_set_fd(ssl, sockfd);

    // 4. SSL 握手
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        fprintf(stderr, "SSL 握手失败\n");
        SSL_free(ssl);
        close(sockfd);
        goto cleanup;
    }

    printf("SSL 握手成功\n");

    // 5. 构造并发送 HTTP 请求
    // 如果使用非标准端口，将其加入 Host 头
    char host_header[256];
    if (port != 443) {
        snprintf(host_header, sizeof(host_header), "%s:%d", hostname, port);
    } else {
        snprintf(host_header, sizeof(host_header), "%s", hostname);
    }
    snprintf(request, sizeof(request), "%s %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", method, path_and_query, host_header);
    SSL_write(ssl, request, strlen(request));
    printf("已发送请求: %s", request);

    // 6. 读取响应
    int body_offset = read_full_response(ssl, response, sizeof(response));
    if (body_offset >= 0) {
        printf("\n--- 服务器响应 Body ---\n%s\n----------------------\n", response + body_offset);
        ret = 0; // 成功
    } else {
        printf("读取响应失败\n");
    }

    // 7. 清理
    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(sockfd);

cleanup:
    SSL_CTX_free(ctx);
    return ret;
}