//实现聊天室的A端
//刘浩远
//12月16日
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/errno.h>

#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

#include<pthread.h>
#include<time.h>
//设置基本的配置参数
#define SEND_PORT 6000
#define RECV_PORT 6001
#define IP_SND "10.0.0.2"
#define IP_RCV "10.0.0.1"
#define SIZE 100
//声明两个线程函数，一个收，一个发,还有一个创建socket的函数
void *thread_send(void *fd);
void *thread_recv(void *fd);
//这里可以声明两个发送、接收缓存
char sendbuf[SIZE];
char recvbuf[SIZE];
time_t lt;
//这里写主函数
int main()
{
    lt=time(NULL);
    //建立发送线程
    pthread_t pthread_send;
    int fd_send=pthread_create(&pthread_send,NULL,thread_send,NULL);
    if(fd_send!=0)
    {
        printf("Thread sent created error!\n");
    }
    //建立接收线程
    pthread_t pthread_recv;
    int fd_recv=pthread_create(&pthread_recv,NULL,thread_recv,NULL);
    if(fd_recv!=0)
    {
        printf("Thread recv created error!\n");
    }
    while(1)
    {

    }
    exit(0);
    return EXIT_SUCCESS;
}
//这里写一个发送的线程函数
void *thread_send(void *fd)
{
    int sock_send=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in b_addr;
    b_addr.sin_family=AF_INET;
    b_addr.sin_port=htons(SEND_PORT);
    b_addr.sin_addr.s_addr=inet_addr(IP_SND);
    memset(sendbuf,0,sizeof(sendbuf));
    while(fgets(sendbuf,sizeof(sendbuf),stdin)!=0)
    {
        sendto(sock_send,sendbuf,sizeof(sendbuf),0,(struct sockaddr *)&b_addr,sizeof(struct sockaddr_in));
        memset(sendbuf,0,sizeof(sendbuf));
    }
    exit(0);
    free(fd);
    pthread_exit(NULL);
}
//这里写一个接收的线程函数
void *thread_recv(void *fd)
{
    int sock_recv=socket(AF_INET,SOCK_DGRAM,0);
    struct sockaddr_in a_addr;
    a_addr.sin_family=AF_INET;
    a_addr.sin_port=htons(RECV_PORT);
    a_addr.sin_addr.s_addr=inet_addr(IP_RCV);
    bind(sock_recv,(struct sockaddr *)&a_addr,sizeof(struct sockaddr_in));
    memset(recvbuf,0,sizeof(recvbuf));
    while(1)
    {
        recvfrom(sock_recv,recvbuf,sizeof(recvbuf),0,(struct sockaddr *)&a_addr,(void *)sizeof(struct sockaddr_in));
        printf("B:%s",recvbuf);
        printf("___________________");
        printf(ctime(&lt),stdout);
        memset(recvbuf,0,sizeof(recvbuf));
    }
    exit(0);
    free(fd);
    pthread_exit(NULL);
}

