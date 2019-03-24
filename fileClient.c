//////////////////////////////////////////////////////    
// client.c 询问上传还是下载　进行对应操作      
// 0 is itself ; 1 is address ; 2 is status ; 3 is filename ; 
// ///////////////////////////////////////////////////    
#include<netinet/in.h>                         // for sockaddr_in    
#include<sys/types.h>                          // for socket    
#include<sys/socket.h>                         // for socket    
#include<stdio.h>                              // for printf    
#include<stdlib.h>                             // for exit    
#include<string.h>                             // for bzero    
#include<time.h> 
#include<sys/time.h>

#define HELLO_WORLD_SERVER_PORT       5555   
#define BUFFER_SIZE                   1024    
#define FILE_NAME_MAX_SIZE            512    
    
int main(int argc, char **argv)    // 0 is itself ; 1 is address ; 2 is status ; 3 is filename ;
{    
    if (argc != 4)    
    {    
        printf("Usage: ./%s ServerIPAddress\n", argv[0]);    
        exit(1);    
    }

//   clock_t start,finish;
//  double duration;  
    
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
        printf("socket connect successfully.\n");  
      
    char status[3];
    //printf(" input get ? \t put ?;\n");    
    //scanf("%s", status);  
    //printf("status == %s\n", status);  
    strncpy(status, argv[2], strlen(argv[2]) > sizeof(status) ? sizeof(status) : strlen(argv[2]));
    send(client_socket, status, sizeof(status), 0);       
      
      
    char file_name[FILE_NAME_MAX_SIZE + 1];    
    bzero(file_name, sizeof(file_name));    
    //printf("Please Input File Name .\t");    
    //scanf("%s", file_name);    
    //printf("输入的文件名为：%s\n",file_name);  
    strncpy(file_name, argv[3], strlen(argv[3]) > sizeof(file_name) ? sizeof(file_name) : strlen(argv[3]));
    char buffer[BUFFER_SIZE];  
    bzero(buffer, sizeof(buffer));    
    strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));    
          
    // 向服务器发送buffer中的数据，此时buffer中存放的是客户端需要接收(发送)的文件的名字    
    send(client_socket, buffer, BUFFER_SIZE, 0);   
    FILE *fp;
if(!strncmp(status,"get",3)) //下载文件；  
{  
      
    fp = fopen(file_name, "w");    
    if (fp == NULL)    
    {    
        printf("File:\t%s Can Not Open To Write!\n", file_name);    
        exit(1);    
    }    
    
    // 从服务器端接收数据到buffer中     
bzero(buffer, BUFFER_SIZE);   
    int length = 0;   
    while((length = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)   
    {   
        if(fwrite(buffer, sizeof(char), length, fp) < length)   
        {   
            printf("File:\t%s Write Failed\n", file_name);   
            break;   
        }   
        bzero(buffer, BUFFER_SIZE);
    }    
    
    printf("Recieve File:\t %s From Server[%s] Finished!\n", file_name, argv[1]);    
  
}  
else //else 上传  
{
        //start = clock();  
        fp = fopen(file_name, "r");  
        if (fp == NULL)  
        {  
            printf("File:\t%s Not Found!\n", file_name);  
        }  
        else  
        {  
            bzero(buffer, BUFFER_SIZE);//清空  
            int file_block_length = 0;  
            while( (file_block_length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)  
            {  
                //每次读1024个字节的字符， 就将其发送出去；  
                //发送buffer中的字符串到 client_socket,实际上就是发送给fuwu端  
                if (send(client_socket, buffer, file_block_length, 0) < 0)  
                {  
                    printf("Send File:\t%s Failed!\n", file_name);  
                    break;  
                }  
  
                bzero(buffer, sizeof(buffer));  
            }  
            //fclose(fp);   //报错 double  free   
            //finish=clock();
            //duration=(double)(finish-start)/ CLOCKS_PRE_SEC;
            printf("File:\t%s Transfer Finished!\n", file_name);
            //printf("time= %f seconds\n",duration);  
        }  
        //goto back;  
      
}  
    
    // 传输完毕，关闭socket     
    fclose(fp);    
    close(client_socket);     
    return 0;    
    
} 
