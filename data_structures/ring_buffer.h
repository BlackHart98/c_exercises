#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


#define RB_LOCAL static


#define ring_buffer_init(buf, type, len) ring_buffer_init_fn(((char *)buf), sizeof(type), len)

typedef struct ring_buffer_t {
    int64_t  read_head, write_head;
    uint64_t type;
    uint64_t len;
    uint64_t count;
    char     *buf;
} ring_buffer_t;


RB_LOCAL ring_buffer_t 
ring_buffer_init_fn(char *buf, uint64_t type, uint64_t len);

RB_LOCAL bool 
ring_buffer_enqueue(ring_buffer_t *, char *);

RB_LOCAL bool 
ring_buffer_dequeue(ring_buffer_t *, char *);


// int 
// ring_buffer_peek(ring_buffer_t *, char *);

RB_LOCAL bool 
ring_buffer_empty(ring_buffer_t *ring_buffer);


RB_LOCAL bool 
ring_buffer_full(ring_buffer_t *ring_buffer);



// #define RB_IMPLEMENTATION
#ifdef RB_IMPLEMENTATION

ring_buffer_t 
ring_buffer_init_fn(char *buf, uint64_t type, uint64_t len)
{
    assert((NULL != buf)&&"Buffer cannot be null");
    assert((type > 0)&&"Type should be greater than 0");
    assert((len >= 3)&&"Length should be greater than 2");
    return (ring_buffer_t){
        .read_head  = 0,
        .write_head = 0,
        .type       = type,
        .len        = len,
        .count      = 0,
        .buf        = buf,
    };
}


bool 
ring_buffer_dequeue(ring_buffer_t *ring_buffer, char *res)
{
    assert((NULL != ring_buffer)&&"Ring cannot be null");
    uint64_t N = ring_buffer->len;
    uint64_t T = ring_buffer->type;
    if (ring_buffer->count == 0) return false;
    
    memcpy(res, ring_buffer->buf + (ring_buffer->read_head * T), T);
    ring_buffer->read_head = ((ring_buffer->read_head + 1) % N);
    ring_buffer->count -= 1;

    return true;
}


bool 
ring_buffer_enqueue(ring_buffer_t *ring_buffer, char* item)
{
    assert((NULL != ring_buffer)&&"Ring cannot be null");
    uint64_t N = ring_buffer->len;
    uint64_t T = ring_buffer->type;
    if (ring_buffer->count == N) return false;

    memcpy(ring_buffer->buf + (ring_buffer->write_head * T), item, T);
    ring_buffer->write_head = ((ring_buffer->write_head + 1) % N);
    ring_buffer->count += 1;

    return true;
}


// RB_LOCAL int 
// ring_buffer_peek(ring_buffer_t *ring_buffer, char *item)
// {
//     assert((NULL != ring_buffer)&&"Buffer cannot be null");
//     if (ring_buffer->write_head == ring_buffer->read_head) return -1;
//     *item = ring_buffer->buffer[ring_buffer->read_head];
//     return 1;
// }


bool 
ring_buffer_empty(ring_buffer_t *ring_buffer)
{
    assert((NULL != ring_buffer)&&"Buffer cannot be null");
    return ring_buffer->count == 0;
}


bool 
ring_buffer_full(ring_buffer_t *ring_buffer)
{
    assert((NULL != ring_buffer)&&"Buffer cannot be null");
    uint64_t N = ring_buffer->len;
    return ring_buffer->count == N;
}

#endif

#endif