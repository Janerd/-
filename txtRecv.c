#include<netinet/in.h>  
#include<sys/types.h>  
#include<sys/socket.h>  
#include<stdio.h>  
#include<stdlib.h>  
#include<string.h>  
  
#define HELLO_WORLD_SERVER_PORT    6666  
#define LENGTH_OF_LISTEN_QUEUE     20  
#define BUFFER_SIZE                1024  
  
int main(int argc, char **argv)  
{  
    // set socket's address information  
    // 设置一个socket地址结构server_addr,代表服务器internet的地址和端口  
    struct sockaddr_in   server_addr;  
    bzero(&server_addr, sizeof(server_addr));  
    server_addr.sin_family = AF_INET;  
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);  
    server_addr.sin_port = htons(HELLO_WORLD_SERVER_PORT);  
  
    // create a stream socket  
    // 创建用于internet的流协议(TCP)socket，用server_socket代表服务器向客户端提供服务的接口  
    int server_socket = socket(PF_INET, SOCK_STREAM, 0);  
    if (server_socket < 0)  
    {  
        printf("Create Socket Failed!\n");  
        exit(1);  
    }  
  
    // 把socket和socket地址结构绑定  
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)))  
    {  
        printf("Server Bind Port: %d Failed!\n", HELLO_WORLD_SERVER_PORT);  
        exit(1);  
    }  
  
    // server_socket用于监听  
    if (listen(server_socket, LENGTH_OF_LISTEN_QUEUE))  
    {  
        printf("Server Listen Failed!\n");  
        exit(1);  
    }  
  
    // 服务器端一直运行用以持续为客户端提供服务  
  
        // 定义客户端的socket地址结构client_addr，当收到来自客户端的请求后，调用accept  
        // 接受此请求，同时将client端的地址和端口等信息写入client_addr中  
        struct sockaddr_in client_addr;  
        socklen_t          length = sizeof(client_addr);  
  
        // 接受一个从client端到达server端的连接请求,将客户端的信息保存在client_addr中  
        // 如果没有连接请求，则一直等待直到有连接请求为止，这是accept函数的特性，可以  
        // 用select()来实现超时检测  
        // accpet返回一个新的socket,这个socket用来与此次连接到server的client进行通信  
        // 这里的new_server_socket代表了这个通信通道  
        int new_server_socket = accept(server_socket, (struct sockaddr*)&client_addr, &length);  
        if (new_server_socket < 0)  
        {  
            printf("Server Accept Failed!\n");  
            exit(0);
            //break;  
        }  
          
        char buffer[BUFFER_SIZE];   
        bzero(buffer, sizeof(buffer));  
        length = recv(new_server_socket, buffer, BUFFER_SIZE, 0);   //从客户端接收数据，数据是文件名。  
        printf("recv text from client: %s\n", buffer);  
        //sleep(10);  
    //  bzero(buffer, sizeof(buffer));    
        if (length < 0)  
        {  
            printf("Server Recieve Data Failed!\n");  
            exit(0);
            //break;  
        } 
        close(new_server_socket);
    

    close(server_socket);
    return 0;
}     

          