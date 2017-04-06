#include "matrix_extend.h"
#include <stdlib.h>
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

    dst->priv = getmatrix(l_row,r_col);
    dst->row=l_row;
    dst->col=r_col;
    for(int i=0; i<l_row; i++) {
        for(int j=0; j<r_col; j++) {
            PRIV(dst)[i][j]=0;
            for(int k=0; k<l_col; k++)
                PRIV(dst)[i][j]+=PRIV(l)[i][k]*PRIV(r)[k][j];
        }
    }

    return true;
}

MatrixAlgo MatrixProvider = {
    .assign=assign,
    .equal=equal,
    .mul=mul,
};
