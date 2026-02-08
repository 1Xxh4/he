// client/client.h
#ifndef CLIENT_H
#define CLIENT_H

// 发起 HTTPS 请求的函数
int perform_https_request(const char *hostname, int port, const char *path_and_query, const char *method);

#endif