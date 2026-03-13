#include <stdlib.h>
#include <stdio.h>
#include <assert.h>



#define MAX_SIZE 10
#define ARRAY_LEN(T, array)  sizeof(array)/sizeof(T)

typedef enum Object {
    PLAYER  = 0,
    PISTOL  = 1,
    KNIFE   = 2,
    CAR     = 3,
    AXE     = 4,
} Object;


typedef struct array_linked_list_s{int data; int next;}array_linked_list_t;

int
init_thing_holder(array_linked_list_t *things, size_t len);

int
add_new_thing(array_linked_list_t *things, size_t len , Object item);

void
get_things(array_linked_list_t *things, size_t len);

int 
main(void)
{
    array_linked_list_t things[MAX_SIZE] = {0};
    size_t array_len = ARRAY_LEN(array_linked_list_t, things);
    init_thing_holder(things, array_len);
    add_new_thing(things, array_len, PISTOL);
    add_new_thing(things, array_len, PISTOL);
    add_new_thing(things, array_len, KNIFE);
    get_things(things, array_len);

    return 0;
}


int
init_thing_holder(array_linked_list_t *things, size_t len)
{
    things[0].data = PLAYER;
    things[0].next = 1;
    return 1;
}


int
add_new_thing(array_linked_list_t *things, size_t len , Object item)
{
    assert((0 < things[0].next)&&"Holder uninitialized");
    for (int i = 0; i < len - 1; i++){
        int next = things[i].next;
        if (item == things[next].data) break;
        if (0 == next){
            things[i].data = item;
            things[i].next = i + 1;
            break;
        }
    }
    return 1;
}


void
get_things(array_linked_list_t *things, size_t len)
{
    for (int i = 0; i < len - 1; i++){
        int next = things[i].next;
        if (next == 0) break;
        switch (things[next].data){
            case PISTOL:
                printf("Player has a pistol\n");
                break;
            case KNIFE:
                printf("Player has a knife\n");
                break;
            case CAR:
                printf("Player has a car\n");
                break;
            case AXE:
                printf("Player has an axe\n");
                break;
            default:
                break;
        }
    }
}
