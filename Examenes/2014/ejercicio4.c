#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/*ESQUEMA SERVIDOR UDP:
	- socket()
	- bind()
	- recvfrom()
	- sendto()
	- close()
*/

/*SERVIDOR*/
#define BUF_SIZE 256
int main(int argc, char** argv){

	struct addrinfo hints;
	struct addrinfo *res;
	int info, sd;
	int nread;
	char buf[BUF_SIZE];

	hints.ai_family = AF_UNSPEC; /*ipv4 o ipv6*/
	hints.ai_socktype = SOCK_DGRAM; /*udp*/
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	
	if(argc < 1) perror("Error de argumentos.\n");

	info = getaddrinfo("::", argv[1], &hints, &res);
	if(info != 0) perror("Error getaddrinfo.\n");
	
	sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	
	bind(sd, res->ai_addr, res->ai_addrlen);
	
	while(1){
		struct sockaddr_storage sock;
		socklen_t sock_len = sizeof(sock);
		char host[NI_MAXHOST];
		char serv[NI_MAXSERV];

		nread = recvfrom(sd, buf, BUF_SIZE, 0, (struct sockaddr *) &sock, &sock_len);
		buf[nread] = '\0';

		getnameinfo((struct sockaddr *) &sock, sock_len, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
		if(strcmp(buf, "a") == 0 || strcmp(buf, "a\n") == 0){
			sendto(sd, host, strlen(host), 0, (struct sockaddr *) &sock, sock_len);
		}
		else if(strcmp(buf, "p") == 0 || strcmp(buf, "p\n") == 0)		{
			sendto(sd, serv, strlen(serv), 0, (struct sockaddr *) &sock, sock_len);
		}
		else if(strcmp(buf, "q") == 0 || strcmp(buf, "q\n") == 0)		{
			sendto(sd, "ADIÓS\n", 6, 0, (struct sockaddr *) &sock, sock_len);
			freeaddrinfo(res);
			close(sd);
			_exit(1);
		}

		printf("Host: %s, Puerto: %s\n", host, serv);

	}
	freeaddrinfo(res);
	close(sd);

	return 0;
}
