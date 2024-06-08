#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define THREAD_NUM 2
#define MSG_SEND_SIZE 1024 // NUmber of characters to send (max)
#define MSG_RECV_SIZE 1024 // Number of characters to receive (max)

#define STR_EXIT ":q!"

#define CODE_OK 0
#define CODE_EXIT 1

struct send_t {
    int code; // EXIT, or OK
    char msg[MSG_SEND_SIZE];
};

struct recv_t {
    int code; // EXIT or OK
    char msg[MSG_RECV_SIZE];
};

struct thread_args_t {
    int fd;
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

void prompt_msg(void)
{
    puts("Welcome to the vim like chat!");
    puts("Maybe press \':q!\' to exit!");
    puts("Feel free to send any message to the others");
}

// Function to ssend a message until the user decides to exit
void * send_msg_thread(void * thread_args)
{
    char * line = NULL;
    int exit = 0; // A flag to terminate the thread, when user exits
    int fd; // The socket file descriptor
    struct send_t * msg;
    struct thread_args_t * targs = (struct thread_args_t *) thread_args;

    fd = targs->fd;

    while (!exit)
    {
        msg = (struct send_t *) calloc(1, sizeof(struct send_t));

        while (1)
        {
            printf(": ");

            if (line == NULL)
                line = (char *) calloc(MSG_SEND_SIZE, sizeof(char));

            fgets(line, MSG_SEND_SIZE, stdin);

            printf("\n");


            // Exit thread
            if (!strncmp(line, STR_EXIT, strlen(STR_EXIT)))
            {
                strcpy(msg->msg, line);
                msg->code = CODE_EXIT;
                exit = 1;
                break;
            }
            // This will mean that the string is not empty.
            else if (line[0] != '\n')
            {
                strcpy(msg->msg, line);
                msg->code = CODE_OK;
                break;
            }
        }
        // Send the message to the server, in order for it to be distributed among the other clients
        send(fd, (void *) msg, sizeof(struct send_t), 0);

        free(msg);
        msg = NULL;

        free(line);
        line = NULL;
    }
}

// Receive messages from server (the other users)
void * recv_msg_thread(void * thread_args)
{
    int exit = 0; // Flag. Terminate on receiving exit code
    int fd; // Socket file descriptor
    struct recv_t * msg;
    struct thread_args_t * targs = (struct thread_args_t *) thread_args;

    fd = targs->fd;

    while (!exit)
    {
        msg = (struct recv_t *) calloc(1, sizeof(struct recv_t));

        // This thread will just wait until it receives a message
        recv(fd, (void *) msg, sizeof(struct recv_t), 0);


        if (msg->code == CODE_OK)
        {
            puts(msg->msg);
            printf(": ");
        }
        if (msg->code == CODE_EXIT)
        {
            exit = 1;
            puts("Closing connection...");
        }

        free(msg);
        msg = NULL;
    }
}

int main(int argc, char * argv[])
{
    int sendfd, recvfd, sockfd;
    struct sockaddr_in servaddr;
    struct thread_args_t targs[THREAD_NUM];
    pthread_t tid[THREAD_NUM];

    if (argc != 3)
    {
        printf("Usage: %s <ip_to_connect> <port_to_connect>\n", argv[0]);
        return EXIT_SUCCESS;
    }

    // Connect tp the server. Open different file ddscriptoors for the two threads
    prompt_msg();
    connect_to_server(&sockfd, &servaddr, argv);
    // connect_to_server(&recvfd, &servaddr, argv);

    // Create the sender and receiver thread
    targs[0].fd = sockfd;
    pthread_create(&tid[0], NULL, send_msg_thread, &targs[0]);
    pthread_create(&tid[1], NULL, recv_msg_thread, &targs[0]);
    
    // Wait for threads to exit
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);

    return EXIT_SUCCESS;
}
