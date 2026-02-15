#include <stdio.h>
#include <stdlib.h>


typedef struct _linked_list_t{
    int content;
    struct _linked_list_t * next;
} linked_list_t;


void linked_list_add_head(linked_list_t **, int);
void linked_list_remove_head(linked_list_t **);
void linked_list_add_pos(linked_list_t **, int, uint32_t);
void linked_list_remove_pos(linked_list_t **, int);
void linked_list_add_tail(linked_list_t **, int);
void linked_list_remove_tail(linked_list_t **);



// utility function
void linked_list_traverse(linked_list_t *);


int main(int argc, char* argv[]){
    linked_list_t * my_linked_list = NULL;
    linked_list_add_head(&my_linked_list, 3);
    linked_list_add_head(&my_linked_list, 4);
    linked_list_remove_tail(&my_linked_list);
    linked_list_traverse(my_linked_list);
    return 0;
}


void linked_list_add_head(linked_list_t ** linked_list, int item){
    linked_list_t * new_node = (linked_list_t *) malloc(sizeof(linked_list));
    if (*linked_list == NULL) {
        new_node->content = item;
        new_node->next = NULL;
        *linked_list = new_node;
        return;
    }
    if ((*linked_list)->next == NULL) {
        new_node->content = item;
        new_node->next = NULL;
        *linked_list = new_node;
        return;
    }
    linked_list_t * temp_node = *linked_list;
    while(temp_node->next->next != NULL){
        temp_node = temp_node->next;
    }
    // *linked_list = new_node;
}


void linked_list_add_tail(linked_list_t ** linked_list, int item){
    linked_list_t * new_node = (linked_list_t *) malloc(sizeof(linked_list));
    new_node->content = item;
    new_node->next = NULL;
    if (*linked_list == NULL) {
        *linked_list = new_node;
        return;
    }
    linked_list_t * temp_node = linked_list;
    while(temp_node->next->next != NULL){
        temp_node = temp_node->next;
    }
    temp_node->next->next = new_node;
}


void linked_list_add_pos(linked_list_t ** linked_list, int item, u_int32_t pos){
    linked_list_t * new_node = (linked_list_t *) malloc(sizeof(linked_list));
    new_node->content = item;
    // new_node->next = NULL;
    if (*linked_list == NULL) {
        *linked_list = new_node;
        return;
    }
    // linked_list_t * temp_node = linked_list;
    // while(temp_node->next->next != NULL){
    //     temp_node = temp_node->next;
    // }
    // temp_node->next->next = new_node;
}




void linked_list_remove_head(linked_list_t ** linked_list){
    if (*linked_list == NULL) return;
    linked_list_t * temp_node = *linked_list;
    *linked_list = (*linked_list)->next;
    free(temp_node);
}


void linked_list_remove_tail(linked_list_t ** linked_list){
    if (*linked_list == NULL) return;
    linked_list_t * temp_node = *linked_list;
    if (temp_node->next == NULL) {
        free(temp_node);
        temp_node = NULL;
    }
    while(temp_node->next->next != NULL){
        temp_node = temp_node->next;
    }
    free(temp_node->next);
    temp_node->next = NULL;
}


void linked_list_traverse(linked_list_t * linked_list){
    linked_list_t * temp_node = linked_list;
    while(temp_node != NULL){
        printf("item is %d\n", temp_node->content);
        temp_node = temp_node->next;
    }
}