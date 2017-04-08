#include "matrix_extend.h"
#include <stdlib.h>
#include <xmmintrin.h>
#include <immintrin.h>
#define PRIV(x) \
    ((float **) ((x)->priv))

float** getmatrix(int row,int col)
{
    float **m = malloc(sizeof(float*)*row);
    for(int i=0; i<row; i++) {
        m[i]=malloc(sizeof(float)*col);
    }
    return m;
}

void createMatrix(Matrix* target,int row,int col)
{
    target->row=row;
    target->col=col;
    target->priv=getmatrix(row,col);
}

static void assign(Matrix *thiz, float *data, int row, int col)
{
    for(int i=0; i<row; i++) {
        for(int j=0; j<col; j++)
            PRIV(thiz)[i][j]=data[i*col+j];
    }
}

static bool equal(const Matrix *l, const Matrix *r)
{
    if(l->row!=r->row||l->col!=r->col)
        return false;

    for(int i=0; i<l->row; i++) {
        for(int j=0; j<l->col; j++)
            if(PRIV(l)[i][j]!=PRIV(r)[i][j])
                return false;
    }

    return true;
}

static bool mul(Matrix *dst, const Matrix *l,const Matrix *r)
{
    int r_row,r_col,l_row,l_col;
    l_row=l->row;
    l_col=l->col;
    r_row=r->row;
    r_col=r->col;

    if(l_col!=r_row)
        return false;

    createMatrix(dst,l_row,r_col);

    for(int i=0; i<l_row; i++) {
        for(int j=0; j<r_col; j++) {
            PRIV(dst)[i][j]=0;
            for(int k=0; k<l_col; k++)
                PRIV(dst)[i][j]+=PRIV(l)[i][k]*PRIV(r)[k][j];
        }
    }

    return true;
}

static bool sse_mul(Matrix *dst, const Matrix *l,const Matrix *r)
{
    int r_row,r_col,l_row,l_col;
    l_row=l->row;
    l_col=l->col;
    r_row=r->row;
    r_col=r->col;

    if(l_col!=r_row)
        return false;

    if(l_col%4!=0)
        return false;
    createMatrix(dst,l_row,r_col);
    __m128 src1,src2,dst1;
    float* all=malloc(sizeof(float)*4);
    for(int i=0; i<l_row; i++) {
        for(int j=0; j<r_col; j++) {
            PRIV(dst)[i][j]=0;
            for(int k=0; k<l_col; k+=4) {
                src1=_mm_setr_ps(PRIV(l)[i][k],PRIV(l)[i][k+1],PRIV(l)[i][k+2],PRIV(l)[i][k+3]);
                src2=_mm_setr_ps(PRIV(r)[k][j],PRIV(r)[k+1][j],PRIV(r)[k+2][j],PRIV(r)[k+3][j]);
                dst1=_mm_mul_ps(src1,src2);
                _mm_storeu_ps(all,dst1);
                PRIV(dst)[i][j]+=all[0]+all[1]+all[2]+all[3];
            }
        }
    }

    return true;
}


static bool avx_mul(Matrix *dst, const Matrix *l,const Matrix *r)
{
    int r_row,r_col,l_row,l_col;
    l_row=l->row;
    l_col=l->col;
    r_row=r->row;
    r_col=r->col;

    if(l_col!=r_row)
        return false;

    if(l_col%8!=0)
        return false;

    createMatrix(dst,l_row,r_col);

    float* all=malloc(sizeof(float)*8);
    __m256 src1,src2,dst1;
    for(int i=0; i<l_row; i++) {
        for(int j=0; j<r_col; j++) {
            PRIV(dst)[i][j]=0;
            for(int k=0; k<l_col; k+=8) {
                src1=_mm256_set_ps(PRIV(l)[i][k+7],PRIV(l)[i][k+6],PRIV(l)[i][k+5],PRIV(l)[i][k+4],
                                   PRIV(l)[i][k+3],PRIV(l)[i][k+2],PRIV(l)[i][k+1],PRIV(l)[i][k]);
                src2=_mm256_set_ps(PRIV(r)[k+7][j],PRIV(r)[k+6][j],PRIV(r)[k+5][j],PRIV(r)[k+4][j],
                                   PRIV(r)[k+3][j],PRIV(r)[k+2][j],PRIV(r)[k+1][j],PRIV(r)[k][j]);

                dst1=_mm256_mul_ps(src1,src2);
                _mm256_storeu_ps(all,dst1);
                PRIV(dst)[i][j]+=all[0]+all[1]+all[2]+all[3]+all[4]+all[5]+all[6]+all[7];
            }
        }
    }
    return true;
}

static bool mul_transpose(Matrix *dst, const Matrix *l,const Matrix *r)
{
    int r_row,r_col,l_row,l_col;
    l_row=l->row;
    l_col=l->col;
    r_row=r->row;
    r_col=r->col;

    if(l_col!=r_row)
        return false;

    createMatrix(dst,l_row,r_col);

    for(int i=0; i<l_row; i++) {
        for(int j=0; j<r_col; j++) {
            PRIV(dst)[i][j]=0;
            for(int k=0; k<l_col; k++)
                PRIV(dst)[i][j]+=PRIV(l)[i][k]*PRIV(r)[j][k];
        }
    }

    return true;
}

void freeMatrix(Matrix *src)
{
    float **rel=PRIV(src);
    for(int i=0; i<src->row; i++)
        free(rel[i]);
    free(rel);
}

MatrixAlgo MatrixProvider = {
    .assign=assign,
    .equal=equal,
    .mul=mul,
};
MatrixAlgo SSEMatrixProvider = {
    .assign=assign,
    .equal=equal,
    .mul=sse_mul,
};
MatrixAlgo AVXMatrixProvider = {
    .assign=assign,
    .equal=equal,
    .mul=avx_mul,
};
MatrixAlgo TRANMatrixProvider = {
    .assign=assign,
    .equal=equal,
    .mul=mul_transpose,
};
