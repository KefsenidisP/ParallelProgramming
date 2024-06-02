// οι καταναλωτές δεν σταματούν: τερματισμός με CTRL-C

/*
 * Unlimited size buffer
 *
 * Removed the full array condition and added the size variable
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #define SIZE 10   // το μέγεθος του κυκλικού buffer
#define LOOP 10   // αριθμός επαναλήψεων παραγωγού
#define NPRO 2    // αριθμός παραγωγών
#define NCON 2    // αριθμός καταναλωτών
#define DPRO 100000  // καθυστέρηση παραγωγού
#define DCON 200000  // καθυστέρηση καταναλωτή

typedef struct {
	int * buffer;     // ο κυκλικός buffer
    int size;         // The buffer size
	int front, back;      // αριθμοδείκτες προσθήκης και αφαίρεσης 
	int counter;          // μετρητής πλήθους στοιχείων
	pthread_mutex_t *mut; // mutex για τον αμοιβαίο αποκλεισμό
	pthread_cond_t *condEmpty; // μεταβλητές συνθήκης για συγχρονισμό
} queue;

typedef struct {
	int id;
	queue *fifo;
} thread_data;

// αρχικοποίηση ουράς
queue *queueInit(int size) {
	queue *q;

	q = (queue *) malloc(sizeof(queue));
	if (q == NULL)
		return (NULL);

	// αρχικά η ουρά είναι άδεια, αρχικοποιούμε τις μεταβλητές 
    q->buffer = (int *) malloc(size * sizeof(int));
    q->size = size;
	q->counter = 0;
	q->front = 0;
	q->back = size - 1;
	q->mut = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(q->mut, NULL);
	q->condEmpty = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
	pthread_cond_init(q->condEmpty, NULL);

	return (q);
}

// διαγραφή της ουράς
void queueDelete(queue *q) {
	pthread_mutex_destroy(q->mut);
	free(q->mut);
	pthread_cond_destroy(q->condEmpty);
	free(q->condEmpty);
	free(q);
}

// προσθήκη στοιχείου στην ουρά
void queueAdd(queue *q, int in) {
	// παίρνουμε το κλειδί
	pthread_mutex_lock(q->mut);

	// The size of the buffer is "infinite" so there is no need to check
    // if it is full or not

	// αλιώς προσθέτουμε κανονικά το στοιχείο 
	q->back = (q->back + 1) % q->size;
	q->buffer[q->back] = in;
	q->counter++;

    // The buffer though, may become empty at some point and it will
    // be at beginning of the program, so this needs to remain,
    // so as to wake any delete thread that enters when the buffer is empty
	if (q->counter == 1) pthread_cond_signal(q->condEmpty);
	pthread_mutex_unlock(q->mut);

	return;
}

// αφαίρεση στοιχείου από την ουρά
void queueDel(queue *q, int *out) {

	// παίρνουμε το κλειδί
	pthread_mutex_lock(q->mut);

	// ελέγχουμε ότι ο buffer είναι άδειος
	while (q->counter == 0) {
		// αν είναι άδειος περιμένουμε στην συνθήκη
		printf("Buffer empty \n");
		pthread_cond_wait(q->condEmpty, q->mut);
	}

	// αλιώς αφαιρούμε κανονικά το στοιχείο 
	*out = q->buffer[q->front];
	q->front = (q->front + 1) % q->size;
	q->counter--;
    
    // The full condtion is removed

	pthread_mutex_unlock(q->mut);

	return;
}

// το νήμα του παραγωγού (βάζουμε καθυστέρηση για δοκιμή)
void *producer(void *threadarg) {
	int i, myid;
	queue *fifo;
	thread_data *my_data;
	my_data = (thread_data *) threadarg;
	myid = my_data->id;
	fifo = (queue *) my_data->fifo;

	for (i = 0; i < LOOP; i++) {
		printf("Producer %d item %d \n", myid, i);
		queueAdd(fifo, i);
		usleep(DPRO);
	}
	return (NULL);
}

// το νήμα του καταναλωτή (βάζουμε καθυστέρηση για δοκιμή)
void *consumer(void *threadarg) {
	int i, myid;
	queue *fifo;
	thread_data *my_data;
	my_data = (thread_data *) threadarg;
	myid = my_data->id;
	fifo = (queue *) my_data->fifo;

	for (;;) {
		queueDel(fifo, &i);
		printf("Consumer %d item %d \n", myid, i);
		usleep(DCON);
	}
	return (NULL);
}

// η main για να ξεκινήσουμε τα νήματα
int main() {
	queue *fifo;
	pthread_t pro[NPRO], con[NCON];
	thread_data pro_args[NPRO];
	thread_data con_args[NCON];
	int i;
    int size;

    size = NPRO * LOOP * NPRO;

	fifo = queueInit(size);
	if (fifo == NULL) {
		exit(1);
	}

	for (i = 0; i < NPRO; i++) {
		pro_args[i].id = i;
		pro_args[i].fifo = fifo;
		pthread_create(&pro[i], NULL, producer, &pro_args[i]);
	}
	for (i = 0; i < NCON; i++) {
		con_args[i].id = i;
		con_args[i].fifo = fifo;
		pthread_create(&con[i], NULL, consumer, &con_args[i]);
	}
	for (i = 0; i < NPRO; i++)
		pthread_join(pro[i], NULL);
	for (i = 0; i < NCON; i++)
		pthread_join(con[i], NULL);

	queueDelete(fifo);

	return 0;
}
