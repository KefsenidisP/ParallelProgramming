// #include <bits/types/cookie_io_functions_t.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define SERV_PORT 1337 /*port*/
#define LISTENQ 5 /*maximum number of waiting client connections */

#define MAX_CLIENTS 32
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
    int id;
    int fd;
};

// Struct with information of clients connexcted
struct client_info_t {
    int id; // Its id
    int fd; // Client's file descriptor
    struct sockaddr_in cliaddr; // Socket of the client
};

// Global for storing client info
struct client_info_t * connected_clinfo;

// A mutex
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

// The close connection prootocol
void clear_connection(int fd, int id)
{
    struct send_t send_msg;

    // The message will just contain a code EXIT to send to the client in order to terminate
    send_msg.code = CODE_EXIT;
    send(fd, (void *) &send_msg, sizeof(struct send_t), 0);

    // Now delete the record from the array
    pthread_mutex_lock(&mutex);
    connected_clinfo[id - 1].id = 0;
    pthread_mutex_unlock(&mutex);

    puts("Closing coonnection...");

    // And close connection 
    close(fd);
}

void * serve(void * thread_args)
{
    struct thread_args_t * targs = (struct thread_args_t *) thread_args;
    int fd = targs->fd;
    int id = targs->id;

    while (1)
    {
        struct recv_t recv_msg;
        struct send_t send_msg;
    
        // The server accepts message from one user
        recv(fd, (void *) &recv_msg, sizeof(struct recv_t), 0);
        
        // Check for protocol
        if (recv_msg.code == CODE_EXIT)
            break;
        else if (recv_msg.code != CODE_OK)
            continue;

        send_msg.code = recv_msg.code;
        strcpy(send_msg.msg, recv_msg.msg);

        // And sends to all the other users
        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            int recver_id = connected_clinfo[i].id;
            int recver_fd = connected_clinfo[i].fd;

            // If the recver id is the same as the sender's 
            // or reecord emppty continuue
            if (recver_id == id || recver_id == 0)
                continue;

            printf("Sending message too %d...\n", recver_id);

            send(recver_fd, (void *) &send_msg, sizeof(struct recv_t), 0);
        }
    }

    clear_connection(fd, id);
}

// Search for the client in one of the lists and return its id = i + 1
// Also it adds a new record to the global connection array
int get_id_and_add_connection(int fd, struct sockaddr_in cliaddr)
{
    int i = 0;

    // Just find the first non empty position of the array
    for (i = 0; i < MAX_CLIENTS; ++i)
        if (connected_clinfo[i].id == 0) // See if id is 0, which means no record at this indeex
            break;

    pthread_mutex_lock(&mutex);
    connected_clinfo[i].id = i + 1;
    connected_clinfo[i].fd = fd;
    connected_clinfo[i].cliaddr = cliaddr;
    pthread_mutex_unlock(&mutex);

    return i + 1;
}

int main(int argc, char * argv[])
{
    int listenfd, n;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    setup_listener(&listenfd, &servaddr);

    connected_clinfo = (struct client_info_t *) malloc(MAX_CLIENTS * sizeof(struct client_info_t));

    while (1)
    {
        int connfd;
        pthread_t tid;
        struct thread_args_t * targs;

        targs = (struct thread_args_t *) calloc(1, sizeof(struct thread_args_t));

        accept_connections(&connfd, listenfd, &cliaddr, &clilen);

        targs->id = get_id_and_add_connection(connfd, cliaddr);
        targs->fd = connfd;

        pthread_create(&tid, NULL, serve, (void *) targs);
    }

    close(listenfd);
}
