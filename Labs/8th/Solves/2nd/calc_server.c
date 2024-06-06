#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define SERV_PORT 1337 /*port*/
#define LISTENQ 5 /*maximum number of waiting client connections */

#define MSG_LINE 256 // Length of the printable messaged
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
            printf("%s, %s, %f", reply.result, request.operand_left, val2);
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

int main (int argc, char **argv)
{
    int listenfd, connfd, n;
    socklen_t clilen;
    struct request_t request;
    struct reply_t reply;
    struct sockaddr_in cliaddr, servaddr;
    char * printable_str;

    setup_listener(&listenfd, &servaddr);

    while (1) 
    {
        accept_connections(&connfd, listenfd, &cliaddr, &clilen);

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
    //close listening socket
    close (listenfd);
}
