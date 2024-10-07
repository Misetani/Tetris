#include "tetris.h"
#include "help/matrix.h"

Tetris::Tetris(int height, int width) {
    state = State::START;

    field.blocks = create_matrix(height, width);
}