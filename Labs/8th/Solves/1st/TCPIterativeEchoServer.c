#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 1337 /*port*/
#define LISTENQ 5 /*maximum number of waiting client connections */
#define DELIMETER ":" // The character seperating command and string
#define OFFSET 13 // the rot offset

// Below are the protocal orders
#define CMD_EXIT "EXIT:"
#define CMD_UCASE "UCASE:"
#define CMD_LCASE "LCASE:"
#define CMD_ROTENC "ROTENC:"
#define CMD_ROTDEC "ROTDEC:"
#define ERROR "Error, wrong command!"

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
char * process_request(char * buf, char * str)
{
    char * reply;

    if (strncmp(buf, CMD_EXIT, strlen(CMD_EXIT)) == 0)
        strcpy(str, "EXIT");
    else if (strncmp(buf, CMD_UCASE, strlen(CMD_UCASE)) == 0)
    {
        strcpy(str, "Sending uppercase of the input string: ");
            
        // Get the required string from the buffer
        reply = strtok(buf, DELIMETER);
        reply = strtok(NULL, DELIMETER);

        for (int i = 0; i < strlen(reply); ++i)
            reply[i] = toupper(reply[i]);
    }
    else if (strncmp(buf, CMD_LCASE, strlen(CMD_LCASE)) == 0)
    {
        strcpy(str, "Sending lowercase of the input string: ");
        
        // Get the required string from the buffer
        reply = strtok(buf, DELIMETER);
        reply = strtok(NULL, DELIMETER);

        for (int i = 0; i < strlen(reply); ++i)
            reply[i] = tolower(reply[i]);
    }
    else if (strncmp(buf, CMD_ROTENC, strlen(CMD_ROTENC)) == 0)
    {
        strcpy(str, "Sending the input string in its rot-13 encrypted form: ");
    
        // Get the required string from the buffer
        reply = strtok(buf, DELIMETER);
        reply = strtok(NULL, DELIMETER);

        for (int i = 0; i < strlen(reply); ++i)
        {
            if (reply[i] <= 'z' && reply[i] >= 'a')
                reply[i] = (reply[i] + OFFSET - 'a') % 26 + 'a';
            else 
                reply[i] = (reply[i] + OFFSET - 'A') % 26 + 'A';
        }
    }
    else if (strncmp(buf, CMD_ROTDEC, strlen(CMD_ROTDEC)) == 0)
    {
        strcpy(str, "Sending the input string in its rot-13 decrypted form: ");

        // Get the required string from the buffer
        reply = strtok(buf, DELIMETER);
        reply = strtok(NULL, DELIMETER);

        for (int i = 0; i < strlen(reply); ++i)
        {
            if (reply[i] <= 'z' && reply[i] >= 'a')
                reply[i] = (reply[i] - OFFSET - 'a') % 26 + 'a';
            else
                reply[i] = (reply[i] - OFFSET - 'A') % 26 + 'A';
        }
    }
    else 
        strcpy(str, ERROR);

    return reply;
}


int main (int argc, char **argv)
{
    int listenfd, connfd, n;
    socklen_t clilen;
    char * buf;
    struct sockaddr_in cliaddr, servaddr;
    char * printable_str;
    char * reply_str;

    setup_listener(&listenfd, &servaddr);

    while (1) 
    {
        accept_connections(&connfd, listenfd, &cliaddr, &clilen);

        while (1)
        {
            // These exist, only to safeguard that the buffer is emptied
            printable_str = (char *) calloc(MAXLINE, sizeof(char));
            buf = (char *) calloc(MAXLINE, sizeof(char));

            //does not work for empty string
            // Receive request
            n = recv(connfd, buf, MAXLINE, 0);

            buf[n - 1] = '\0'; /*remove \n and put \0 instead */
            reply_str = process_request(buf, printable_str);

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
            puts(reply_str); // Print the string to send, only

            //process request here
            send(connfd, reply_str, strlen(reply_str), 0);

            free(printable_str);
            free(buf);
        }   

        close(connfd);
        printf ("Connection closed ...\n");
    }
    //close listening socket
    close (listenfd);
}
