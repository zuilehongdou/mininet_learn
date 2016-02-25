#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>
#include <netdb.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <resolv.h>
#include <signal.h>
#include <getopt.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

//首先定义网卡端口
#define PHYSICALPORT "h2-eth0"
#define PHYSICALPORT_LEN 30

//创建实际网卡端口数组
char physical_port[30];

//定义缓存大小
#define BUFSIZE 1024*5
char sendbuf[BUFSIZE]={0};
char recvbuf[BUFSIZE]={0};

//shengming 
void makepkg(char *hdr,char *buffer);
void ethernet_setpormisc(int fd,int i_flags);
time_t lt;
//创建发送函数线程
void *thread_send()
{
    //首先建立socket
    int sock_send;
    sock_send=socket(PF_PACKET,SOCK_PACKET,htons(ETH_P_ALL));
    if(sock_send<0)
    {
        perror("scoket created");   
    }
    
    //设置发包地址
    struct sockaddr send_addr;
    memset(&send_addr,0,sizeof(send_addr));
    strcpy(send_addr.sa_data,physical_port);
	char hdr[4]="TO_A";

    //创建发送程序
    while(fgets(sendbuf,sizeof(sendbuf),stdin)!=0)
    {
		makepkg(hdr,sendbuf);
        //printf("%s",sendbuf);
        int len=sendto(sock_send,&sendbuf,strlen(sendbuf),0,&send_addr,sizeof(send_addr));
        //printf("message sent!\n");
        memset(sendbuf,0,sizeof(sendbuf));
    }
    close(sock_send);
    pthread_exit(NULL);
}


//创建一个接收的线程
void *thread_recv()
{
	socklen_t recv_socket_len=0;
    //首先创建一个原始套接字
    int sock_recv;
    if((sock_recv=socket(PF_PACKET,SOCK_RAW,htons(ETH_P_ALL)))<0)
    {
        perror("physicl socket created");
    }
    
    //设置网卡为混杂模式；
    ethernet_setpormisc(sock_recv,1);

	//setsockopt
	int recvbuf_size=BUFSIZE;
	setsockopt(sock_recv,SOL_SOCKET,SO_RCVBUF,&recvbuf,sizeof(char));
    //获取物理网卡接口索，用以传输数据
    struct ifreq ifr_recv;
    strcpy(ifr_recv.ifr_name,physical_port);
    if(ioctl(sock_recv,SIOCGIFINDEX,&ifr_recv)<0)
    {
        perror("[3]get interface index");
    }   
    //绑定物理网卡
    struct sockaddr_ll local_addr;
    local_addr.sll_family=PF_PACKET;
    local_addr.sll_ifindex=ifr_recv.ifr_ifindex;
    local_addr.sll_protocol=htons(ETH_P_ALL);
    if((bind(sock_recv,(struct sockaddr *)&local_addr,sizeof(local_addr)))<0)
    {
        perror("[4]bind physical address");
    }   
    
    //开始接收数据包
    while(1)
    {
		bzero(recvbuf,BUFSIZE);
        recvfrom(sock_recv,recvbuf,BUFSIZE,0,NULL,NULL);
        if(recvbuf[0]=='T'&&recvbuf[1]=='O'&&recvbuf[2]=='_'&&recvbuf[3]=='B')
        {
            printf("Recived:%s",recvbuf+4);
			printf("___________________");
        	printf(ctime(&lt),stdout);
        }
		memset(recvbuf,0,sizeof(recvbuf));
    }   
    exit(0);
    close(sock_recv);
    pthread_exit(NULL);
}


//创建设置网卡混杂模式函数
void ethernet_setpormisc(int fd,int i_flags)
{
    //首先获取网卡接口标志位
    struct ifreq ifr_s;
	//printf("1.%s\n\n",physical_port);
    memcpy(ifr_s.ifr_name,physical_port,sizeof(physical_port));
	//printf("2.%s",physical_port);//there is a problem canot printf it 
    if(ioctl(fd,SIOCGIFFLAGS,&ifr_s)<0)
    {
        perror("[1]get interface flags");
    }
    if(i_flags==0)
    {
        //取消混杂模式
        ifr_s.ifr_flags &= ~IFF_PROMISC;
    }
    else
    {
        //设置为混杂模式
        ifr_s.ifr_flags |= IFF_PROMISC;
    }   
    
    //将接口设置为相应的模式
    if(ioctl(fd,SIOCSIFFLAGS,&ifr_s)<0)
    {
        perror("[2]set interface flags");
    }
}

//创建一个makepackge的组包程序
void makepkg(char *hdr,char *buffer)
{
    int i=0;
    char temp[BUFSIZE]; 
    memcpy(temp,sendbuf,BUFSIZE);
    memcpy(sendbuf,hdr,sizeof(hdr));
    memcpy((sendbuf+sizeof(hdr)),temp,sizeof(temp));
}


//创建主函数
void main()
{
	lt=time(NULL);	

    //根据声明定义好网卡端口号
    memcpy(physical_port,PHYSICALPORT,PHYSICALPORT_LEN);
    
    //创建发送线程函数
    pthread_t pthread_send;
    int fd_send=pthread_create(&pthread_send,NULL,thread_send,NULL);
    if(fd_send<0)
    {
        perror("thread_send created");
    } 
    
    //创建接收线程函数
    pthread_t pthread_recv;
    int fd_recv=pthread_create(&pthread_recv,NULL,thread_recv,NULL);
    if(fd_recv<0)
    {
        perror("thread_recv created");
    } 
    //创建循环
    while(1)
    {

    }
}
