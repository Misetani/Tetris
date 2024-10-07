#include "matrix.h"

int** create_matrix(int h, int w) {
    int** matrix = new int*[h];

    for (int i = 0; i < h; ++i) {
        matrix[i] = new int[w]();
    }

    return matrix;
}

void free_matrix(int** matrix, int h) {
    for (int i = 0; i < h; ++i) {
        delete matrix[i];
    }

    delete matrix;
}