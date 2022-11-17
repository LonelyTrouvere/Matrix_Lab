#include "Matrix.h"
#include <iostream>
#include <fstream>
#include <random>

Matrix::Matrix(size_t height, size_t width, bool random_init)
{

    if (height < 1 || width < 1)
        throw std::invalid_argument("Matrix width and height must be 1 or larger");

    matrix = new std::complex<double> *[height];
    for (size_t i = 0; i < height; i++)
        matrix[i] = new std::complex<double>[width];

    _height = height;
    _width = width;
    if (random_init)
    {
        srand(time(NULL));
        for (size_t i = 0; i < height; ++i)
        {
            for (size_t j = 0; j < width; ++j)
            {
                matrix[i][j] = std::complex<double>(rand() % 100, rand() % 100);
            }
        }
    }
}

Matrix::Matrix(size_t height, size_t width, std::pair<double, double> **matrixOfPairs) : Matrix(height, width)
{
    for (size_t i = 0; i < height; ++i)
    {
        for (size_t j = 0; j < width; ++j)
        {
            matrix[i][j] = std::complex<double>(matrixOfPairs[i][j].first, matrixOfPairs[i][j].second);
        }
    }
}

Matrix::Matrix(std::string filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::invalid_argument("No file with such name exists: " + filename);

    size_t width, height;
    file >> height >> width;
    // Unfortuantely c++ doesn't allow to call a
    // constructor inside of another constructor

    if (height < 1 || width < 1)
        throw std::invalid_argument("Width or height of matrix is not correct, file may be corrupted");

    _height = height;
    _width = width;

    matrix = new std::complex<double> *[height];
    for (size_t i = 0; i < height; i++)
        matrix[i] = new std::complex<double>[width];

    double real, imagine;
    for (size_t i = 0; i < height; ++i)
    {
        for (size_t j = 0; j < width; ++j)
        {
            file >> matrix[i][j];
        }
    }
}

Matrix::Matrix(const Matrix &src)
{
    _height = src._height;
    _width = src._width;

    matrix = new std::complex<double> *[_height];
    for (size_t i = 0; i < _height; i++)
        matrix[i] = new std::complex<double>[_width];

    for (int i = 0; i < _height; i++)
        for (int j = 0; j < _width; j++)
            matrix[i][j] = src[i][j];
}

Matrix::Matrix(const Matrix &src, size_t x_skip, size_t y_skip)
{

    if (src.height() < 2 || src.width() < 2)
        throw std::invalid_argument("Matrix with size 1 or less can't have minor");

    if (x_skip > src.height() - 1 || y_skip > src.height() - 1)
        throw std::invalid_argument("x_skip or y_skip  is greater than minor matrix size");

    _height = src._height - 1;
    _width = src._width - 1;

    matrix = new std::complex<double> *[_height];
    for (size_t i = 0; i < _height; ++i)
        matrix[i] = new std::complex<double>[_width];

    size_t i_add, j_add;
    std::complex<double> tmp;
    for (size_t i = 0; i < _height; ++i)
        for (size_t j = 0; j < _width; ++j)
        {
            i_add = (i < y_skip) ? 0 : 1;

            j_add = (j < x_skip) ? 0 : 1;

            tmp = src[i + i_add][j + j_add];

            matrix[i][j] = tmp;
        }
}

Matrix::~Matrix()
{

    for (size_t i = 0; i < _height; i++)
        delete matrix[i];

    delete matrix;
}

/// operators
/*
Matrix &Matrix::operator=(const Matrix &other) const
{
    Matrix ret(other);
    return ret;
}
*/

Matrix &Matrix::operator=(const Matrix src) {

    if (src.height() != this->height() || src.width() != this->width())
        throw std::invalid_argument("Matrices with different sizes");

    for (int i=0; i<this->_height; i++)
        for (int j=0; j<this->_width; j++)
            this->matrix[i][j] = src[i][j];

}

Matrix Matrix::operator+(const Matrix &other) const
{
    if (this->height() != other.height() || this->width() != other.width())
        throw std::invalid_argument("Can't add matrices with different sizes");

    Matrix a(*this);
    Matrix ret(this->height(), this->width());

    for (int i = 0; i < this->height(); i++)
        for (int j = 0; j < this->width(); j++)
            ret[i][j] = a[i][j] + other[i][j];

    return ret;
}

Matrix Matrix::operator-(const Matrix &other) const
{
    if (this->height() != other.height() || this->width() != other.width())
        throw std::invalid_argument("Can't subtract matrices with different sizes");

    Matrix a(*this);
    Matrix ret(this->height(), this->width());

    for (int i = 0; i < this->height(); i++)
        for (int j = 0; j < this->width(); j++)
            ret[i][j] = a[i][j] - other[i][j];

    return ret;
}

Matrix Matrix::operator*(const Matrix &other) const
{

    if (this->width() != other.height())
        throw std::invalid_argument("Can't multiply this matrices");

    Matrix a(*this);
    Matrix ret(a.height(), other.width());

    // O(n^3) !! JACKPOT !!
    for (int x = 0; x < a.height(); x++)
        for (int k = 0; k < other.width(); k++)
        {
            ret[x][k] = 0;
            for (int i = 0; i < a.width(); i++)
                ret[x][k] += a[x][i] * other[i][k];
        }

    return ret;
}



Matrix Matrix::Strassen(Matrix A, Matrix B, size_t N)
{
    if (N == 1)
    {
        Matrix C = Matrix(1, 1, false);
        C[0][0] = A[0][0] * B[0][0];
        return C;
    }

    Matrix C = Matrix(N, N, false);


    size_t K = N / 2;

    Matrix A11 = Matrix(K, K, false);
    Matrix A12 = Matrix(K, K, false);
    Matrix A21 = Matrix(K, K, false);
    Matrix A22 = Matrix(K, K, false);
    Matrix B11 = Matrix(K, K, false);
    Matrix B12 = Matrix(K, K, false);
    Matrix B21 = Matrix(K, K, false);
    Matrix B22 = Matrix(K, K, false);

    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][K + j];
            A21[i][j] = A[K + i][j];
            A22[i][j] = A[K + i][K + j];

            B11[i][j] = B[i][j];
            B12[i][j] = B[i][K + j];
            B21[i][j] = B[K + i][j];
            B22[i][j] = B[K + i][K + j];
        }
    }


    Matrix S1 = B12 - B22;
    Matrix S2 = A11 + A12;
    Matrix S3 = A21 + A22;
    Matrix S4 = B21 - B11;
    Matrix S5 = A11 + A22;
    Matrix S6 = B11 + B22;
    Matrix S7 = A12 - A22;
    Matrix S8 = B21 + B22;
    Matrix S9 = A11 - A21;
    Matrix S10 = B11 + B12;

    Matrix P1 = Strassen(A11, S1, K);
    Matrix P2 = Strassen(S2, B22, K);
    Matrix P3 = Strassen(S3, B11, K);
    Matrix P4 = Strassen(A22, S4, K);
    Matrix P5 = Strassen(S5, S6, K);
    Matrix P6 = Strassen(S7, S8, K);
    Matrix P7 = Strassen(S9, S10, K);

    Matrix C11 = ((P5 + P4) + P6) - P2;
    Matrix C12 = P1 + P2;
    Matrix C21 = P3 + P4;
    Matrix C22 = ((P5 + P1) - P3) - P7;


    for (int i = 0; i < K; i++) {
        for (int j = 0; j < K; j++) {
            C[i][j] = C11[i][j];
            C[i][j + K] = C12[i][j];
            C[K + i][j] = C21[i][j];
            C[K + i][K + j] = C22[i][j];
        }
    }
    return C;

}

void Matrix::to_triangle_form()
{
    for (int k = 0; k < _height - 1; k++)
    {

        for (int i = k + 1; i < _height; i++)
        {

            std::complex<double> div = (*this)[i][k] / (*this)[k][k];
            for (int j = k; j < _height; j++)
            {
                (*this)[i][j] -= div * (*this)[k][j];
            }
        }
    }
}

void Matrix::print() const
{

    for (size_t i = 0; i < _height; ++i)
    {
        for (size_t j = 0; j < _width; ++j)
            std::cout << matrix[i][j] << ' ';
        std::cout << '\n';
    }
}

std::string Matrix::str() const
{
    std::string s;
    s += std::to_string(_height) + ' ' + std::to_string(_width) + '\n';

    for (size_t i = 0; i < _height; ++i)
    {
        for (size_t j = 0; j < _width; ++j)
        {
            s += '(' + std::to_string(matrix[i][j].real()) + ',' + std::to_string(matrix[i][j].imag()) + ") ";
        }
        s += '\n';
    }

    return s;
}

void Matrix::to_file(std::string filename) const
{
    std::ofstream file(filename);

    file << this->str();
}

void Matrix::T()
{
    std::complex<double> tmp;

    for (size_t i = 0; i < _height; ++i)
    {
        for (size_t j = i + 1; j < _width; ++j)
        {
            tmp = matrix[i][j];
            matrix[i][j] = matrix[j][i];
            matrix[j][i] = tmp;
        }
    }
}

std::complex<double> Matrix::Dminor(size_t x_skip, size_t y_skip) const
{
    Matrix minor(*this, x_skip, y_skip);
    std::complex<double> res(1, 0);

    minor.to_triangle_form();
    // std::cout << minor.str() << std::endl;

    for (size_t i = 0; i < minor._height; ++i)
    {

        res *= minor[i][i];
    }

    return res;
}

void Matrix::DminorVoid(size_t x_skip, size_t y_skip, std::complex<double> &ret) const
{
    ret = Dminor(x_skip, y_skip);
}

std::complex<double> Matrix::D() const
{

    // throw error when _height != _width

    Matrix det(*this);

    det.to_triangle_form();

    std::complex<double> ans(1, 0);
    for (int k = 0; k < _height; k++)
    {
        ans *= det[k][k];
    }

    return ans;
}