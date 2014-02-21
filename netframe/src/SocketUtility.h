#ifndef SOCKET_UTILITY_H
#define SOCKET_UTILITY_H

#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <stdint.h>


int listen_to(const char* ip, int port, struct sockaddr_in* addr, int is_block);
int bind_to(const char* ip, int port, struct sockaddr_in* addr);
int connect_to(const char* ip, int port, struct sockaddr_in* addr);
int nonblock_connect_to(const char * ip,int port, struct sockaddr_in* addr);

int accept_client(int fd, struct sockaddr_in* addr);
void make_address(struct sockaddr_in* addr, const char* ip, int port);

int recv_data(int fd, char* buffer, int buffer_len);
int send_data(int fd, const char* buffer, int buffer_len);

int recv_data2(int fd, char* buffer, int buffer_len, struct sockaddr* from, socklen_t* fromlen);
int send_data2(int fd, const char* buffer, int buffer_len, struct sockaddr* to, socklen_t tolen);

char* ip_to_string(uint32_t ip);

int set_block(int fd);
int set_nonblock(int fd);
int set_reuse(int fd);
int set_recv_timeout(int fd, int timeout_ms);
int set_send_timeout(int fd, int timeout_ms);
int set_recv_bufsize(int fd, int bufsize);
int set_send_bufsize(int fd, int bufsize);
int set_linger(int fd, int onoff, int linger);

int create_socket();
int create_socket_udp();
int close_socket(int fd);

void get_peer_ipport(int sock_fd, unsigned int* ip, unsigned int* port);
void get_local_ipport(int sock_fd, unsigned int* ip, unsigned int* port);
void addr_to_ipport(struct sockaddr_in* addr, char* ip, uint32_t* port);

#endif // SOCKET_UTILITY_H

