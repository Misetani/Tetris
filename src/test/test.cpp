#include <gtest/gtest.h>

#include "../game/tetris.h"
#include "../game/help/matrix.h"

TEST(matrix, memory_allocation) {
    int h = 20;
    int w = 10;

    int **matrix = create_matrix(h, w);

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            EXPECT_EQ(matrix[i][j], 0);
        }
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}