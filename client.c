#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<signal.h>
#include<pthread.h>
#define PORT 1821
#define BUFFER_SIZE 1024

int sockfd,sendbytes,recvbytes;
struct hostent *host;
struct sockaddr_in serv_addr;
char send_buf[BUFFER_SIZE],recv_buf[BUFFER_SIZE];

/*
void handle(){
	if((recvbytes = recv(sockfd,recv_buf,BUFFER_SIZE,0)) > 0){
		//printf("Received a message from %d: %s\n",sockfd,recv_buf);
		//perror("recv");
		printf("\t\t\t\t%s\n",recv_buf);
	}	

}

void *thread_send(void *arg){
	while(1){
		memset(send_buf,0,sizeof(send_buf));
		scanf("%s",send_buf);
	
		setbuf(stdin,NULL);
		//c = getchar();
		//printf("send_buf %s\n",send_buf);
		if((sendbytes = send(sockfd,send_buf,strlen(send_buf),MSG_DONTWAIT))==-1){
			perror("send");
			exit(1);
		}
	}
	//pthread_exit(NULL);
}
*/

void *thread_recv(void *arg){
	while(1){
		memset(recv_buf,0,sizeof(recv_buf));
		if((recvbytes = recv(sockfd,recv_buf,BUFFER_SIZE,MSG_DONTWAIT)) > 0){
			//printf("Received a message from %d: %s\n",sockfd,recv_buf);
			//perror("recv");
			printf("\t\t\t\t%s\n",recv_buf);
		}
	}
	//pthread_exit(NULL);
}

int main(int argc,char *argv[]){

 	if(argc < 2){
		fprintf(stderr,"USAGE: ./client Hostname(or ip address)\n");
		exit(1);	
	}

	//地址解析函数，将IP转化为主机名
	if((host = gethostbyname(argv[1])) == NULL){
		perror("gethostbyname");
		exit(1);
	}
	
	//创建socket
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("socket");
		exit(1);
	}
	printf("\nThis is client!!!\n");
	//printf("Socket id = %d\n",sockfd);

	//设置sockaddr_in结构体中的相关参数
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
	memset(serv_addr.sin_zero,0,8);
	
	//调用connect()函数主动发起对服务器端的连接
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr)) == -1){
		perror("connect");
		exit(1);
	}

	printf("Connect success!\n");
	printf("----------------------------------------------------\n");
	printf("\t\tLet Us Start Chatting!!!\n");
	printf("----------------------------------------------------\n");
	printf("   ME\t\t\t\t\tOTHERS\n");

	pthread_t recvthread;
	int res;
	//创建新的线程recvthread,该线程调用thread_recv()函数，来接收服务器端发送的消息
	res = pthread_create(&recvthread,NULL,thread_recv,NULL);
	if(res != 0){
		printf("Create thread recvthread failed!\n");
		exit(1);
	}

	while(1){
		
		memset(send_buf,0,sizeof(send_buf));

		scanf("%s",send_buf);
		//清空缓冲区
		setbuf(stdin,NULL);
		
		//调用send()函数发送输入的字符序列
		if((sendbytes = send(sockfd,send_buf,strlen(send_buf),MSG_DONTWAIT))==-1){
			perror("send");
			exit(1);
		}
		
	}
		
	close(sockfd);
	exit(0);
}

