#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdbool.h>

/* predefined shortcut */
#define DECLARE_MATRIX(col, row) \
    typedef struct { float values[col][row]; } Mat ## col ## x ## row

typedef struct {
    int row,col;
    void *priv;
} Matrix;

typedef struct {
    void (*assign)(Matrix *thiz, float *data, int row, int col);
    bool (*equal)(const Matrix *l, const Matrix *r);
    bool (*mul)(Matrix *dst, const Matrix *l, const Matrix *r);
} MatrixAlgo;

extern MatrixAlgo MatrixProvider,SSEMatrixProvider,AVXMatrixProvider;
float** getmatrix(int row,int col);
#endif
