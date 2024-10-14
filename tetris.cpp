#include <iostream>
#include <fstream>
#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include <random>
#include <chrono>

class RNG {
public:
    RNG() {
        auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::mt19937::result_type seed = static_cast<std::mt19937::result_type>(now);
        engine.seed(seed);
    }

    int rand(int min, int max) {
        std::uniform_int_distribution<> distr(min, max);
        return distr(engine);
    }

private:
    std::mt19937 engine; // Mersenne Twister engine
};

/* ========================================================================= */
/*                      Matrix manipulation functions                        */
/* ========================================================================= */

void create_matrix(int*** matrix, int h, int w) {
    *matrix = new int*[h];

    for (int i = 0; i < h; ++i) {
        (*matrix)[i] = new int[w];
    }
}

void free_matrix(int** matrix, int h) {
    for (int i = 0; i < h; ++i) {
        delete matrix[i];
    }

    delete matrix;
}

void print_matrix(int** matrix, int h, int w) {
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            std::cout << matrix[i][j] << " ";
        }

        std::cout << '\n';
    }
}

class Tetris {
private:
    int** m_field;
    int m_height{ 20 };
    int m_width{ 10 };

    int** m_figure;
    int m_size{ 4 };
    int m_x{ 0 };
    int m_y{ 0 };

    RNG rng;
    
    bool playing = false;
    bool paused = false;
    bool attaching = false;

public:
    Tetris() {
        init_ncurses();

        create_matrix(&m_field, m_height, m_width);

        m_size = create_figure(&m_figure);

        m_x = (m_width - m_size) / 2;
        m_y = 0;
    }

    ~Tetris() {
        free_matrix(m_field, m_height);
        free_matrix(m_figure, m_size);

        cleanup_ncurses();
    }

    void play() {
        render_game();

        wait_for_start();

        int count = 0;
        int max_count = 10;
        while (playing) {
            usleep(30000);

            process_user_input();

            if (attaching) {
                add_figure_to_field();

                if (is_game_end()) {
                    break;
                }

                spawn_figure();

                attaching = false;
            }

            if (!paused && ++count == max_count) {
                shift_figure();

                count = 0;
            }

            render_game();
        }
    }

private:

/* ========================================================================= */
/*                               Game Library                                */
/* ========================================================================= */

    bool is_game_end() {
        return m_y <= 0;
    }

    void process_user_input() {
        char key = getch();

        if (key == 'q') {
            playing = false;
        } else if (key == 'p') {
            paused = !paused;
        } else if (!paused) {
            if (key == 's') {
                move_figure_down();
            } else if (key == 'a') {
                move_figure_left();
            } else if (key == 'd') {
                move_figure_right();
            } else if (key == 'w') {
                rotate_figure();
            }
        }
    }

    void rotate_figure() {
        int** rotated_figure;
        create_matrix(&rotated_figure, m_size, m_size);

        for (int i = 0; i < m_size; ++i) {
            for (int j = 0; j < m_size; ++j) {
                rotated_figure[j][m_size - i - 1] = m_figure[i][j];
            }
        }

        int** temp_figure = m_figure;
        m_figure = rotated_figure;

        if (!is_figure_in_field()) {
            m_figure = temp_figure;

            free_matrix(rotated_figure, m_size);
        } else {
            free_matrix(temp_figure, m_size);
        }
    }

    void move_figure_down() {
        if (!is_figure_attached()) {
            ++m_y;
        } else {
            attaching = true;
        }
    }

    void move_figure_left() {
        if (!is_figure_attached()) {
            --m_x;

            if (!is_figure_in_field()) {
                ++m_x;
            }
        } else {
            attaching = true;
        }
    }

    void move_figure_right() {
        if (!is_figure_attached()) {
            ++m_x;

            if (!is_figure_in_field()) {
                --m_x;
            }
        } else {
            attaching = true;
        }
    }

    void wait_for_start() {
        char key;
        while ((key = getch()) != '\n') {
            if (key == 'q') {
                return;
            }
        }

        playing = true;
    }

    void spawn_figure() {
        free_matrix(m_figure, m_size);

        create_figure(&m_figure);

        m_x = (m_width - m_size) / 2;
        m_y = 0;
    }

    int create_figure(int*** figure) {
        std::ifstream fin("figures.txt");

        int figure_count;
        fin >> figure_count;

        int figure_number = rng.rand(1, figure_count);

        int count = 0;

        int size;
        while (count != figure_number) {
            fin >> size;

            if (size > 1) {
                ++count;
            }
        }

        create_matrix(&m_figure, size, size);

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                fin >> (*figure)[i][j];
            }
        }

        fin.close();

        m_size = size;

        return size;
    }

    void shift_figure() {
        if (!is_figure_attached()) {
            m_y += 1;
        } else {
            attaching = true;
        }
    }

    bool is_figure_attached() {
        for (int i = 0; i < m_size; ++i) {
            for (int j = 0; j < m_size; ++j) {
                int x = m_x + j;
                int y = m_y + i + 1;

                if (m_figure[i][j] && (y >= m_height || m_field[y][x])) {
                    return true;
                }
            }
        }

        return false;
    }

    void add_figure_to_field() {
        for (int i = 0; i < m_size; ++i) {
            for (int j = 0; j < m_size; ++j) {
                int x = m_x + j;
                int y = m_y + i;

                if (m_figure[i][j] && is_block_in_field(x, y)) {
                    m_field[y][x] = 1;
                }
            }
        }
    }

    void remove_figure_from_field() {
        for (int i = 0; i < m_size; ++i) {
            for (int j = 0; j < m_size; ++j) {
                int x = m_x + j;
                int y = m_y + i;

                if (m_figure[i][j] && is_block_in_field(x, y)) {
                    m_field[y][x] = 0;
                }
            }
        }
    }

    bool is_figure_in_field() {
        for (int i = 0; i < m_size; ++i) {
            for (int j = 0; j < m_size; ++j) {
                int x = m_x + j;
                int y = m_y + i;

                if (m_figure[i][j] && !is_block_in_field(x, y)) {
                    return false;
                }
            }
        }

        return true;
    }

    bool is_block_in_field(int x, int y) {
        return x >= 0 && x < m_width && y >= 0 && y < m_height;
    }

    void init_ncurses() {
        initscr();
        nodelay(stdscr, TRUE);
        curs_set(0);
        cbreak();
        keypad(stdscr, TRUE);
        noecho();
    }

    void cleanup_ncurses() {
        endwin();
    }

    void render_game() {
        if (playing) { add_figure_to_field(); }

        WINDOW *game_window = newwin(m_height + 2, 3 * m_width + 2, 0, 0);

        refresh();
        box(game_window, 0, 0);

        for (int i = 0; i < m_height; ++i) {
            for (int j = 0; j < m_width; ++j) {
                if (m_field[i][j] == 1) {
                    mvwprintw(game_window, i + 1, 3 * j + 1, "[+]");
                } else {
                    mvwprintw(game_window, i + 1, 3 * j + 1, "   ");
                }
            }
        }

        wrefresh(game_window);

        delwin(game_window);

        if (playing) { remove_figure_from_field(); }
    }

};

int main() {
    Tetris tetris;

    tetris.play();

    return 0;
}