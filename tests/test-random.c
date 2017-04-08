#include "matrix_extend.h"
#include "stopwatch.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef MATRIXSIZE
#define MATRIXSIZE 1024
#endif

MatrixAlgo *matrix_providers[]= {
    &MatrixProvider,&SSEMatrixProvider,&AVXMatrixProvider,&TRANMatrixProvider
};

float* transpose(float *data)
{
    float *output=malloc(sizeof(float)*MATRIXSIZE*MATRIXSIZE);

    for(int i=0; i<MATRIXSIZE; i++) {
        for(int j=0; j<MATRIXSIZE; j++) {
            output[j*MATRIXSIZE+i]=data[i*MATRIXSIZE+j];
        }
    }

    return output;
}

int main()
{
    Matrix dst[4],src1,src2,src3;
    float* data1,*data2,*data3;
    data1=malloc(sizeof(float)*MATRIXSIZE*MATRIXSIZE);
    data2=malloc(sizeof(float)*MATRIXSIZE*MATRIXSIZE);

    /*set random data*/
    srand(time(NULL));

    for(int i=0; i<MATRIXSIZE*MATRIXSIZE; i++) {
        data1[i]=rand()%99+1;
        data2[i]=rand()%99+1;
    }
    data3=transpose(data2);

    createMatrix(&src1,MATRIXSIZE,MATRIXSIZE);
    createMatrix(&src2,MATRIXSIZE,MATRIXSIZE);
    createMatrix(&src3,MATRIXSIZE,MATRIXSIZE);

    matrix_providers[0]->assign(&src1,data1,MATRIXSIZE,MATRIXSIZE);
    matrix_providers[0]->assign(&src2,data2,MATRIXSIZE,MATRIXSIZE);
    matrix_providers[0]->assign(&src3,data3,MATRIXSIZE,MATRIXSIZE);


    /*start testing*/

    watch_p ctx=Stopwatch.create();
    if(!ctx) return -1;
    for(int i=0; i<3; i++) {
        Stopwatch.start(ctx);
        matrix_providers[i]->mul(&dst[i],&src1,&src2);
        printf("%lf ",Stopwatch.read(ctx));

        Stopwatch.stop(ctx);
        Stopwatch.reset(ctx);
    }
    Stopwatch.start(ctx);
    matrix_providers[3]->mul(&dst[3],&src1,&src3);
    printf("%lf ",Stopwatch.read(ctx));

    Stopwatch.stop(ctx);
    Stopwatch.reset(ctx);
    Stopwatch.destroy(ctx);
    printf("\n");

    free(data1);
    free(data2);

    if(!matrix_providers[0]->equal(&dst[0],&dst[1])) {
        printf("SSE error!\n");
        return -1;
    }

    if(!matrix_providers[0]->equal(&dst[0],&dst[2])) {
        printf("AVX error!\n");
        return -1;
    }

    if(!matrix_providers[0]->equal(&dst[0],&dst[3])) {
        printf("tran error!\n");
        return -1;
    }
    return 0;
}
