#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>


#define SERV_PORT 1336 /*port*/
#define LISTENQ 5 /*maximum number of waiting client connections */

#define BUFFER_SIZE 64 // Size of the request_t reply_t char arrays
#define MSG_PROMPT_SIZE 128 // The maximum size of prompt messages

// Protocol statements
#define EXIT -1
#define CALC_PI 1

// Error statements
#define ERROR 100
#define OK 0

// This is unique to the server
// It changes to OK
#define FOUND_IN_ARRAY 10

struct request_t {
    int code; // -1 for exit, 1 for calculation of pi
    char iters[BUFFER_SIZE]; // Number of iterations
};

struct reply_t {
    int code; // This can be ERROR or OK
    double result; // Pi value 
};

struct thread_args {
    int fd; // A file descriptor
};

struct calced_pis {
    int iters;
    double pi;
    struct calced_pis * next;
};

// List of the calculated pis. It is a global array
struct calced_pis * pi_list;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

double f( double a )
{
    return (4.0 / (1.0 + a * a));
}

void add_pi_elm(struct calced_pis * pi_list_elm)
{
    struct calced_pis * tmp;

    // Insert new element, only if a copy of it does not exist
    for (tmp = pi_list; ; tmp = tmp->next)
    {
        if (tmp->iters == pi_list_elm->iters)
            return;

        if (tmp->next == NULL)
            break;
    }

    tmp->next = pi_list_elm;

}

// Returns the value of pi
double calc_pi(int iters)
{
    double x = 0.0;
    double step = 1.0 / (double) iters;
    double sum = 0.0;
    double pi = 0.0;
    struct calced_pis * pi_list_elm;

    pi_list_elm = (struct calced_pis *) malloc(sizeof(struct calced_pis));

    for (int i = 1; i <= iters; ++i)
    {
    	x = step * ((double)i - 0.5);
        sum += f(x);
    }

    pi = step * sum;

    pi_list_elm->iters = iters;
    pi_list_elm->pi = pi;
    pi_list_elm->next = NULL;

    pthread_mutex_lock(&mutex);
    add_pi_elm(pi_list_elm);
    pthread_mutex_unlock(&mutex);

    return pi;
}

void search_in_pi_list(int iters, struct reply_t * reply)
{
    struct calced_pis * pi_list_elm;

    // Iterate through the pi_list list
    for (pi_list_elm = pi_list; ; pi_list_elm = pi_list_elm->next)
    {
        if (pi_list_elm->iters == iters)
        {
            reply->code = FOUND_IN_ARRAY;
            reply->result = pi_list_elm->pi;

            return;
        }

        if (pi_list_elm->next == NULL)
            break;
    }

    // The pi was never calculated
    reply->code = OK;
}

// Returns a debugging str string anf the answer to the server reply
void process_request(struct request_t request, struct reply_t * reply, char * str)
{
    int iters_int;

    iters_int = strtol(request.iters, NULL, 10);

    if (iters_int > 0)
    {
        search_in_pi_list(iters_int, reply);
        
        // If not found then calculate it
        if (reply->code == OK)
        {
            strcpy(str, "Calculating pi...");
            reply->result = calc_pi(iters_int);
            
            return;
        }

        strcpy(str, "Found in the array!");
    }
    else 
    {
        strcpy(str, "Invalid integer value!");
        reply->code = ERROR;
    }
}

void * serve(void * targs)
{
    struct thread_args * thread_args = (struct thread_args *) targs;
    int connfd = thread_args->fd;
    int n;
    char * printable_str;

    pi_list = (struct calced_pis *) malloc(sizeof(struct calced_pis));

    while (1)
    {
        struct request_t request;
        struct reply_t reply;

        // These exist, only to safeguard that the buffer is emptied
        printable_str = (char *) calloc(MSG_PROMPT_SIZE, sizeof(char));

        //does not work for empty string
        // Receive request
        n = recv(connfd, &request, sizeof(struct request_t), 0);

        if (request.code == EXIT)
            break;
        else if (request.code == CALC_PI)
        {

            process_request(request, &reply, printable_str);

            // Check if command is wrong
            if (reply.code == ERROR)
            {
                printf("%s\n", printable_str);
                continue;
            }
            if (reply.code == FOUND_IN_ARRAY)
                reply.code = OK;

            //debugging
            printf("%s\n", printable_str);
            printf("%.16f\n", reply.result); // Print the string to send, only

            //process request here
            send(connfd, &reply, sizeof(struct reply_t), 0);

            free(printable_str);
        }
    }

    close(connfd);
    puts("Connection closed...");
}

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

int main(int argc, char ** argv)
{
    int listenfd, n;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    setup_listener(&listenfd, &servaddr);

    // Create a thread foor each coonnection
    while (1)
    {
        int connfd; // Made this local for each thread
        pthread_t tid;
        struct thread_args th_args;

        // Get the local thrread aguments
        accept_connections(&connfd, listenfd, &cliaddr, &clilen);

        th_args.fd = connfd;

        pthread_create(&tid, NULL, serve, &th_args);
    }

    close(listenfd);

    return EXIT_SUCCESS;
}



