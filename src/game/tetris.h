#ifndef TETRIS_H
#define TETRIS_H

constexpr int HEIGHT = 20;
constexpr int WIDTH = 10;

class Tetris {
public:

    Tetris(int height = HEIGHT, int width = WIDTH);

private:
    enum class State {
        START,
        SPAWN,
        MOVE,
        SHIFT,
        ATTACH,
        END,
        PAUSE,
    };

    struct Figure {
        int** blocks;

        int size;
        int x;
        int y;
    };

    struct Field {
        int** blocks;

        int height;
        int width;
    };

    Field field;
    Figure figure;

    State state;
};

#endif