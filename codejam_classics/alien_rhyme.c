#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SENTINEL 0
#define ARRAY_LEN(arr, T) (sizeof(arr)/sizeof(T))

typedef struct LETTER_OCCUR_NODE {
    char letter[26];
    int num_of_occurence[26];
    struct LETTER_OCCUR_NODE* next_node;
} LETTER_OCCUR_NODE;

inline size_t get_letter_postion(char);
inline char get_postion_from_letter(size_t x);

size_t max_capacity = 1024;


size_t get_alien_rhyme(char **word, size_t word_count, LETTER_OCCUR_NODE *buffer){
    size_t result = 0;
    memset(buffer, 0, sizeof(LETTER_OCCUR_NODE) * max_capacity);
    size_t max_rhyme = 0;

    size_t max_depth = 0;
    for (size_t i = 0; i < word_count; i++){
        size_t depth = 0;
        size_t word_len = strlen(word[i]);
        size_t prev_letter_pos = 0;
        for (size_t j = 0; j < word_len; j++){
            if (max_capacity <= depth){
                max_capacity *= 2;
                buffer = (LETTER_OCCUR_NODE *)realloc(buffer, sizeof(LETTER_OCCUR_NODE) * max_capacity);
                if (NULL == buffer) return 0;
                memset(buffer + (sizeof(LETTER_OCCUR_NODE) * (max_capacity >> 1)), 0, (sizeof(LETTER_OCCUR_NODE) * (max_capacity >> 1)));
            }
            size_t letter_pos = get_letter_postion(word[i][word_len - j - 1]);
            buffer[depth].letter[letter_pos] = word[i][word_len - j - 1];
            if (depth == 0) {
                buffer[depth].num_of_occurence[letter_pos] += 1;
            } else if ((buffer[depth].num_of_occurence[letter_pos] + 1) > buffer[depth - 1].num_of_occurence[prev_letter_pos]){
                break;
            } else {
                buffer[depth].num_of_occurence[letter_pos] += 1;
            }
            prev_letter_pos = letter_pos;
            depth++;
            if (max_depth < depth) max_depth = depth;
        }
    }

    size_t tt = word_count;
    for (size_t i = 0; i < max_depth; i++){
        for (size_t j = 0; j < 26; j++){
            if (buffer[max_depth - i - 1].num_of_occurence[j] > 1){
                result += 2;
                word_count -= 2;
            }
            if (word_count <= 1) goto end;
        }
    }
    end:
        return result;
}

int main(int argc, char *argv[]){
    LETTER_OCCUR_NODE *buffer = (LETTER_OCCUR_NODE *)malloc(sizeof(LETTER_OCCUR_NODE) * max_capacity);
    if (NULL == buffer) return 1;

    char *word1[] = {"TARPOL", "PROL"};
    char *word2[] = {"TARPOR", "PROL", "TARPRO"};
    char *word3[] = {"CODEJAM", "JAM", "HAM", "NALAM", "HUM", "NOLOM"};
    char *word4[] = {"PI", "HI", "WI", "FI"};

    char *word5[] = {"PIA", "HIA", "WA"};


    size_t word1_count = ARRAY_LEN(word1, char *);
    printf("Case #1: %lu\n", get_alien_rhyme(word1, word1_count, buffer));

    size_t word2_count = ARRAY_LEN(word2, char *);
    printf("Case #2: %lu\n", get_alien_rhyme(word2, word2_count, buffer));

    size_t word3_count = ARRAY_LEN(word3, char *);
    printf("Case #3: %lu\n", get_alien_rhyme(word3, word3_count, buffer));

    size_t word4_count = ARRAY_LEN(word4, char *);
    printf("Case #4: %lu\n", get_alien_rhyme(word4, word4_count, buffer));

    size_t word5_count = ARRAY_LEN(word5, char *);
    printf("Case #5: %lu\n", get_alien_rhyme(word5, word5_count, buffer));

    free(buffer);
    return 0;
}

size_t get_letter_postion(char x){
    return (x - (char)'A'); 
}

char get_postion_from_letter(size_t x){
    return (char)(x + (char)'A'); 
}