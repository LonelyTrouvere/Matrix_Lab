
#include "Helper.hpp"
#include "GaussianInversion.h"
#include "doctest.h"

Matrix GaussianInversion::inverse(const Matrix &mat)
{

    if (mat.height() != mat.width())
        throw std::invalid_argument("The matrix in not square");

    if (!abs(mat.D()))
        throw std::invalid_argument("Determinant is zero");

    size_t _size = mat.height();
    Matrix inverse(_size, _size);

    for (int i = 0; i < _size; i++)
        inverse[i][i] = 1;

    for (int k = 0; k < _size; k++)
    {
        std::complex<double> kk = mat[k][k];
        for (int i = 0; i < _size; i++)
        {
            mat[k][i] /= kk;
            inverse[k][i] /= kk;
        }

        for (int i = 0; i < _size; i++)
        {
            if (i == k)
                continue;
            else
            {
                std::complex<double> div = mat[i][k] / mat[k][k];
                for (int j = 0; j < _size; j++)
                {
                    mat[i][j] -= div * mat[k][j];
                    inverse[i][j] -= div * inverse[k][j];
                }
            }
        }
    }

    for (int i = 0; i < _size; i++)
        for (int j = 0; j < _size; j++)
            if (abs(inverse[i][j]) < 1.0e-12)
                inverse[i][j] = 0;

    return inverse;
}

TEST_CASE("name2")
{
    Matrix ToInverse("inverse_test_data/test1.txt"); // "inverse_test_data/test1.txt"
    GaussianInversion m;
    Matrix CorrectAnswer("inverse_test_data/answer1.txt");

    Matrix Result = m.inverse(ToInverse);
    check::eq(1.2, 1.2);

    for (size_t i = 0; i < Result.height(); ++i)
    {
        for (size_t j = 0; j < Result.width(); ++j)
            CHECK(check::eq(Result[i][j] CorrectAnswer[i][j]));
    }
}