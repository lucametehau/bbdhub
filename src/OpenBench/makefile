BUILD_FLAGS = -O3 -std=c++20 -march=native -Wall -g -flto
EXE = bbd
SRC = $(wildcard ../*.cpp)
OBJ = $(SRC:.cpp=.o)

%.o: %.cpp
	clang++ $(BUILD_FLAGS) $(RFLAGS) -o $@ -c $<

native: $(OBJ)
	clang++ $(OBJ) $(BUILD_FLAGS) -o $(EXE)