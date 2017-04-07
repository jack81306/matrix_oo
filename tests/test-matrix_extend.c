#include "matrix_extend.h"
#include <stdio.h>

MatrixAlgo *matrix_providers[] = {
    &SSEMatrixProvider,
};
int main()
{
    MatrixAlgo *algo = matrix_providers[0];

    Matrix dst, m, n, fixed;
    m.col=m.row=n.col=n.row=fixed.col=fixed.row=4;
    m.priv=getmatrix(4,4);
    n.priv=getmatrix(4,4);
    fixed.priv=getmatrix(4,4);
    float mData[]= {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
    float nData[]= {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8};
    float fixData[]= {34,44,54,64,82,108,134,160,34,44,54,64,82,108,134,160};
    algo->assign(&m, mData,4,4);
    algo->assign(&n, nData,4,4);
    algo->assign(&fixed, fixData,4,4);

    algo->mul(&dst, &m, &n);


    if (algo->equal(&dst, &fixed))
        return 0;
    return -1;
}
