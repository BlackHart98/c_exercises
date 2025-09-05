#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define MAX_SUBS        50
#define MAX_TOPICS      50
#define BUFFER_SIZE     3




typedef struct _ring_buffer_t{
    int read_head, write_head;
    char * buffer[BUFFER_SIZE];
    pthread_mutex_t ring_buffer_mutex;
    pthread_cond_t is_empty;
    pthread_cond_t is_full;
} ring_buffer_t;


void ring_buffer_create(ring_buffer_t *);
int ring_buffer_empty(const ring_buffer_t *);
int ring_buffer_full(const ring_buffer_t *);
void enqueue(ring_buffer_t *, char *);
void dequeue(ring_buffer_t *, char **);
void ring_buffer_destroy(ring_buffer_t *);


typedef struct topic_t{
    uint8_t * sub_ids[MAX_SUBS];
} topic_t;


typedef struct broker_t {
    uint8_t subscribe_count;
    uint8_t topics[MAX_TOPICS];
    topic_t subscribers[MAX_TOPICS];
    ring_buffer_t ring_buffer[MAX_TOPICS];
} broker_t;

typedef struct subscriber_t {
    uint8_t topic_id[MAX_TOPICS];
    char * message;
} subscriber_t;


void broker_init(broker_t * my_broker);
void broker_deinit(broker_t * my_broker);
void subscribe(subscriber_t * my_subscriber, broker_t * my_broker, uint8_t topic);
void subscriber_init(subscriber_t *);
void unsubscribe(subscriber_t * my_subscriber, broker_t * my_broker, uint8_t topic);
void publish(broker_t * my_broker, uint8_t topic, char * message);
void get_subscribers(broker_t * my_broker, uint8_t topic_id, subscriber_t **);


void recieve_message(const subscriber_t *);


// This expected behaviour is to be stuck in busy waiting until any of the ring buffer is
// not empty then send the messages to the subscribers (I am working with the assumption 
// that the message string has a static lifetime duration, which is incorrect)
void * process_messages(void * arg){
    broker_t * broker_ptr = (broker_t *) arg;
    while(1){
        for (int i = 0; i < MAX_TOPICS; i++){
            if (ring_buffer_empty(&broker_ptr->ring_buffer[i])){
                // skip over empty ring buffers
                continue;
            } else {
                printf("Ring buffer for topic #%d contains message(s)\n", i);
                char * message;
                // if the ring buffer is empty it waits, this property is not too useful for this implementation
                dequeue(&broker_ptr->ring_buffer[i], &message); 
                for (int j = 0; j < broker_ptr->topics[i]; j++){
                    subscriber_t * sub = (subscriber_t *) broker_ptr->subscribers[i].sub_ids[j];
                    sub->topic_id[i] = 1;
                    sub->message = message;
                }
            }
        }
        fflush(stdout);
    }
    return NULL;
}



int main(int argc, char * argv[]){
    pthread_t some_thread;


    broker_t some_broker;
    broker_init(&some_broker);

    subscriber_t s[3];
    for (int i = 0; i < 3; i++){
        subscriber_init(&s[i]);
    }
    subscribe(&s[0], &some_broker, 0);
    subscribe(&s[1], &some_broker, 1);
    subscribe(&s[2], &some_broker, 1);

    // process messages just broadcasts the message to the subscribers
    pthread_create(&some_thread, NULL, process_messages, (void *)&some_broker);

    publish(&some_broker, 0, "Hello world");
    publish(&some_broker, 1, "Yet another hello world");


    sleep(2);
    recieve_message(&s[0]);
    recieve_message(&s[2]);

    pthread_join(some_thread, NULL);
    broker_deinit(&some_broker);
    return 0;
}



// Function declarations
void ring_buffer_create(ring_buffer_t * ring_buffer){
    static_assert((2 <= BUFFER_SIZE), "BUFFER_SIZE should be at least 2");
    assert((NULL != ring_buffer)&&"Ring buffer argument should not be not");
    ring_buffer->read_head = 0;
    ring_buffer->write_head = 0;
}

void enqueue(ring_buffer_t * ring_buffer, char * item){
    pthread_mutex_lock(&ring_buffer->ring_buffer_mutex);
    while (ring_buffer_full(ring_buffer)){
        printf("waiting for a consumer...\n");
        pthread_cond_wait(&ring_buffer->is_empty, &ring_buffer->ring_buffer_mutex);
    }
    ring_buffer->buffer[ring_buffer->write_head] = item;
    ring_buffer->write_head = (ring_buffer->write_head + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&ring_buffer->ring_buffer_mutex);
    printf("finished enqueuing.\n");
    pthread_cond_signal(&ring_buffer->is_full);
    fflush(stdout);
}

void dequeue(ring_buffer_t * ring_buffer, char ** item){
    pthread_mutex_lock(&ring_buffer->ring_buffer_mutex);
    while (ring_buffer_empty(ring_buffer)){
        printf("waiting for a producer...\n");
        pthread_cond_wait(&ring_buffer->is_full, &ring_buffer->ring_buffer_mutex);
    }
    *item = ring_buffer->buffer[ring_buffer->read_head];
    ring_buffer->read_head = (ring_buffer->read_head + 1) % BUFFER_SIZE;
    pthread_mutex_unlock(&ring_buffer->ring_buffer_mutex);
    printf("finished dequeuing.\n");
    pthread_cond_signal(&ring_buffer->is_empty);
    fflush(stdout);
}

int ring_buffer_full(const ring_buffer_t * ring_buffer){
    int next_idx = (ring_buffer->write_head + 1) % BUFFER_SIZE;
    if (next_idx == ring_buffer->read_head) return 1;
    return 0;
}


int ring_buffer_empty(const ring_buffer_t * ring_buffer){
    if (ring_buffer->write_head == ring_buffer->read_head) return 1;
    return 0;
}


void broker_init(broker_t * my_broker){
    my_broker->subscribe_count = 0;
    memset(my_broker->topics, 0, sizeof(my_broker->topics));
    for (int i = 0 ; i < MAX_TOPICS; i++){
        ring_buffer_create(&my_broker->ring_buffer[i]);
        memset(my_broker->subscribers[i].sub_ids, 0, sizeof(void *) * MAX_SUBS);
    }
}

void subscriber_init(subscriber_t * my_subscriber){
    my_subscriber->message = NULL;
    for (int i = 0; i < MAX_TOPICS; i++)
        my_subscriber->topic_id[i] = 0;
}


void broker_deinit(broker_t * my_broker){
    my_broker->subscribe_count = 0;
    memset(my_broker->topics, 0, sizeof(my_broker->topics));
    for (int i = 0 ; i < MAX_TOPICS; i++){
        ring_buffer_create(&my_broker->ring_buffer[i]);
        memset(my_broker->subscribers[i].sub_ids, 0, sizeof(void *) * MAX_SUBS);
    }
}

void subscribe(subscriber_t * my_subscriber, broker_t * my_broker, uint8_t topic){
    if (my_subscriber->topic_id[topic] == 0){
        my_broker->subscribe_count++;
        my_subscriber->topic_id[topic] = 1;
        uint8_t idx = my_broker->topics[topic];
        my_broker->subscribers[topic].sub_ids[idx] = (uint8_t *)my_subscriber;
        my_broker->topics[topic]++;
    }
}

void publish(broker_t * my_broker, uint8_t topic_id, char * message){
    if (0 == my_broker->topics[topic_id]){
        printf("There is no subscriber to this topic %d\n", topic_id);
    } else {
        printf("Message: %s to %d\n", message, topic_id);
        
        // this will be placed on a separate thread in the multithreaded variant
        for (int i = 0; i < my_broker->topics[topic_id]; i++){
            printf("This message is going into the queue: %s\n", message);
            // if the ring buffer is full it waits, this property is not too useful for this implementation
            enqueue(&my_broker->ring_buffer[topic_id], message);
        }
    }
}


// this function could buffer overflow if for some reason, the array for the expected subscriber is less than the actual subscriber 
void get_subscribers(broker_t * my_broker, uint8_t topic_id, subscriber_t ** subscribers){
    for (int i = 0; i < my_broker->topics[topic_id]; i++){
        subscribers[i] = (subscriber_t *)my_broker->subscribers[topic_id].sub_ids[i];
    }
}

void recieve_message(const subscriber_t * my_subscriber){
    for (int i = 0; i < MAX_TOPICS; i++){
        if (my_subscriber->topic_id[i] != 0){
            printf("Recieved: %s\n", my_subscriber->message);
        }
    }
}