
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 4096 /*max text line length*/

// Below are the protocal orders
#define CMD_EXIT "EXIT:"
#define CMD_UCASE "UCASE:"
#define CMD_LCASE "LCASE:"
#define CMD_ROTENC "ROTENC:"
#define CMD_ROTDEC "ROTDEC:"
#define ERROR "Error, wrong command!"

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

// Read from buffer.
void make_request(char * line, int fd)
{
    int buf_size;

    printf ("Enter a message to send\n");
    fgets(line, MAXLINE, stdin);

    buf_size = strlen(line);

    send(fd, line, buf_size, 0);

    line[buf_size - 1] = '\0'; /*remove \n and put \0 instead */
}

// According to the user's iinput, choose an action
// Just returns a cosmetic mesage, to accompany the seerver's answer
char * process_client_protocol(char * line)
{
    char * str;

    str = (char *) calloc(MAXLINE, sizeof(char));

    if (strncmp(line, CMD_EXIT, strlen(CMD_EXIT)) == 0)
        exit(0);
    else if (strncmp(line, CMD_UCASE, strlen(CMD_UCASE)) == 0)
        strcpy(str, "Receiving uppercase of the input string: ");
    else if (strncmp(line, CMD_LCASE, strlen(CMD_LCASE)) == 0)
        strcpy(str, "Receiving lowercase of the input string: ");
    else if (strncmp(line, CMD_ROTENC, strlen(CMD_ROTENC)) == 0)
        strcpy(str, "Receiving the input string in its rot-13 encrypted form: ");
    else if (strncmp(line, CMD_ROTDEC, strlen(CMD_ROTDEC)) == 0)
        strcpy(str, "Receiving the input string in its rot-13 decrypted form: ");
    else 
        strcpy(str, ERROR);

    return str;
}

// Process thee server's reply
void process_reply(char * line, int fd, char * str)
{
    recv(fd, line, MAXLINE, 0);
    printf("%s", str);
    fputs(line, stdout);
    printf("\n");
}

// Print the menu
void print_menu(void)
{
    puts("Insert a command <CMD:> and a string <str>");
    puts("1) \'EXIT:\' to exit");
    puts("2) \'UCASE:<str>\' to print the string <str> in upper case");
    puts("3) \'LCASE:<str>\' to print the string <str> in lower case");
    puts("4) \'ROTENC:<str>\' to print the string encrypted by the rot-13 algorithm");
    puts("5) \'ROTDEC:<str>\' to print the string decrypted by the rot-13 algorithm");
}

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    char *sendline, *recvline;
    char * printable_str;

    if (argc !=3) 
    {
        printf("Usage: TCPClient <IP address> <port number>\n");
        exit(1);
    }

    // Initialize socket
    connect_to_server(&sockfd, &servaddr, argv);
    print_menu();

    while (1)
    {
        sendline = (char *) calloc(MAXLINE, sizeof(char));
        recvline = (char *) calloc(MAXLINE, sizeof(char));

        make_request(sendline, sockfd);
        printable_str = process_client_protocol(sendline);

        // Wrong command. Do not receive from server
        if (strncmp(printable_str, ERROR, strlen(ERROR)) == 0)
        {
            printf("%s\n", printable_str);
            continue;
        }

        process_reply(recvline, sockfd, printable_str);
    
        free(printable_str);
        free(sendline);
        free(recvline);
    }

    return EXIT_SUCCESS;
}
