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
    for (size_t i = 0; i < _height; i++)
        matrix[i] = new std::complex<double>[_width];

    size_t i_add, j_add;

    for (size_t i = 0; i < _height; ++i)
        for (size_t j = 0; j < _width; ++j)
        {
            if (y_skip)
                i_add = i / y_skip;
            else
                i_add = 1;
            if (x_skip)
                j_add = j / x_skip;
            else
                j_add = 1;
            matrix[i][j] = src[i + i_add][j + j_add];
        }
}

Matrix::~Matrix()
{

    for (size_t i = 0; i < _height; i++)
        delete matrix[i];

    delete matrix;
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

    for (size_t i = 0; i < minor._height; ++i)
        res *= minor[i][i];

    return res;
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

Matrix Matrix::GaussianInverse(Matrix mat)
{

    // throw error when _height != _width

    // throw error when matrix's determinant = 0

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