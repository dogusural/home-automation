#include <iostream>
#include <stdlib.h>
#include <thread>         // std::thread
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <string>
#include <mutex>

#define MAX_THREAD_NUMBER 24
std::mutex network_mutex;

typedef struct
{
	int sock;
	struct sockaddr address;
	socklen_t addr_len;
} connection_t;

void process(connection_t * conn_ptr)
{
	int len = 4096;
	char * buffer = new char[4096];
	connection_t * conn;
	long addr = 0;
	if (!conn_ptr) 
		std::terminate(); 
	std::unique_lock<std::mutex> lock( network_mutex );
	conn = (connection_t *)conn_ptr;

	/* read length of message */
	
	addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;
	buffer[len] = 0;

	/* read message */

	int bytesReceived = recv(conn->sock, buffer, len, 0);
	if (bytesReceived == -1)
	{
		std::cerr << "Error in recv(). Quitting" << std::endl;
	}

	if (bytesReceived == 0)
	{
		std::cout << "Client disconnected " << std::endl;
	}

	std::cout << std::string(buffer, 0, bytesReceived) << std::endl;
 
	// Echo message back to client
	send(conn->sock, buffer, bytesReceived + 1, 0);
	/* print message */
	printf("%d.%d.%d.%d: %s\n",
		(int)((addr      ) & 0xff),
		(int)((addr >>  8) & 0xff),
		(int)((addr >> 16) & 0xff),
		(int)((addr >> 24) & 0xff),
		buffer);
	delete[] buffer;

	/* close socket and clean up */
	close(conn->sock);
	delete (conn);
}

int main(int argc, char ** argv)
{
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t * connection;
	std::thread thread[24];
	size_t thread_indice = 0;
	/* check for command line arguments */
	if (argc != 2)
	{
		fprintf(stderr, "usage: %s port\n", argv[0]);
		port = 54000;
	}

	/* obtain port number */
	// if (sscanf(argv[1], "%d", &port) <= 0)
	// {
	// 	fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
	// 	return -2;
	// }

	/* create socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
		return -3;
	}

	/* bind socket to port */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
		return -4;
	}

	/* listen on port */
	if (listen(sock, 5) < 0)
	{
		fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
		return -5;
	}

	printf("%s: ready and listening\n", argv[0]);
	
	while (1)
	{
		/* accept incoming connections */

		connection = new connection_t;
		connection->sock = accept(sock, &connection->address, &connection->addr_len);

		if (connection->sock <= 0)
		{

			delete (connection);
		}
		else
		{
			/* start a new thread but do not wait for it */
			thread[thread_indice++] = std::thread(process, connection);
			
		}

		if (thread_indice == MAX_THREAD_NUMBER)
		{
			printf("Joining threads\n");
			int i = 0;
			while(i < MAX_THREAD_NUMBER)
			{
				printf("Joining thread thread[%d]\n", i);
				thread[i++].join();
			}
			thread_indice = 0;
		}
	}
	
	return 0;
}