CC = g++
CFLAGS = --std=c++17 -Wall -Wextra -Werror
LIBS = -lgtest -lgtest_main -lpthread

TEST_SRC = $(wildcard test/*.cpp)
TEST_OBJ = $(TEST_SRC:.cpp=.o)

LIB_SRC = $(wildcard game/*.cpp) $(wildcard game/help/*.cpp)
LIB_OBJ = $(LIB_SRC:.cpp=.o)

TEST = gtest
GAME = tetris

all: tetris

tetris: $(LIB_OBJ) main.o
	@$(CC) $(CFLAGS) $(LIB_OBJ) main.o -o $(GAME)
	@./$(GAME) || true

test: $(TEST_OBJ) $(LIB_OBJ)
	@$(CC) $(CFLAGS) $(TEST_OBJ) $(LIB_OBJ) -o $(TEST) $(LIBS)
	@./$(TEST) || true

%.o: %.cpp
	@$(CC) $(CFLAGS) -c $< -o $@

valgrind: gtest
	valgrind --tool=memcheck --leak-check=yes --leak-check=full --show-leak-kinds=all ./test > /dev/null

clean:
	@rm -rf $(TEST_OBJ) $(LIB_OBJ) $(LIBRARY) $(TEST) main.o