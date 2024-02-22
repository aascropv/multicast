/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024] = "Multicast test message lol!";
int datalen = sizeof(databuf);

#define ERR_EXIT(msg)   \
    do {    \
        perror(msg);    \
        exit(EXIT_FAILURE); \
    } while(0)
 
int main (int argc, char *argv[ ])
{
/* Create a datagram socket on which to send. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 226.1.1.1 and port 4321. */
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(4321);
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr("127.0.0.1");
	
	/* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");


	FILE *fp;
	char *filename = argv[1];
	fp = fopen(filename, "r");

	if (fp == NULL) {
		perror("Error in reading file.\n");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	int sof = ftell(fp);
	
	fclose(fp);
	fp = fopen(filename, "r");
	
	char sendbuf[1024] = {0};

	while (fread(sendbuf, 1024, 1, fp)) {
	// fread(sendbuf, 1024, 1, fp);
		int ret;
		if ((ret = sendto(sd, sendbuf, sizeof(sendbuf), 0, (struct sockaddr *)&groupSock, sizeof(groupSock))) < 0) {
			perror("Error in sending file.\n");
		}
		// else
		// 	printf("send sendbuf. %d\n", ret);
		bzero(sendbuf, 1024);
	}
	fclose(fp);
	printf("file size: %d KB\n", sof);
	printf("File sent successfully.\n");
	close(sd);

	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */
	/*int datalen = 1024;*/	
	// if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
	// {
	// 	perror("Sending datagram message error");
	// }
	// else
	//   printf("Sending datagram message...OK\n");

	
	 
	/* Try the re-read from the socket if the loopback is not disable
	if(read(sd, databuf, datalen) < 0)
	{
	perror("Reading datagram message error\n");
	close(sd);
	exit(1);
	}
	else
	{
	printf("Reading datagram message from client...OK\n");
	printf("The message is: %s\n", databuf);
	}
	*/
	return 0;
}
