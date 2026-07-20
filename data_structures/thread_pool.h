#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>

#define RB_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/ring_buffer.h"


#define THREAD_POOL_LOCAL static

#define thread_pool_init(gpa, len, job_type, queue_buf, queue_len) \
    thread_pool_init_fn(gpa, len, queue_len, sizeof(job_type), queue_buf)

/* Control flag for worker threads. */
static volatile int THREADS_KEEPALIVE = 1;
static volatile int THREADS_ON_HOLD   = 0;

typedef struct job_t {
    void (*function)(void *); // I could instead use tagged unions but for now I am using a callback
    void *arg;
} job_t;

typedef struct thread_t {
    size_t    thread_idx;
    pthread_t thread_obj;
} thread_t;


typedef struct thread_pool_t {
    slice_t         threads; // []thread_t
    slice_t         thread_args; // []thpool_arg_t
    volatile int    num_threads_alive;
    volatile int    num_threads_working;
    ring_buffer_t   job_queue; // [N]job_t
    pthread_mutex_t pool_lock; // mutex lock for the thread pool
    pthread_cond_t  has_job; // condition variable for nonempty job queue
    pthread_cond_t  all_idle; // condition variable for completely idle threads
} thread_pool_t;


typedef struct thpool_arg_t {
    size_t        current_idx;
    thread_pool_t *tp;
} thpool_arg_t;


thread_pool_t 
thread_pool_init_fn(
    arena_allocator_t *gpa, 
    size_t size, 
    size_t job_queue_len, 
    size_t job_queue_type, 
    char   *job_queue_buf);


THREAD_POOL_LOCAL void 
thread_pool_deinit(thread_pool_t *tp);


THREAD_POOL_LOCAL bool
thread_pool_is_valid(thread_pool_t *tp);

THREAD_POOL_LOCAL bool 
thread_pool_start(thread_pool_t *tp);  


THREAD_POOL_LOCAL void*
thread_pool_do(thpool_arg_t *self);

THREAD_POOL_LOCAL bool
thread_pool_add_work(thread_pool_t *tp, job_t job);


THREAD_POOL_LOCAL void
thread_pool_wait(thread_pool_t *tp);


#ifdef THREAD_POOL_IMPLEMENTATION


thread_pool_t 
thread_pool_init_fn(
    arena_allocator_t *gpa, 
    size_t len, 
    size_t job_queue_len, 
    size_t job_queue_type, 
    char *job_queue_buf)
{
    assert((gpa != NULL)&&"Allocator cannot be NULL");
    assert((len > 0)&&"Thread pool should be greater than 0");
    assert((job_queue_len > 0)&&"Job queue len should be greater than 0");
    assert((job_queue_type > 0)&&"Invalid job queue type");
    assert((job_queue_buf != NULL)&&"Job buffer cannot be NULL");
    ring_buffer_t r = ring_buffer_init_fn(job_queue_buf, job_queue_type, job_queue_len);
    slice_t threads = arena_allocator_alloc(gpa, thread_t, len);
    if (NULL == threads.ptr) return (thread_pool_t){0};
    slice_t thread_args = arena_allocator_alloc(gpa, thpool_arg_t, len);
    if (NULL == thread_args.ptr) return (thread_pool_t){0};
    pthread_mutex_t pool_lock;
    pthread_mutex_init(&pool_lock, NULL);

    pthread_cond_t has_job;
    pthread_cond_init(&has_job, NULL);

    pthread_cond_t all_idle;
    pthread_cond_init(&all_idle, NULL);

    return (thread_pool_t){
        .threads     = threads,
        .thread_args = thread_args,
        .job_queue   = r,
        .pool_lock   = pool_lock,
        .has_job     = has_job,
        .all_idle    = all_idle,
    };
}


void 
thread_pool_deinit(thread_pool_t *tp)
{
    assert((NULL != tp)&&"Thread pool cannot be null");

    THREADS_KEEPALIVE = 0;
    pthread_mutex_lock(&(tp->pool_lock));
    pthread_cond_broadcast(&(tp->has_job));
    while (tp->num_threads_alive > 0) {
        pthread_cond_wait(&(tp->all_idle), &(tp->pool_lock));
    }
    pthread_mutex_unlock(&(tp->pool_lock));

    pthread_mutex_destroy(&(tp->pool_lock));
    pthread_cond_destroy(&(tp->all_idle));
    pthread_cond_destroy(&(tp->has_job));
}


bool
thread_pool_is_valid(thread_pool_t *tp)
{
    assert((NULL != tp)&&"Thread pool cannot be null");
    return NULL != tp->threads.ptr;
}


bool 
thread_pool_start(thread_pool_t *tp)
{
    assert((NULL != tp)&&"Thread pool cannot be null");
    thread_t *ptr = tp->threads.ptr;
    thpool_arg_t *args = tp->thread_args.ptr;
    size_t N = tp->threads.len_in_bytes / sizeof(thread_t);
    for (size_t i = 0; i < N; i++) {
        ptr[i].thread_idx = i;
        args[i].current_idx = i;
        args[i].tp = tp;
        pthread_create(&(ptr[i].thread_obj), NULL, (void*(*)(void *))thread_pool_do, (void *)&args[i]);
        pthread_detach(ptr[i].thread_obj);
    }
    return true;
}


void*
thread_pool_do(thpool_arg_t *self)
{
    assert((NULL != self)&&"Thread arg cannot be null");
    thread_pool_t *tp = self->tp;
    size_t idx = self->current_idx;
    assert((NULL != tp)&&"Thread pool cannot be null");

    pthread_mutex_lock(&(tp->pool_lock));
    tp->num_threads_alive += 1;
    pthread_mutex_unlock(&(tp->pool_lock));

    while (THREADS_KEEPALIVE) {
        pthread_mutex_lock(&(tp->pool_lock));
        while (THREADS_KEEPALIVE && ring_buffer_empty(&(tp->job_queue))){
            pthread_cond_wait(&(tp->has_job), &(tp->pool_lock));
        }
        pthread_mutex_unlock(&(tp->pool_lock));

        if (THREADS_KEEPALIVE){
            pthread_mutex_lock(&(tp->pool_lock));
			tp->num_threads_working += 1;
			pthread_mutex_unlock(&(tp->pool_lock));

            void (*func_buf)(void*);
		    void* arg_buf;
            
            job_t job = {0};
            pthread_mutex_lock(&(tp->pool_lock));
            ring_buffer_dequeue(&(tp->job_queue), (char *)&job);
            pthread_mutex_unlock(&(tp->pool_lock));
            if (NULL != job.function) {
                // printf("Thread index: %zu\t", self->current_idx);
                func_buf = job.function; arg_buf = job.arg;
                func_buf(arg_buf);
            }

            pthread_mutex_lock(&(tp->pool_lock));
            tp->num_threads_working -= 1;
            if (!tp->num_threads_working) pthread_cond_signal(&(tp->all_idle));
            pthread_mutex_unlock(&(tp->pool_lock));
        }
    }

    pthread_mutex_lock(&tp->pool_lock);
	tp->num_threads_alive -= 1;
	pthread_cond_signal(&(tp->all_idle));
	pthread_mutex_unlock(&tp->pool_lock);
    return NULL;
}


bool
thread_pool_add_work(thread_pool_t *tp, job_t job)
{
    assert((NULL != tp)&&"Thread pool cannot be null");
    pthread_mutex_lock(&tp->pool_lock);
    bool ret = ring_buffer_enqueue(&(tp->job_queue), (char *)&job);
    pthread_cond_signal(&(tp->has_job));
    pthread_mutex_unlock(&tp->pool_lock);
    return ret;
}


void
thread_pool_wait(thread_pool_t *tp)
{
    assert((NULL != tp)&&"Thread pool cannot be null");
    pthread_mutex_lock(&tp->pool_lock);
    while (!ring_buffer_empty(&(tp->job_queue)) || tp->num_threads_working) {
        pthread_cond_wait(&(tp->all_idle), &(tp->pool_lock));
    }
    pthread_mutex_unlock(&tp->pool_lock);
}

#endif

#endif /* ARRAY_LIST_H */