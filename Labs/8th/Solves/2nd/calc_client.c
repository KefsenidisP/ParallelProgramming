
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MSG_LINE 256 // Length of the printable messages
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

// Returns 0 if the input is ok. Also returns the request message
int check_input(char * line, struct request_t * request)
{
    int operand_cnt = 0;
    int operator_found = 0;
    char * tmp;

    // The operator must be at the beginning of the line
    request->operation = line[0];
    if (request->operation == '!')
        return 0;
    else if (request->operation != '*' && request->operation != '-' &&
            request->operation != '/' && request->operation != '+')
    {
        printf("Wrong operator %c!\n", request->operation);
        return 1;
    }

    // Cut the inputed string and parse to a request type
    tmp = strtok(line, DELIMETER); // The first one is the operator, which has already been parsed
    while (1)
    {
        tmp = strtok(NULL, DELIMETER);

        if (tmp == NULL)
            break;

        operand_cnt++;

        // 1 is the left 2 is the right operand
        if (operand_cnt == 1)
            strcpy(request->operand_left, tmp);
        else if (operand_cnt == 2)
            strcpy(request->operand_right, tmp);
    }

    if (operand_cnt != 2)
    {
        printf("Two operands needed only %d provided!\n", operand_cnt);
        return 2;
    }

    return 0;
}

// Read from buffer. It also rreturns the operration that it neeeds to b performed
char make_request(int fd)
{
    int buf_size;
    char operation;
    char line[sizeof(struct request_t)];
    struct request_t request;

    while (1)
    {
        printf ("Enter a message to send\n");
        fgets(line, sizeof(struct request_t), stdin);

        if (!check_input(line, &request))
            break;
    }

    send(fd, &request, sizeof(struct request_t), 0);
    
    return request.operation;
}

// According to the user's iinput, choose an action
// Just returns a cosmetic mesage, to accompany the seerver's answer
char * process_client_protocol(char operation)
{
    char * str;

    str = (char *) calloc(MSG_LINE, sizeof(char));

    if (operation == CMD_EXIT)
        exit(0); // Closes all file descriptors
    else if (operation == CMD_ADD)
        strcpy(str, "Performing addition: ");
    else if (operation == CMD_REDUCE)
        strcpy(str, "Performing substitution: ");
    else if (operation == CMD_MUL)
        strcpy(str, "Performing multiplication: ");
    else if (operation == CMD_DIV)
        strcpy(str, "Performing division: ");
    else 
        strcpy(str, ERROR); // I do not think that it will ever reach here, because of the checking function

    return str;
}

// Process thee server's reply
void process_reply(int fd, char * str)
{
    struct reply_t reply;

    recv(fd, &reply, sizeof(struct reply_t), 0);
    printf("%s", str);
    fputs(reply.result, stdout);
    printf("\n");
}

// Print the menu
void print_menu(void)
{
    puts("Insert a message in the form of <operation> <operand> <operand>");
    puts("1) \'!\' to exit");
    puts("2) \'+ <operand> <operand>\' for addition");
    puts("3) \'- <operand> <operand>\' for substitution");
    puts("4) \'/ <operand> <operand>\' for division");
    puts("5) \'* <operand> <operand>\' for multiplication");
}

int main(int argc, char *argv[])
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
    print_menu();

    while (1)
    {
        operator = make_request(sockfd);
        printable_str = process_client_protocol(operator);

        // Wrong command. Do not receive from server
        if (strncmp(printable_str, ERROR, strlen(ERROR)) == 0)
        {
            printf("%s\n", printable_str);
            continue;
        }

        process_reply(sockfd, printable_str);
    
        free(printable_str);
    }

    return EXIT_SUCCESS;
}
