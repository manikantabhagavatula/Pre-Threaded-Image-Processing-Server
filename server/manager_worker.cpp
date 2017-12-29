extern "C"{
#include "csapp.h"
}
#include "csapp.cpp"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define QUEUESIZE 1             //Change depending on the number of threads
#define LOOP 5
#define mode S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
void *producer (void *args);
void *consumer (void *args);
void echo(int connfd);

typedef struct {
    int buf[QUEUESIZE];
    long head, tail;
    int full, empty;
    pthread_mutex_t *mut;
    pthread_cond_t *notFull, *notEmpty;
} queue;

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, int in);
void queueDel (queue *q, int *out);
void millisleep(int milliseconds);

int listenfd;

int main(int argc, char **argv)
{
    int connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;

    port = atoi(argv[1]);

    struct sched_param my_param;
    pthread_attr_t hp_attr; //high priority attr
    pthread_attr_t lp_attr; //low priority attr
    int i, min_priority, policy;

    my_param.sched_priority = sched_get_priority_min(SCHED_FIFO);
    min_priority = my_param.sched_priority;
    pthread_setschedparam(pthread_self(), SCHED_RR, &my_param);
    pthread_getschedparam (pthread_self(), &policy, &my_param);

    pthread_attr_init(&lp_attr);
    pthread_attr_init(&hp_attr);

    pthread_attr_setinheritsched(&lp_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setinheritsched(&hp_attr, PTHREAD_EXPLICIT_SCHED);
    
    pthread_attr_setschedpolicy(&lp_attr, SCHED_FIFO);
    pthread_attr_setschedpolicy(&hp_attr, SCHED_FIFO);

    my_param.sched_priority = min_priority + 1;
    pthread_attr_setschedparam(&lp_attr, &my_param);
    my_param.sched_priority = min_priority + 2;
    pthread_attr_setschedparam(&hp_attr, &my_param);

    queue *fifo;
    pthread_t pro, con1,con2,con3;

    fifo = queueInit ();
    if (fifo ==  NULL) {
        fprintf (stderr, "main: Queue Init failed.\n");
        exit (1);
    }

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    listenfd = Open_listenfd(port);
    pthread_create (&pro, &hp_attr, producer, fifo);
    pthread_create (&con1, &lp_attr, consumer, fifo);
    pthread_create (&con2, &lp_attr, consumer, fifo);
    pthread_create (&con3, &lp_attr, consumer, fifo);
    
    
    while(1);

}


void *producer (void *q)
{
    queue *fifo;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    fifo = (queue *)q;
    int *connfd;
    socklen_t clientlen;
    ///
    pthread_t thread_id = pthread_self();
    struct sched_param param;
    int priority, policy, ret;

    ret = pthread_getschedparam (thread_id, &policy, &param);
    priority = param.sched_priority;    
    printf("Priority of the manager is %d \n", priority);
    ///
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd=(int*)malloc(sizeof(int));
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        /* Determine the domain name and IP address of the client */
        hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
            sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        haddrp = inet_ntoa(clientaddr.sin_addr);
        printf("server connected to %s (%s)\n", hp->h_name, haddrp);

        pthread_mutex_lock (fifo->mut);
        while (fifo->full) {
            printf ("producer: queue FULL.\n");
            pthread_cond_wait (fifo->notFull, fifo->mut);
        }
        queueAdd (fifo, *connfd);
        pthread_mutex_unlock (fifo->mut);
        pthread_cond_signal (fifo->notEmpty);
        //millisleep (200);

        
    }
    return (NULL);
}

void *consumer (void *q)
{
    queue *fifo;
    int d;

    fifo = (queue *)q;

    pthread_t thread_id = pthread_self();
    struct sched_param param;
    int priority, policy, ret;


    ret = pthread_getschedparam (thread_id, &policy, &param);
    priority = param.sched_priority;    
    printf("The priority of worker thread is %d \n", priority);

    while(1) {
        pthread_mutex_lock (fifo->mut);
        while (fifo->empty) {
            printf ("Worker queue EMPTY.\n");
            pthread_cond_wait (fifo->notEmpty, fifo->mut);
        }
        queueDel (fifo, &d);
        pthread_mutex_unlock (fifo->mut);
        pthread_cond_signal (fifo->notFull);
        printf ("Worker recieved %d.\n", d-3);
        millisleep(300);
        echo(d);
        Close(d);
    }
    return (NULL);
}


queue *queueInit (void)
{
    queue *q;

    q = (queue *)malloc (sizeof (queue));
    if (q == NULL) return (NULL);

    q->empty = 1;
    q->full = 0;
    q->head = 0;
    q->tail = 0;
    q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
    pthread_mutex_init (q->mut, NULL);
    q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
    pthread_cond_init (q->notFull, NULL);
    q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
    pthread_cond_init (q->notEmpty, NULL);
    
    return (q);
}

void queueAdd (queue *q, int in)
{
    q->buf[q->tail] = in;
    q->tail++;
    if (q->tail == QUEUESIZE)
        q->tail = 0;
    if (q->tail == q->head)
        q->full = 1;
    q->empty = 0;

    return;
}

void queueDel (queue *q, int *out)
{
    *out = q->buf[q->head];

    q->head++;
    if (q->head == QUEUESIZE)
        q->head = 0;
    if (q->head == q->tail)
        q->empty = 1;
    q->full = 0;

    return;
}

void millisleep(int milliseconds)
{
      usleep(milliseconds * 1000);
}
