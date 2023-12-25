#ifndef ADVENT2023_MATRIX_H
#define ADVENT2023_MATRIX_H

#include <vector>
#include <initializer_list>

template <class T> 
class Matrix {

public:
    Matrix(std::initializer_list<std::vector<T>> list): data_(list) {}
    Matrix() : data_() {}
    Matrix(size_t nLines) : data_(nLines) {}
    Matrix(size_t nLines, size_t nCols, T value) : data_(nLines, std::vector<T>(nCols, value)) {}

    Matrix(std::vector<std::vector<T>>& v) : data_(v) {}
    Matrix(std::vector<std::vector<T>>&& v) : data_(std::move(v)) {}

    size_t size() const { return data().size(); }

    std::vector<T>& operator[](size_t idx) {
        return data_[idx];
    }

    const std::vector<T>& operator[](size_t idx) const {
        return data_[idx];
    }

    operator std::vector<std::vector<T>>() { return data(); }
    operator std::vector<std::vector<T>>() const { return data(); }

    std::vector<std::vector<T>>& data() { return data_; }
    const std::vector<std::vector<T>>& data() const { return data_; }

    auto begin()        { return data().begin(); }
    auto begin() const  { return data().begin(); }

    auto end()       { return data().end(); }
    auto end() const { return data().end(); }


private:
    std::vector<std::vector<T>> data_;
};

template <class T>
bool invertMatrix(const Matrix<T> &matrix, Matrix<T> &inverse) {
    //  Invert Matrix by Gauss-Jordan elimination
    if ((matrix.size() == 0) || (matrix.size() != matrix[0].size())) {
        return false;
    }
    assert(matrix.size() == 6);

    Matrix<T> augmentedMatrix = matrix;

    std::vector<T> identityLine(matrix.size(), 0);
    for (size_t i=0; i<matrix.size(); ++i) {
        identityLine[i] = 1;
        augmentedMatrix[i].insert(augmentedMatrix[i].end(), identityLine.begin(), identityLine.end());
        identityLine[i] = 0;
    }

    for (size_t i=0; i<matrix.size(); ++i) {
        auto &row = augmentedMatrix[i];
        auto d = row[i];
        if (d == static_cast<T>(0)) {
            return false;
        }
        for (size_t j = 0; j < row.size(); j++) {
            row[j] /= d;
        }

        for (size_t k=0; k<matrix.size(); ++k) {
            if (k == i) {
                continue;
            }

            auto &krow = augmentedMatrix[k];
            auto factor = krow[i];
            if (factor == static_cast<T>(0)) {
                continue;
            }
            for (size_t j = 0; j < row.size(); j++) {
                krow[j] -= factor * row[j];
            }
        }
    }

    inverse = matrix;
    for (size_t i=0; i<inverse.size(); ++i) {
        inverse[i].clear();
        inverse[i].insert(inverse[i].end(), augmentedMatrix[i].begin() + inverse.size(), augmentedMatrix[i].end());
    }

    return true;
}

template <class T>
Matrix<T> transposeMatrix(const Matrix<T> &matrix) {
    Matrix<T> transposed(matrix[0].size());

    for (size_t i=0; i<matrix.size(); ++i) {
        for (size_t j=0; j<matrix[0].size(); ++j) {
            transposed[j].push_back(matrix[i][j]);
        }
    }
    return transposed;
}

template <class T>
Matrix<T> multiplyMatrixes(const Matrix<T> &mat1, const Matrix<T> &mat2) {
    const size_t r1 = mat1.size();
    const size_t c1 = mat1[0].size();
    const size_t r2 = mat2.size();
    const size_t c2 = mat2[0].size();

    assert(c1 == r2);
    Matrix<T> prod(r1, c2, 0);

    for (size_t i = 0; i < r1; i++) {
        for (size_t j = 0; j < c2; j++) {
            for (int k = 0; k < r2; k++) {
                prod[i][j] += mat1[i][k] * mat2[k][j];
            }
        }
    }
    return prod;
}

#endif // ADVENT2023_MATRIX_H
