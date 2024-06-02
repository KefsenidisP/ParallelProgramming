// οι καταναλωτές δεν σταματούν: τερματισμός με CTRL-C

/*
 * code for SPSC
 *
 * We have only one thread for each function, so we can remove the inner lock,
 * which locks the buffer. The other two locks are needed, so as not to exceed 
 * the limit of the buffer and also not to remove an element outside of the bound of
 * the buffer.
*/

#include <pthread.h>
#include <semaphore.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE 10   // το μέγεθος του κυκλικού buffer
#define LOOP 10   // αριθμός επαναλήψεων παραγωγού
#define NPRO 1    // αριθμός παραγωγών
#define NCON 1    // αριθμός καταναλωτών
#define DPRO 100000  // καθυστέρηση παραγωγού
#define DCON 200000  // καθυστέρηση καταναλωτή

typedef struct {
  int buffer[SIZE];     // ο κυκλικός buffer
  int front, back;      // αριθμοδείκτες προσθήκης και αφαίρεσης 
  sem_t *toPut, *toGet; // counting semaphores για σηματοδοσία
} queue;

typedef struct 
{
   int	id;
   queue *fifo;
}thread_data;

// αρχικοποίηση ουράς
queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  // αρχικά η ουρά είναι άδεια, αρχικοποιούμε τις μεταβλητές 
  q->front = 0;
  q->back = SIZE - 1;
  q->toPut = (sem_t *) malloc (sizeof (sem_t));
  sem_init(q->toPut, 0, SIZE);
  q->toGet = (sem_t *) malloc (sizeof (sem_t));
  sem_init(q->toGet, 0, 0);
	
  return (q);
}

// διαγραφή της ουράς
void queueDelete (queue *q)
{
  sem_destroy (q->toPut);
  free (q->toPut);
  sem_destroy (q->toGet);
  free (q->toPut);
  free (q);
}

// προσθήκη στοιχείου στην ουρά
void queueAdd (queue *q, int in)
{
  // ελέγχουμε αν ο buffer είναι γεμάτος P(q->toPut)
  sem_wait (q->toPut);

  /*
   * The buffer semaphore is not needed, beecause we have only one thread
   * per function. The other two semaphores and the way they are iinitialized
   * (toGet = 0, toPut = size) ensure that the functions will be locked only 
   * wheen the buffer iis empty or full.
  */

  // προσθέτουμε το στοιχείο 
  q->back = (q->back + 1) % SIZE;
  q->buffer[q->back] = in;
  
  // σηματοδοτούμε οτι ο buffer διαθέτει στοιχεία V(q->toGet) 
  sem_post (q->toGet);
  
 
  return;
}

// αφαίρεση στοιχείου από την ουρά
void queueDel (queue *q, int *out)
{

  // ελέγχουμε αν ο buffer είναι άδειος P(q->toGet)
  sem_wait (q->toGet);

  // αφαιρούμε το στοιχείο 
  *out = q->buffer[q->front];
  q->front = (q->front + 1) % SIZE;
  
  // σηματοδοτούμε οτι ο buffer διαθέτει κενές θέσεις V(q->toPut) 
  sem_post (q->toPut);
  
  return;
}

// το νήμα του παραγωγού (βάζουμε καθυστέρηση για δοκιμή)
void *producer (void *threadarg)
{
  int i, myid;
  queue *fifo;
  thread_data *my_data;
  my_data = (thread_data *) threadarg;
  myid = my_data->id;
  fifo = (queue *)my_data->fifo;

  for (i = 0; i < LOOP; i++) {
    printf("Producer %d item %d \n", myid,i);
    queueAdd (fifo, i);
    usleep (DPRO);
  }
  return (NULL);
}

// το νήμα του καταναλωτή (βάζουμε καθυστέρηση για δοκιμή)
void *consumer (void *threadarg)
{
  int i, myid;
  queue *fifo;
  thread_data *my_data;
  my_data = (thread_data *) threadarg;
  myid = my_data->id;
  fifo = (queue *)my_data->fifo;

  for (;;) {
    queueDel (fifo, &i);
    printf("Consumer %d item %d \n", myid, i);
    usleep (DCON);
  }
  return (NULL);
}

// η main για να ξεκινήσουμε τα νήματα
int main ()
{
  queue *fifo;
  pthread_t pro[NPRO], con[NCON];
  thread_data pro_args[NPRO];
  thread_data con_args[NCON];
  int i;

  fifo = queueInit ();
  if (fifo ==  NULL) {
    exit (1);
  }
  
  for (i=0; i<NPRO; i++) {
       pro_args[i].id = i;
       pro_args[i].fifo = fifo;
       pthread_create (&pro[i], NULL, producer, &pro_args[i]);
  }     
  for (i=0; i<NCON; i++) {
       con_args[i].id = i;
       con_args[i].fifo = fifo;    
       pthread_create (&con[i], NULL, consumer, &con_args[i]);
  }     
  for (i=0; i<NPRO; i++)     
       pthread_join (pro[i], NULL);
  for (i=0; i<NCON; i++)        
  		pthread_join (con[i], NULL);
  		
  queueDelete (fifo);

  return 0;
}
