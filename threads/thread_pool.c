#include <stdio.h>

#define WSA_IMPLEMENTATION
#define THREAD_POOL_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/thread_pool.h"

void foo(void *v)
{
    printf("Hello thread pool: foo\n");
}

void foobar(void *v)
{
    printf("Hello thread pool: foobar\n");
}

void bar(void *v)
{
    printf("Hello thread pool: bar\n");
}

int
main(int argc, char *argv[])
{
    context_t context = context_init(KB(50), KB(10));
    if (!context_is_valid(&context)) goto cleanup;
    job_t buf[3]       = {0};
    size_t num_threads = 3;
    thread_pool_t tp   = thread_pool_init(&(context.allocator), num_threads, job_t, (char *)buf, sizeof(buf) / sizeof(job_t));
    if (!thread_pool_is_valid(&tp)) goto cleanup;
    if (!thread_pool_start(&tp))    goto cleanup;

    job_t jobs[] = {
        (job_t){ .function = foo,    .arg = NULL }, 
        (job_t){ .function = foobar, .arg = NULL }, 
        (job_t){ .function = bar,    .arg = NULL },
        (job_t){ .function = bar,    .arg = NULL },
        (job_t){ .function = foobar, .arg = NULL },
        (job_t){ .function = bar,    .arg = NULL },
        (job_t){ .function = bar,    .arg = NULL },
        (job_t){ .function = foobar, .arg = NULL },
        (job_t){ .function = bar,    .arg = NULL },
    };

    for (size_t i = 0; i < (size_t)(sizeof(jobs) / sizeof(job_t)); i++){
        if (!thread_pool_add_work(&tp, jobs[i])) {
            printf("Could not add work\n");
        }
    }

    thread_pool_wait(&tp);
    cleanup:
        thread_pool_deinit(&tp);
        context_deinit(&context);
        return 0;
}
