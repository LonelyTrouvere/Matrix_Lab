#include "Matrix.h"
#include <iostream>
#include "test.hpp"
#include "GaussianInversion.h"
#include "MinorInversion.hpp"

int main()
{
    Matrix k(4,4,true);
    size_t N = 4;
    GaussianInversion g;

    Matrix m = g.inverse(k);
    Matrix A;
    Matrix e = A.Strassen(m, k, N);

    for (int i=0; i<e.height() ; i++) {
        for (int j = 0; j < e.width(); j++)
            std::cout << e[i][j] << ' ';
        std::cout<<std::endl;
    }

}