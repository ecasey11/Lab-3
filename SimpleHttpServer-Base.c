#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PORT 9001
#define QUEUE_MAX_COUNT 5
#define BUFF_SIZE 99999


#define SERVER_STRING "Server: CISC4615 Simple Http Server\r\n"

int read_file_to_buffer(char* filename, char buffer[BUFF_SIZE])
{
    size_t len;
    FILE *fp = fopen(filename, "r");
    if (fp != NULL)
    {
        len = fread(buffer, sizeof(char), 7000000, fp);
        if (ferror(fp) != 0)
        {
            return -1;
        } else
        {
            buffer[len] = '\0';
        }
    }
    else
    {
        return -1;
    }
    // Clean exit.
    fclose(fp);
    return len;
}

int main()
{

    /* socket descriptor */
    int server_fd = -1;
    int client_fd = -1;

    u_short port = PORT;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    char buf[BUFF_SIZE*10];
    char recv_buf[BUFF_SIZE];
    int received_len = 0;

    /* create a socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(-1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    /* tcp/ip */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* bind */
    if (bind(server_fd, (struct sockaddr *)&server_addr,
         sizeof(server_addr)) < 0) {
        perror("bind");
        exit(-1);
    }

    /* start socket */
    if (listen(server_fd, QUEUE_MAX_COUNT) < 0) {
        perror("listen");
        exit(-1);
    }


    printf("http server running on port %d\n", port);

    while (1)
    {
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                   &client_addr_len);
        if (client_fd < 0)
        {
            perror("accept");
            exit(-1);
        }
        printf("accept a client\n");
        printf("client socket fd: %d\n", client_fd);
        if(!fork())
        {
        close(server_fd);
        received_len = recv(client_fd, recv_buf, BUFF_SIZE, 0);
        printf("receive %d\n", received_len);
        printf("receive: --- \%s\n", recv_buf);

        if(strncmp(recv_buf, "GET / ",6) == 0)
        {
            sprintf(buf, "HTTP/1.0 200 OK\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, SERVER_STRING);
            send(client_fd, buf, strlen(buf), 0);
            sprintf(buf, "Content-Type: text/html\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, "\r\n");
            send(client_fd, buf, strlen(buf), 0);
            sprintf(buf, "Hello World!");
            send(client_fd, buf, strlen(buf), 0);

            int len1 = read_file_to_buffer("index.html",buf);
            send(client_fd, buf, len1, 0);
        }

        // you should write the similar code to transfer css files to the browser
	else if(strncmp(recv_buf, "GET /style.css", 14) == 0)
	{
	    printf("*******Send CSS********\n");
	    sprintf(buf, "HTTP/1.0 200 OK\r\n");
	    send(client_fd, buf, strlen(buf), 0);
	    strcpy(buf, SERVER_STRING);
	    send(client_fd, buf, strlen(buf), 0);
	    sprintf(buf, "Content Type: text/css\r\n");
	    send(client_fd, buf, strlen(buf), 0);
	    strcpy(buf, "\r\n");
	    send(client_fd, buf, strlen(buf), 0);
		
	    int len2 = read_file_to_buffer("style.css", buf);
	    if(send(client_fd, buf, len2, 0) != -1)
	    {
		printf("Successfully sent CSS!\n");
	    }
	}

        else if(strncmp(recv_buf, "GET /fordham_logo.png", 20) == 0)
        {
            printf("********Send png********\n");
            sprintf(buf, "HTTP/1.0 200 OK\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, SERVER_STRING);
            send(client_fd, buf, strlen(buf), 0);
            sprintf(buf,"Content-Type: image/png\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, "\r\n");
            send(client_fd, buf, strlen(buf), 0);

            int len2 = read_file_to_buffer("fordham_logo.png",buf);
            if(send(client_fd, buf, len2, 0)!= -1)
            {
                printf("Successfully sent png.\n");
            }
        }
        else if(!strncmp(recv_buf, "GET /app.js", 10))
        {
            printf("********Send js********\n");
            sprintf(buf, "HTTP/1.0 200 OK\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, SERVER_STRING);
            send(client_fd, buf, strlen(buf), 0);
            sprintf(buf,"Content-Type: application/javascript\r\n");
            send(client_fd, buf, strlen(buf), 0);
            strcpy(buf, "\r\n");
            send(client_fd, buf, strlen(buf), 0);

            int len3 = read_file_to_buffer("app.js",buf);
            if(send(client_fd, buf, len3, 0)!= -1)
            {
                printf("Successfully sent js.\n");
            }
        }

        // you should write similar code to transfer .mp4 files to the browser
	else if (!strncmp(recv_buf, "GET /FordhamUniversity.mp4", 26))
	{
	    printf("********SEND MP4**********\n");
	    sprintf(buf, "HTTP/1.0 200 OK\r\n");
	    send(client_fd, buf, strlen(buf), 0);
	    strcpy(buf, SERVER_STRING);
	    send(client_fd, buf, strlen(buf), 0);
	    sprintf(buf, "Content type: video/mp4\r\n");
	    send(client_fd, buf, strlen(buf), 0);
	    strcpy(buf, "\r\n");
	    send(client_fd, buf, strlen(buf), 0);

	    char *buf2 = (char*) malloc (BUFF_SIZE*1000);
	    int len4 = read_file_to_buffer("FordhamUniversity.mp4", buf2);
	    if(send(client_fd, buf2, len4, 0)!= -1)
	    {
		printf("Successfully sent mp4!\n");
	    }
	}

        else
        {
            sprintf(buf, "HTTP/1.0 404 Not Found\r\n");
            send(client_fd, buf, strlen(buf), 0);
        }

    }
         close(client_fd);

    }

    return 0;
}
