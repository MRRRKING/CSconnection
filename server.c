#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#define PORT 1821
#define BUFFER_SIZE 1024
#define MAX_QUE_CONN_NM 5
#define MAX_SOCK_FD FD_SETSIZE
char buf[BUFFER_SIZE];

int main(){
	int client_queue[MAX_QUE_CONN_NM],num=0; //连接服务器的客户端个数
	struct sockaddr_in server_sockaddr,client_sockaddr;
	int sin_size,recvbytes,sendbytes;
	int sockfd,client_fd,fd,sendto_fd;
	int j;
	int flags;
	fd_set inset,tmp_inset;
	
	//建立socket连接
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
	
		perror("socket");
		exit(1);	
	}
	printf("\nThis is server!!!\n");
	printf("Socket id = %d\n",sockfd);

	//设置sockaddr_in结构体中相关参数
	server_sockaddr.sin_family = AF_INET;
	server_sockaddr.sin_port = htons(PORT);
	server_sockaddr.sin_addr.s_addr = INADDR_ANY;
	memset(server_sockaddr.sin_zero,0,8);
	
	//允许重复使用本地地址与套接字进行绑定
	int i = 1;
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(i));

	//绑定函数bind()
	if(bind(sockfd,(struct sockaddr *)&server_sockaddr,sizeof(struct sockaddr))==-1){
		perror("bind");
		exit(1);
	}
	printf("Bind success!\n");

	//调用listen()函数，创建未处理请求的队列
	if(listen(sockfd,MAX_QUE_CONN_NM) == -1){
		perror("listen");
		exit(1);
	}
	printf("Listening...\n");
 	
	//调用socket()函数的描述符作为文件描述符
	FD_ZERO(&inset);
	FD_SET(sockfd,&inset);

	while(1){
		tmp_inset = inset;
		sin_size = sizeof(struct sockaddr_in);
		memset(buf,0,sizeof(buf));
		//调用select()函数,来处理I/O多路复用问题
		if(!(select(MAX_SOCK_FD,&tmp_inset,NULL,NULL,NULL)>0)){
			perror("select");
		}

		for(fd = 0;fd<MAX_SOCK_FD;fd++){
			if(FD_ISSET(fd,&tmp_inset) > 0){
				if(fd == sockfd){//服务端接收客户端的连接请求
					
					if((client_fd = accept(sockfd,(struct sockaddr *)&client_sockaddr,&sin_size)) == -1){
						perror("accept");
						exit(1);
					}
					FD_SET(client_fd,&inset);
					printf("New connection from %d(socket)\n",client_fd);
					client_queue[num++] = client_fd;//将客户端socket　id记录在客户端队列中
					sendto_fd = client_fd;
				}
				else{//处理从客户端发来的消息

					for(j=0;j<num;j++){
						
						client_fd = client_queue[j];
						//printf("client_fd:%d\n",client_fd);
						
						//调用recv()函数接收客户端的消息，MSG_DONTWAIT设置其为非阻塞函数
						if((recvbytes = recv(client_fd,buf,BUFFER_SIZE,MSG_DONTWAIT)) > 0){
							printf("Received a message from %d: %s\n",client_fd,buf);
							sendto_fd = client_fd;

							//处理消息的来向，在消息队列中可以更清楚地看到消息是由哪个客户端发送的
							char from[10];
							memset(from,0,sizeof(from));
							sprintf(from,"(from socket %d)",client_fd);
							printf("%s\n",from);
							sprintf(buf,"%s %s",buf,from);
							printf("buf:%s\n",buf);

							break;
						}
					}
					

					for(j=0;j<num;j++){
						if(client_queue[j] != client_fd){
							sendto_fd = client_queue[j];
							if(strcmp(buf,"") != 0){
								if((sendbytes = send(sendto_fd,buf,strlen(buf),MSG_DONTWAIT))==-1){
									perror("send");
									exit(1);
								}
								printf("Send %s to %d success!\n",buf,sendto_fd);
							}
							//break;
						}
					}
					/*
					if((sendbytes = send(sendto_fd,buf,strlen(buf),MSG_DONTWAIT))==-1){
						perror("send");
						exit(1);
					}
					printf("Send %s to %d success!\n",buf,sendto_fd);*/
					
				}
			}
		}
	}

	close(sockfd);
	//printf("exit...\n");
	exit(0);

}
