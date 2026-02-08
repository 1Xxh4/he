// server/server.c
#include "server.h"
#include "../common/database.h" // 已经在 server.h 中包含了，但再次包含也没坏处
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <openssl/ssl.h>

void handle_client_https(SSL *ssl) {
    char buffer[4096];
    int bytes_read;

    bytes_read = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (bytes_read <= 0) {
        int ssl_error = SSL_get_error(ssl, bytes_read);
        fprintf(stderr, "SSL_read failed or connection closed. Error code: %d\n", ssl_error);
        return;
    }
    buffer[bytes_read] = '\0';

    printf("收到 HTTPS 请求:\n%.500s...\n", buffer);

    char *method = strtok(buffer, " ");
    char *path = strtok(NULL, " ");
    strtok(NULL, "\r\n");

    if (!method || !path) {
        const char *bad_request_response = "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\nBad Request";
        SSL_write(ssl, bad_request_response, strlen(bad_request_response));
        return;
    }

    // 使用从 database.h 导入的宏
    char response_body[RESPONSE_BUFFER_SIZE] = {0};

    if (strcmp(path, "/list") == 0) {
        printf("处理 /list 请求\n");
        db_list_employees(response_body, sizeof(response_body));
    } else if (strncmp(path, "/add?", 5) == 0) {
        printf("处理 /add 请求\n");
        char *query_string = path + 5;
        char name[50] = {0}, sex[10] = {0}, birth[11] = {0}, dept[50] = {0};
        int items_parsed = sscanf(query_string, "name=%49[^&]&sex=%9[^&]&birth=%10[^&]&dept=%49s", name, sex, birth, dept);
        
        if (items_parsed == 4) {
            db_add_employee(name, sex, birth, dept, response_body, sizeof(response_body));
        } else {
            snprintf(response_body, sizeof(response_body), "格式错误！用法: /add?name=姓名&sex=性别&birth=出生日期&dept=部门\n");
        }
    } else if (strncmp(path, "/del/", 5) == 0) {
        printf("处理 /del 请求\n");
        const char *num_str = path + 5;
        int serial_number = atoi(num_str);

        if (serial_number > 0) {
            db_delete_employee_by_serial(serial_number, response_body, sizeof(response_body));
        } else {
            strcpy(response_body, "格式错误！用法: /del/序号 (序号必须为正整数)\n");
        }
    } else {
        strcpy(response_body, "404 Not Found\n");
    }

    char response_header[512];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain; charset=utf-8\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             strlen(response_body));

    SSL_write(ssl, response_header, strlen(response_header));
    SSL_write(ssl, response_body, strlen(response_body));
    
    printf("发送 HTTPS 响应完成。\n");
}