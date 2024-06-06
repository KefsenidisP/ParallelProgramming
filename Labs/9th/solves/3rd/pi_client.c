#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 64 // Size of the request_t reply_t char arrays
#define MSG_PROMPT_SIZE 128 // The maximum size of prompt messages

// Protocol statements
#define EXIT -1
#define CALC_PI 1

// Error statements
#define ERROR 100
#define OK 0

struct request_t {
    int code; // -1 for exit, 1 for calculation of pi
    char iters[BUFFER_SIZE]; // Number of iterations
};

struct reply_t {
    int code; // This can be ERROR or OK
    double result; // Pi value
};

// Socket initialization
void connect_to_server(int * fd, struct sockaddr_in * addr, char * argv[])
{
    *fd = socket (AF_INET, SOCK_STREAM, 0);

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr= inet_addr(argv[1]);
    addr->sin_port =  htons(strtol(argv[2], NULL, 10)); //convert to big-endian order

    if (connect(*fd, (struct sockaddr *) addr, sizeof(*addr)) < 0)
    {
        puts("Connection failed!");
        exit(2);
    }
}

// It makes a reqeuest to the server. Also returns the request code
int make_request(int fd)
{
    char line[BUFFER_SIZE];
    int iters;
    struct request_t request;

    while (1)
    {
        printf("Enter your iteration number: ");

        fgets(line, BUFFER_SIZE, stdin);

        iters = strtol(line, NULL, 10);

        // Set the protocol to follow
        if (iters > 0)
        {
            request.code = CALC_PI;
            break;
        }
        else if (iters == EXIT)
        {
            request.code = EXIT;
            break;
        }
    }

    // Copy the number of iterations to the request variable
    strcpy(request.iters, line);

    // Send the request
    send(fd, &request, sizeof(struct request_t), 0);

    return request.code;
}

// That is mainly for debugging, it just prints a string to the user
void process_protocol_client(int code)
{
    if (code == CALC_PI)
        puts("Calculating pi...");
    else if (code == EXIT)
    {
        puts("Closing connection...");
        exit(0); // It will close all file descriptors open
    }
}

void process_reply(int fd)
{
    struct reply_t reply;

    recv(fd, &reply, sizeof(struct reply_t), 0);

    if (reply.code == OK)
        printf("\nCalculated pi: %.16f\n", reply.result);
    else if (reply.code == ERROR)
        puts("\n An error occured during pi calculation!");
}

int main(int argc, char ** argv)
{
    int sockfd;
    char operator;
    struct sockaddr_in servaddr;
    struct request_t * request;
    char * printable_str;

    if (argc !=3) 
    {
        printf("Usage: TCPClient <IP address> <port number>\n");
        exit(1);
    }

    // Initialize socket
    connect_to_server(&sockfd, &servaddr, argv);
    puts("Send a positive number and you will get the number pi");

    while (1)
    {
        int code;
        
        code = make_request(sockfd);
        process_protocol_client(code);
        process_reply(sockfd);
    }
}
