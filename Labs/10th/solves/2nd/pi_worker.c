#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define THREAD_NUM 4

// The structs are the opposite than the master
struct recv_t {
    int id; // Thread id
    int iters;
    int max_iters; // It actually gets the value of the macro
};

struct send_t {
    int id; // Thread id
    double sum; // The calculated worker sum
};

struct thread_args_t {
    char ** argv; // It is actually the whole argument vector
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

double f( double a )
{
    return (4.0 / (1.0 + a * a));
}

// Worker receive message protocol. Returns the calculated sum
double recv_protocol(int fd)
{
    struct recv_t recv_msg;
    double step;
    double x;
    double sum;
    int start;
    int end;

    // Receive data from master 
    recv(fd, (void *) &recv_msg, sizeof(struct recv_t), 0);

    step = 1.0 / recv_msg.max_iters;
    x = 0.0;
    sum = 0.0;
    
    start = recv_msg.id * recv_msg.iters + 1;
    end = start + recv_msg.iters - 1;
    printf("Start: %d, End: %d\n", start, end);

    // And use them to make calculation
    for (int i = start; i <= end; ++i)
    {
        x = step * ((double)i - 0.5);
        sum += f(x);
    }

    return sum;
}

void send_protocol(int fd, double sum)
{
    struct send_t send_msg;

    // Iniitialize message
    send_msg.sum = sum;
    // And send it to the master
    send(fd, (void *) &send_msg, sizeof(struct send_t), 0);
}

// The main worker function
void * worker_thread(void * thread_args)
{
    int connfd;
    struct thread_args_t * targs = (struct thread_args_t *) thread_args;
    struct sockaddr_in addr;
    char ** argv;
    double sum;

    // First connect to a master thread
    connect_to_server(&connfd, &addr, targs->argv);
    // Receive message frrom server
    sum = recv_protocol(connfd);
    // And now send the sum back to the master thread
    send_protocol(connfd, sum);
}

int main(int argc, char * argv[])
{
    int fd;
    struct thread_args_t targs[THREAD_NUM];
    pthread_t tid[THREAD_NUM];

    if (argc != 3)
    {
        printf("Usage: %s <ip_to_connect> <port_to_connect>\n", argv[0]);
        return EXIT_SUCCESS;
    }
    
    // Create worrker threads
    for (int i = 0; i < THREAD_NUM; ++i)
    {
        targs[i].argv = argv;

        pthread_create(&tid[i], NULL, worker_thread, &targs[i]);
    }

    // And wait for threads too finish
    for (int i = 0; i < THREAD_NUM; ++i)
        pthread_join(tid[i], NULL);

    return EXIT_SUCCESS;
}
