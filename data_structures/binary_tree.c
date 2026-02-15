#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct binary_node_t {
    struct binary_node_t * left;
    struct binary_node_t * right;
    void * content;
} binary_node_t;


typedef struct binary_tree_t {
    size_t type_;
    binary_node_t * node;
}binary_tree_t;

void binary_tree_create(binary_tree_t *, size_t);
void binary_tree_insert(binary_tree_t *, void *);
void binary_tree_delete(binary_tree_t *, void *);
void binary_tree_get_item(binary_tree_t *, void *);
int left_gte_right(void *, void *); // 0 or > 0 



int main(int argc, char *  argv[]){
    binary_tree_t binary_tree;
    binary_tree_create(&binary_tree, sizeof(int));
    printf("Hey here is the binary tree %s\n", (char *)binary_tree.node);

    int x = 6;


    binary_tree_insert(&binary_tree, (void *)&x);
    
    printf("Hey here is the binary tree %d\n", *(int *)binary_tree.node->content);
    return 0;
}

void binary_tree_create(binary_tree_t * binary_tree,  size_t type_){
    binary_tree->type_ = type_;
    binary_tree->node = NULL;
}

void binary_tree_insert(binary_tree_t * binary_tree, void * content){
    if (NULL == binary_tree->node){
        printf("got here.........\n");
        binary_tree->node = (binary_node_t *) malloc(sizeof(binary_node_t));
        if (NULL == binary_tree->node){
            printf("Out of memory!\n");
            return;
        }
        binary_tree->node->content = (void *)malloc(binary_tree->type_);
        if (NULL == binary_tree->node->content){
            printf("Out of memory!\n");
            return;
        }
        memcpy(binary_tree->node->content, content, binary_tree->type_);
        printf("Yello! this is the content: #%d\n", *(int *)binary_tree->node->content);
        return;
    } else {
        binary_node_t * temp = binary_tree->node;
        while(temp != NULL){
            if (left_gte_right(temp->content, content)){
                temp = temp->left;
            } else {
                temp = temp->right;
            }
        }
        temp->content = (void *)malloc(binary_tree->type_);
        if (NULL == temp->content){
            printf("Out of memory!\n");
            return;
        }
        memcpy(temp->content, content, binary_tree->type_);
        return;
    }
}

// for illustration: int type
int left_gte_right(void * left, void * right){
    int l_ = *(int *) left;
    int r_ = *(int *) right;
    return l_ >= r_;
}
