/*
 * It does not need many changes. Just create a thread and pass to 
 * it any required attibutes (the file deescriptor actuually). Then
 * each of the threads will have its own local buffers, so there will
 * also be no need for locks. 
 *
 * The server function is the one that serves the clients.
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SERV_PORT 1332 /*port*/
#define LISTENQ 5 /*maximum number of waiting client connections */

#define MSG_LINE 256 // Lngth of printable messages
#define OP_LEN 21 // Max length of an operand
#define DELIMETER " " // Second argument of the strtok function

// Below are the protocal orders
#define CMD_EXIT '!'
#define CMD_ADD '+'
#define CMD_REDUCE '-'
#define CMD_DIV '/'
#define CMD_MUL '*'
#define ERROR "Error, wrong command!"

struct request_t {
    char operation;
    char operand_left[OP_LEN];
    char operand_right[OP_LEN];
};

struct reply_t {
    char result[OP_LEN + 1];
};

struct thread_args {
    int fd;
};

// Seting up the listener at the specified port
void setup_listener(int * fd, struct sockaddr_in * addr)
{
    *fd = socket (AF_INET, SOCK_STREAM, 0);

    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    addr->sin_port = htons(SERV_PORT);

    if (bind(*fd, (struct sockaddr *) addr, sizeof(*addr)) < 0)
    {
        printf("Unable to bind at port %d\n", SERV_PORT);
        exit(1);
    }
    

    listen(*fd, LISTENQ);
}

void 
accept_connections(int * connfd, int listenfd, struct sockaddr_in * cliaddr, socklen_t * clilen)
{
    printf("Server running...waiting for connections.\n");
    *clilen = sizeof(*cliaddr);
    *connfd = accept (listenfd, (struct sockaddr *) cliaddr, clilen);
    printf("Connection accepted ... \n");
}

// Returns a debugging str string anf the answer to the server reply
struct reply_t process_request(struct request_t request, char * str)
{
    struct reply_t reply;
    double val1, val2;

    switch (request.operation)
    {
        case '+':
            strcpy(str, "Addition OK");
            val1 = strtod(request.operand_left, NULL);
            val2 = strtod(request.operand_right, NULL);
            sprintf(reply.result, "%f", val1 + val2);
            break;
        case '*':
            strcpy(str, "Addition OK");
            val1 = strtod(request.operand_left, NULL);
            val2 = strtod(request.operand_right, NULL);
            sprintf(reply.result, "%f", val1 * val2);
            break;
        case '/':
            strcpy(str, "Addition OK");
            val1 = strtod(request.operand_left, NULL);
            val2 = strtod(request.operand_right, NULL);
            sprintf(reply.result, "%f", val1 / val2);
            break;
        case '-':
            strcpy(str, "Addition OK");
            val1 = strtod(request.operand_left, NULL);
            val2 = strtod(request.operand_right, NULL);
            sprintf(reply.result, "%f", val1 - val2);
            break;
        case '!':
            strcpy(str, "EXIT");
            break;
        default:
            strcpy(str, "Something wrong");
            strcpy(reply.result, "Something wrong");
            break;
    }

    return reply;
}

// The parallel function
void * serve(void * thread_args)
{
    struct request_t request;
    struct reply_t reply;
    struct thread_args * targs = (struct thread_args *) thread_args;
    char * printable_str;
    int n;
    int connfd = targs->fd;
        

    while (1)
    {
        // These exist, only to safeguard that the buffer is emptied
        printable_str = (char *) calloc(MSG_LINE, sizeof(char));

        //does not work for empty string
        // Receive request
        n = recv(connfd, &request, sizeof(struct request_t), 0);

        reply = process_request(request, printable_str);

        // Check if commanf iis wrong
        if (strncmp(printable_str, ERROR, strlen(ERROR)) == 0)
        {
            puts("Internal server error! Unknown comamnd!");
            continue;
        }
        else if (strncmp(printable_str, "EXIT", strlen("EXIT")) == 0)
            break;

        //debugging
        printf("%s\n", printable_str);
        puts(reply.result); // Print the string to send, only

        //process request here
        send(connfd, &reply, sizeof(struct reply_t), 0);

        free(printable_str);
    }   

    close(connfd);
    printf ("Connection closed ...\n");

}

int main (int argc, char **argv)
{
    int listenfd, n;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    setup_listener(&listenfd, &servaddr);

    while (1) 
    {
        int connfd; // Made this local for each thad
        pthread_t tid;
        struct thread_args th_args;

        // Wait for connections
        accept_connections(&connfd, listenfd, &cliaddr, &clilen);
        
        th_args.fd = connfd;

        pthread_create(&tid, NULL, serve, &th_args);

        // No join, because we do not want to wait for it to finish
    }

    //close listening socket
    close (listenfd);
}
