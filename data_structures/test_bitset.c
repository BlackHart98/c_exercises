#include <stdio.h>

#define BITSET_IMPLEMENTATION
#define WSA_IMPLEMENTATION
#define ARRAY_LIST_IMPLEMENTATION
#include "../memory_management/why_so_arena.c"
#include "../data_structures/bitset.h"

int 
main(int argc, char* argv[])
{
    context_t context = context_init(MB(1), KB(512));
    if (!context_is_valid(&context)) goto cleanup;

    bitset_t my_bitset = bitset_init(&(context.allocator), 100);
    if (!bitset_is_valid(&my_bitset)) goto cleanup;

    printf("Bitset: %zu, %zu\n", my_bitset.size, my_bitset.len);
    bitset_add(&my_bitset, 30);
    bitset_add(&my_bitset, 40);

    if (bitset_test(&my_bitset, 30)) {
        printf("%d in bitset\n", 30);
        bitset_toggle(&my_bitset, 30);
    } else printf("%d not in bitset\n", 30);

    if (bitset_test(&my_bitset, 40)) printf("%d in bitset\n", 40);
    else printf("%d not in bitset\n", 40);

    if (bitset_test(&my_bitset, 30)) printf("%d in bitset\n", 30);
    else printf("%d not in bitset\n", 30);

    cleanup:
        context_deinit(&context);
    return 0;
}

