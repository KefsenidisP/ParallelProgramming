#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#define SERV_PORT 1337 /*port*/
#define LISTENQ 5 /*maximum number of waiting client connections */
#define THREAD_NUM 4
#define MAX_ITERS 1000000 // The max number of iterations, if the algorithm was sequential

struct send_t {
    int id; // Thread rank
    int iters;
    int max_iters; // It actually gets the value of the macro
};

struct recv_t {
    int id; // Thread rank
    double sum; // The calculated worker sum
};

struct thread_args_t {
    int rank; // Worker rank
    int listenfd; // The file descriptor opened with each worker
    double * sum; // This is shared
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

// Function to send data towrads the workers
void send_protocol(int fd, int rank)
{
    struct send_t send_msg;

    send_msg.id = rank;
    send_msg.iters = MAX_ITERS / THREAD_NUM;
    send_msg.max_iters = MAX_ITERS;
    send(fd, (void *) &send_msg, sizeof(struct send_t), 0);
}

// Function to receive data from the workers
void recv_protocol(int fd, double * sum)
{
    struct recv_t recv_msg;
    
    recv(fd, (void *) &recv_msg, sizeof(struct recv_t), 0);
    
    // Protect this as it is shared
    // Reduce sum
    pthread_mutex_lock(&mutex);
    *sum += recv_msg.sum;
    pthread_mutex_unlock(&mutex);
}

// The main master function
void * master_thread(void * thread_args)
{
    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    struct thread_args_t * targs = (struct thread_args_t *) thread_args;
    int rank = targs->rank;
    int connfd;
    int listenfd = targs->listenfd;

    // Accept connections from workers
    accept_connections(&connfd, listenfd, &cli_addr, &cli_len);
    // Send info to worker newly assigned
    send_protocol(connfd, rank);
    // Wait to receive the results from the thread
    recv_protocol(connfd, targs->sum);
    // And close the connection
    close(connfd); 
}

int main(int argc, char * argv[])
{
    int listenfd;
    int connfd[THREAD_NUM];
    double sum, pi, step;
    struct sockaddr_in server_addr;
    pthread_t tid[THREAD_NUM];
    struct thread_args_t targs[THREAD_NUM];
    double PI25DT = 3.141592653589793238462643;

    setup_listener(&listenfd, &server_addr);

    sum = 0.0;
    step = 1.0 / MAX_ITERS;

    for (int i = 0; i < THREAD_NUM; ++i)
    {
        targs[i].rank = i;
        targs[i].listenfd = listenfd;
        targs[i].sum = &sum;

        pthread_create(&tid[i], NULL, master_thread, &targs[i]);
    }

    // And now wait for the workers to finish and get back the results
    for (int i = 0; i < THREAD_NUM; ++i)
        // Wait for threads to return
        pthread_join(tid[i], NULL);

    // Calculate and print pi
    pi = sum * step;
    printf("Pi is approximately %.16f, Error is %.16f\n", pi, fabs(pi - PI25DT));

    // Cloose listener
    close(listenfd);

    return EXIT_SUCCESS;
}
