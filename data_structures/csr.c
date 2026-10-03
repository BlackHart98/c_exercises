#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define CSR_SUCCEEDED 1
#define CSR_FAILED    -1

typedef struct sparse_matrix_t {
    double *values; // len = NZ
    int *col_idx;  // len = NZ
    int *row_ptr;  // len = row_len + 1
    int nz_len;
    int row_len;
} sparse_matrix_t;


int
matrix_to_csr(double *matrix, int row, int col, sparse_matrix_t *csr);

void
sparse_matrix_deinit(sparse_matrix_t *csr);

int
main(void)
{
    // Matrix to CSR

    double *matrix = (double[]){
        1, 0, 0, 3,
        0, 0, 0, 0,
        0, 4, 0, 6,
        0, 0, 0, 9,
    };

    int row = 4;
    int col = 4;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            double item = *(matrix + (i * row + j));
            printf("%f| ", item);
        }
        printf("\n");
    }

    sparse_matrix_t csr = {0};
    int ret = matrix_to_csr(matrix, row, col, &csr);
    if (ret != CSR_SUCCEEDED) goto cleanup;
    printf("---------------------------\n");
    for (int i = 0; i < csr.row_len; i++) {
        printf("%i | ", csr.row_ptr[i]);
    }
    printf("\n");

cleanup:
    if (ret != CSR_SUCCEEDED) {
        printf("CSR failed!\n");
    }
    sparse_matrix_deinit(&csr);
    return 0;
}


int
matrix_to_csr(double* matrix, int row, int col, sparse_matrix_t *csr)
{
    assert((matrix != NULL)&&"Matrix cannot be NULL");
    assert((csr != NULL)&&"CSR cannot be NULL");
    int result = CSR_SUCCEEDED;

    int nz_len = 0;
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            double item = *(matrix + (i * row + j));
            if (item != (double)0) nz_len += 1;
        }
    }

    csr->values  = (double *)calloc(nz_len, sizeof(double));
    csr->col_idx = (int *)calloc(nz_len, sizeof(int));
    csr->row_ptr = (int *)calloc(row + 1, sizeof(int));
    csr->nz_len  = nz_len;
    csr->row_len = row + 1;

    if (NULL == csr->values || NULL == csr->col_idx || NULL == csr->row_ptr) {
        result = CSR_FAILED;
        goto cleanup;
    }

    int k = 0;
    for (int i = 0; i < row; i++) {
        int col_count = 0;
        for (int j = 0; j < col; j++) {
            double item = *(matrix + (i * row + j));
            if (item != (double)0) {
                csr->values[k]  = item;
                csr->col_idx[k] = j;
                k += 1;
                col_count += 1;
            }
        }
        csr->row_ptr[i + 1] = csr->row_ptr[i] + col_count;
    }

cleanup:
    return result;
}


void
sparse_matrix_deinit(sparse_matrix_t *csr)
{
    if (NULL != csr->values)  free(csr->values);
    if (NULL != csr->row_ptr) free(csr->row_ptr);
    if (NULL != csr->col_idx) free(csr->col_idx);
}