/// 文本发送端  1参为地址 2参为发送文本 2参传入程序后 发送
#include<netinet/in.h>                         // for sockaddr_in    
#include<sys/types.h>                          // for socket    
#include<sys/socket.h>                         // for socket    
#include<stdio.h>                              // for printf    
#include<stdlib.h>                             // for exit    
#include<string.h>                             // for bzero    
#include<time.h> 
#include<sys/time.h>

#define HELLO_WORLD_SERVER_PORT       6666   
#define BUFFER_SIZE                   1024     
    
int main(int argc, char **argv)    
{    
    if (argc != 3)    
    {    
        printf("Usage: ./%s ServerIPAddress\n", argv[0]);    
        exit(1);    
    }
    
    
    // 设置一个socket地址结构client_addr, 代表客户机的internet地址和端口    
    struct sockaddr_in client_addr;    
    bzero(&client_addr, sizeof(client_addr));    
    client_addr.sin_family = AF_INET; // internet协议族    
    client_addr.sin_addr.s_addr = htons(INADDR_ANY); // INADDR_ANY表示自动获取本机地址    
    client_addr.sin_port = htons(0); // auto allocated, 让系统自动分配一个空闲端口    
    
    // 创建用于internet的流协议(TCP)类型socket，用client_socket代表客户端socket    
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);    
    if (client_socket < 0)    
    {    
        printf("Create Socket Failed!\n");    
        exit(1);    
    }    
    
    // 把客户端的socket和客户端的socket地址结构绑定     
    if (bind(client_socket, (struct sockaddr*)&client_addr, sizeof(client_addr)))    
    {    
        printf("Client Bind Port Failed!\n");    
        exit(1);    
    }    
    
    // 设置一个socket地址结构server_addr,代表服务器的internet地址和端口    
    struct sockaddr_in  server_addr;    
    bzero(&server_addr, sizeof(server_addr));    
    server_addr.sin_family = AF_INET;    
    
    // 服务器的IP地址来自程序的参数     
    if (inet_aton(argv[1], &server_addr.sin_addr) == 0)    
    {    
        printf("Server IP Address Error!\n");    
        exit(1);    
    }    
    
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);    
    socklen_t server_addr_length = sizeof(server_addr);    
    
    
      
    
    // 向服务器发起连接请求，连接成功后client_socket代表客户端和服务器端的一个socket连接    
    if (connect(client_socket, (struct sockaddr*)&server_addr, server_addr_length) < 0)    
    {    
        printf("Can Not Connect To %s!\n", argv[1]);    
        exit(1);    
    }    
      
    else  
        printf("socket 连接成功.\n");  


    char buffer[BUFFER_SIZE];
    bzero(buffer,sizeof(buffer));
    strncpy(buffer, argv[2], strlen(argv[2]) > BUFFER_SIZE ? BUFFER_SIZE : strlen(argv[2]));
    send(client_socket , buffer , BUFFER_SIZE ,0);
    close(client_socket); 
    return 0;
}