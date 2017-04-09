#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "matrix_extend.h"

#define MATRIXSIZE 1024


#ifdef SSE
MatrixAlgo* algo=&SSEMatrixProvider;
#elif AVX
MatrixAlgo* algo=&AVXMatrixProvider;
#elif TRAN
MatrixAlgo* algo=&TRANMatrixProvider;
#else
MatrixAlgo* algo=&MatrixProvider;
#endif

int main()
{
    srand(time(NULL));
    float *data1,*data2;
    Matrix src1,src2,dst;
    data1=malloc(sizeof(float)*MATRIXSIZE);
    data2=malloc(sizeof(float)*MATRIXSIZE);

    for(int i=0; i<MATRIXSIZE; i++) {
        data1[i]=rand()%99+1;
        data2[i]=rand()%99+1;
    }

    createMatrix(&src1,MATRIXSIZE,MATRIXSIZE);
    createMatrix(&src2,MATRIXSIZE,MATRIXSIZE);

    algo->assign(&src1,data1,MATRIXSIZE,MATRIXSIZE);
    algo->assign(&src2,data2,MATRIXSIZE,MATRIXSIZE);

    algo->mul(&dst,&src1,&src2);

    freeMatrix(&src1);
    freeMatrix(&src2);
    freeMatrix(&dst);

    free(data1);
    free(data2);
}
