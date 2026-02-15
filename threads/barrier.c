#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

/*
    Reimplementation of the barrier on Apple from the Extreme C code example
*/

#define MAX_THREADS 150

struct custom_barrier_t{
    int count;
    int size;
    int group;
    pthread_cond_t cv;
    pthread_mutex_t mtx;
};


typedef struct custom_barrier_t custom_barrier_t;


int custom_barrier_init(custom_barrier_t *barrier, int n);
int custom_barrier_destroy(custom_barrier_t *barrier);
int custom_barrier_wait(custom_barrier_t *barrier);


typedef struct hydrogen_t{
    int *num_of_molecules;
    sem_t *sem;
    custom_barrier_t *water_barrier;
} hydrogen_t;

typedef struct oxygen_t{
    int *num_of_molecules;
    pthread_mutex_t mtx;
    custom_barrier_t *water_barrier;
} oxygen_t;


void* hydrogen_worker(void *arg){
    hydrogen_t *local = (hydrogen_t *) arg;
    sem_wait(local->sem);
    custom_barrier_wait(local->water_barrier);
    fflush(0);
    sem_post(local->sem);
    return NULL;
}

void* oxygen_worker(void *arg){
    printf("Entered oxygen\n");
    oxygen_t *local = (oxygen_t *) arg;
    pthread_mutex_lock(&local->mtx);
    custom_barrier_wait(local->water_barrier);
    (*local->num_of_molecules)++;
    pthread_mutex_unlock(&local->mtx);
    return NULL;
}


int main(int argc, char *argv[]){
    pthread_t workers[MAX_THREADS];
    custom_barrier_t water_barrier;
    int num_of_molecules = 0;

    custom_barrier_init(&water_barrier, 2);

    hydrogen_t hydrogen_s = {0};
    oxygen_t oxygen_s = {0};

    hydrogen_s.sem = sem_open("hydrogen", O_CREAT | O_EXCL, 0644, 1); 
    hydrogen_s.water_barrier = &water_barrier;
    hydrogen_s.num_of_molecules = &num_of_molecules;

    pthread_mutex_init(&oxygen_s.mtx, NULL); 
    oxygen_s.water_barrier = &water_barrier;
    oxygen_s.num_of_molecules = &num_of_molecules;

    int i = 0;
    for (; i < (MAX_THREADS - 50); i++){
        pthread_create(&workers[i], NULL, hydrogen_worker, (void *)&hydrogen_s);
    }

    // for (; i < 2; i++){
    //     pthread_create(&workers[i], NULL, hydrogen_worker, (void *)&hydrogen_s);
    // }
    for (; i < MAX_THREADS; i++){
        pthread_create(&workers[i], NULL, oxygen_worker, (void *)&oxygen_s);
    }

    // for (; i < 3; i++){
    //     pthread_create(&workers[i], NULL, oxygen_worker, (void *)&oxygen_s);
    // }


    for (int i = 0;  i < MAX_THREADS; i++){
        pthread_join(workers[i], NULL);
    }

    // for (int i = 0; i < 3; i++){
    //     pthread_join(workers[i], NULL);
    // }

    printf("Number of water molecules %d\n", num_of_molecules);

    /* Clear after use */ 
    custom_barrier_destroy(&water_barrier);
    pthread_mutex_destroy(&oxygen_s.mtx);
    sem_close(hydrogen_s.sem);
    sem_unlink("hydrogen");
    return 0;
}


int custom_barrier_init(custom_barrier_t *barrier, int n){
    barrier->count = 0;
    barrier->size = n;
    barrier->group = 0;

    pthread_mutex_init(&barrier->mtx, NULL);
    pthread_cond_init(&barrier->cv, NULL);
    return 0;
}


int custom_barrier_destroy(custom_barrier_t *barrier){
    pthread_mutex_destroy(&barrier->mtx);
    pthread_cond_destroy(&barrier->cv);
    return 0;
}


int custom_barrier_wait(custom_barrier_t *barrier){
    /* Enter into the barrier */
    pthread_mutex_lock(&(barrier->mtx));
    barrier->count++;
    if (barrier->count >= barrier->size){
        barrier->count = 0;
        barrier->group++;
        pthread_cond_broadcast(&(barrier->cv));
    } else {
        int group = barrier->group;
        printf("Wait group (%d), count(%d)\n", group, barrier->count);
        // wait for count
        do{
            // printf("Got here hydrogen\n");
            pthread_cond_wait(&(barrier->cv), &(barrier->mtx));
        } while (group == barrier->group); 
        printf("....................\n");
        printf("Got here hydrogen\n");
        printf("....................\n");
        printf("Done, group(%d)\n", group);
        printf("++++++++++++++++++++\n");
    }
    pthread_mutex_unlock(&(barrier->mtx));
    return 0;

}