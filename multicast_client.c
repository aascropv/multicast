/* Receiver/client multicast Datagram example. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <arpa/inet.h>
 
struct sockaddr_in localSock;
struct ip_mreq group;
int sd;
int datalen;
char databuf[1024];
 
int main(int argc, char *argv[])
{
/* Create a datagram socket on which to receive. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd < 0)
	{
		perror("Opening datagram socket error");
		exit(1);
	}
	else
	printf("Opening datagram socket....OK.\n");
		 
	/* Enable SO_REUSEADDR to allow multiple instances of this */
	/* application to receive copies of the multicast datagrams. */
	
	int reuse = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
	{
		perror("Setting SO_REUSEADDR error");
		close(sd);
		exit(1);
	}
	else
		printf("Setting SO_REUSEADDR...OK.\n");
	
	 
	/* Bind to the proper port number with the IP address */
	/* specified as INADDR_ANY. */
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(4321);
	localSock.sin_addr.s_addr = INADDR_ANY;
	if(bind(sd, (struct sockaddr*)&localSock, sizeof(localSock)))
	{
		perror("Binding datagram socket error");
		close(sd);
		exit(1);
	}
	else
		printf("Binding datagram socket...OK.\n");
	 
	/* Join the multicast group 226.1.1.1 on the local address*/
	/* interface. Note that this IP_ADD_MEMBERSHIP option must be */
	/* called for each local interface over which the multicast */
	/* datagrams are to be received. */
	group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
	/* your ip address */ 
	group.imr_interface.s_addr = inet_addr("127.0.0.1"); 
	/* IP_ADD_MEMBERSHIP:  Joins the multicast group specified */ 
	if(setsockopt(sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0)
	{
		perror("Adding multicast group error");
		close(sd);
		exit(1);
	}
	else
		printf("Adding multicast group...OK.\n");
	 
	/* Read from the socket. */
	datalen = sizeof(databuf);
	
	int ret;
	char recvbuf[1024] = {0};

	FILE *fp;
	fp = fopen("recv.txt", "w");

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	int recvtimeout = 0;

	while (1) {
		ret = recvfrom(sd, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
		if (ret <= 0)
			break;
		// printf("recv from serv. %d\n", ret);
		recvtimeout = setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));
		if (recvtimeout < 0)
			break;
		fprintf(fp, "%s", recvbuf);
		bzero(recvbuf, 1024);
	}

	fclose(fp);

	fp = fopen("recv.txt", "r");
	fseek(fp, 0, SEEK_END);
	int sizeOfRecv = ftell(fp);
	fclose(fp);
	
	printf("file size: %d KB.\n", sizeOfRecv);
	
	close(sd);
	
	// if(recvfrom(sd, databuf, datalen, 0, NULL, NULL) < 0)
	// {
	// 	perror("Reading datagram message error");
	// 	close(sd);
	// 	exit(1);
	// }
	// else
	// {
	// 	printf("Reading datagram message...OK.\n");
	// 	printf("The message from multicast server is: \"%s\"\n", databuf);
	// }
	return 0;
}
