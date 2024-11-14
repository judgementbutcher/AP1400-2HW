#include "hw1.h"
#include <algorithm>
#include <cfloat>
#include <cinttypes>
#include <cmath>
#include <numeric>
#include<random>
#include<iomanip>
#include<iostream>
#include <stdexcept>

Matrix algebra::zeros(size_t n, size_t m) {
    Matrix matrix(n, std::vector<double>(m,0));
    return matrix;
}

Matrix algebra::ones(size_t n, size_t m) {
    Matrix matrix(n, std::vector<double>(m, 1));
    return matrix;
}

Matrix algebra::random(size_t n, size_t m, double min, double max) {
    //general usage of random library
    if (max < min) {
        throw std::logic_error("min couldn't greater than max!");
    }
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(min, std::nextafter(max,DBL_MAX));

    Matrix matrix(n, std::vector<double>(m));

    //using for each and lambda function to fill a two dimension vector with random number
    std::for_each(matrix.begin(), matrix.end(), [&](std::vector<double> &row) {
        std::generate(row.begin(), row.end(), [&](){ return dist(mt);});
    });

    return matrix;
}

void algebra::show(const Matrix& matrix) {
    for(auto const& row : matrix) {
        for(auto const& element: row) {
            std::cout << std::fixed << std::setprecision(3) << element << " ";
        }
        std::cout << std::endl;
    }
}

Matrix algebra::multiply(const Matrix& matrix, double c) {
    //first version: using for loop
    Matrix re_matrix{zeros(matrix.size(), matrix[0].size())};
    for(size_t i = 0; i < matrix.size(); i++) {
        for(size_t j = 0; j < matrix[0].size();j++) {
            re_matrix[i][j] = matrix[i][j] * c;
        }
    }
    return re_matrix;
}

Matrix algebra::multiply(const Matrix &matrix1, const Matrix &matrix2) {
    if(matrix1.size() == 0) {
        return matrix1;
    }
    //if the matrix doesn't match, throw a logic error.
    if (matrix1[0].size() != matrix2.size()) {
        throw std::logic_error("the matrix1's column don't match the matrix2's row!");
    }
    Matrix matrix{zeros(matrix1.size(), matrix2[0].size())};
    for(size_t i = 0;i < matrix1.size(); i++) {
        for(size_t j = 0;j < matrix2[0].size();j++) {
            for(size_t k = 0; k < matrix2.size();k++) {
                matrix[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
    return matrix; 
}

Matrix algebra::sum(const Matrix &matrix, double c) {
    if(matrix.size() == 0) {
        return matrix;
    }
    Matrix re_matrix{zeros(matrix.size(),matrix[0].size())};
    for(size_t i = 0; i < matrix.size();i++) {
        for(size_t j = 0; j < matrix[0].size(); j++) {
            re_matrix[i][j] = matrix[i][j] + c;
        }
    }
    return re_matrix;
}

Matrix algebra::sum(const Matrix &matrix1, const Matrix &matrix2) {
    if(matrix1.size() == 0) {
        return matrix1;
    }
    if(matrix1.size() != matrix2.size() || matrix1[0].size() != matrix2[0].size()) {
        throw std::logic_error("matrices with wrong dimensions cannot be summed");
    }
    Matrix matrix{zeros(matrix1.size(), matrix1[0].size())};
    for(size_t i = 0;i < matrix.size(); i++) {
        for(size_t j = 0;j < matrix[0].size();j++) {
            matrix[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
    return matrix;
}

Matrix algebra::transpose(const Matrix& matrix) {
    if(matrix.size() == 0) {
        return matrix;
    }
    Matrix re_matrix{zeros(matrix[0].size(), matrix.size())};
    for(size_t i = 0;i < re_matrix.size();i++) {
        for(size_t j = 0; j < re_matrix[0].size();j++) {
            re_matrix[i][j] = matrix[j][i];
        }
    }
    return re_matrix;
}

Matrix algebra::minor(const Matrix& matrix, size_t n, size_t m) {
    Matrix re_matrix;
    for(size_t i = 0; i < matrix.size();i++) {
        if(i == n)continue;
        std::vector<double> column;
        for(size_t j = 0;j < matrix[0].size(); j++) {
            if(j == m)continue;
            column.push_back(matrix[i][j]);
        }
        re_matrix.push_back(column);
    }
    return re_matrix;
}

double algebra::determinant(const Matrix &matrix) {
    //base case
   if (matrix.size() == 0) {
        return 1;
   } 
   // check non-square
   if(matrix.size() != matrix[0].size()) {
        throw std::logic_error("non-square matrices have no determinant");
   }
   // base case
   if(matrix.size() == 2) {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
   }
   // how to transform to a tail recursion?
   double determinant_value = 0;
   for(size_t i = 0; i < matrix[0].size(); i++) {
        if(i % 2 == 0){
            determinant_value += matrix[0][i] * determinant(minor(matrix, 0, i));
        } else {
            determinant_value -= matrix[0][i] * determinant(minor(matrix,0,i));
        }
   }
   return determinant_value;
}

//this function does a lot copy, how to optimization? swap?
Matrix algebra::inverse(const Matrix &matrix) {
    if (matrix.size() == 0) {
        return matrix;
    }
    if (matrix.size() != matrix[0].size()) {
        throw std::logic_error("non-square matrices have no inverse");
    }
    if (determinant(matrix) == 0) {
        throw std::logic_error("singular matrices have no inverse");
    }
    Matrix follow_matrix;
    for(size_t i = 0;i < matrix.size(); i++) {
        std::vector<double> row;
        for(size_t j = 0;j < matrix[0].size();j++) {
            row.push_back(pow(-1, i + j) * determinant(minor(matrix, i, j)));
        }
        follow_matrix.push_back(row);
    }
    Matrix re_matrix{transpose(follow_matrix)};
    double determinant_value = determinant(matrix);
    re_matrix = multiply(re_matrix, 1.0 / determinant_value);
    return re_matrix;
}

Matrix algebra::concatenate(const Matrix &matrix1, const Matrix &matrix2, int axis) {
    if(axis == 1 && matrix1.size() != matrix2.size() ||
    axis == 0 && matrix1[0].size() != matrix2[0].size()) {
        throw std::logic_error("matrices with wrong dimensions cannot be concatenated");
    }
    size_t row_num = matrix1.size();
    size_t column_num = matrix1[0].size();
    if(axis) {
        Matrix re_matrix{zeros(row_num, column_num + matrix2[0].size())};
        for(size_t i = 0; i < re_matrix.size(); i++) {
            for(size_t j = 0; j < re_matrix[0].size(); j++) {
                if(j < column_num) {
                    re_matrix[i][j] = matrix1[i][j];
                } else {
                    re_matrix[i][j] = matrix2[i][j - column_num]; 
                }
            }
        }   
        return re_matrix;
    } else {
        Matrix re_matrix{zeros(row_num + matrix2.size(), column_num)};
        for(size_t i = 0; i < re_matrix.size(); i++) {
            for(size_t j = 0; j < re_matrix[0].size(); j++) {
                if(i < row_num) {
                    re_matrix[i][j] = matrix1[i][j];
                } else {
                    re_matrix[i][j] = matrix2[i - row_num][j]; 
                }
            }
        }   
        return re_matrix;      
    }
}

Matrix algebra::ero_swap(const Matrix &matrix, size_t r1, size_t r2) {
    if(r1 >= matrix.size() || r2 >= matrix.size()) {
        throw std::logic_error("r1 or r2 inputs are out of range");
    }
    Matrix re_matrix{zeros(matrix.size(), matrix[0].size())};
    for(size_t i = 0; i < re_matrix.size();i++) {
        for(size_t j = 0; j < re_matrix[0].size();j++) {
            if(i == r1) {
                re_matrix[i][j] = matrix[r2][j];
            } else if(i == r2) {
                re_matrix[i][j] = matrix[r1][j];
            } else {
                re_matrix[i][j] = matrix[i][j];
            }
        }
    }
    return re_matrix;
}

Matrix algebra::ero_multiply(const Matrix &matrix, size_t r, double c) {
    if (r < 0 || r > matrix.size()) {
        throw std::logic_error("r out of range");
    }
    Matrix re_matrix{zeros(matrix.size(), matrix[0].size())};
    for(size_t i = 0;i < matrix.size();i++) {
        for(size_t j = 0; j < matrix[0].size();j++) {
            if(i == r) {
                re_matrix[i][j] = matrix[i][j] * c;
            } else {
                re_matrix[i][j] = matrix[i][j];
            }
        }
    }
    return re_matrix;
}

Matrix algebra::ero_sum(const Matrix &matrix, size_t r1, double c, size_t r2) {
    if( (r1 < 0 || r1 > matrix.size()) || (r2 < 0 || r2 > matrix.size() ) ) {
        throw std::logic_error("r1 or r2 out of range.");
    }
    Matrix re_matrix{zeros(matrix.size(), matrix[0].size())};
    for(size_t i = 0; i < re_matrix.size();i++) {
        for(size_t j = 0;j < re_matrix[0].size();j++) {
            if(i == r2) {
                re_matrix[i][j] = matrix[r1][j] * c + matrix[i][j];
            } else {
                re_matrix[i][j] = matrix[i][j];
            }
        }
    }
    return re_matrix;
}


Matrix algebra::upper_triangular(const Matrix &matrix) {
    
}
