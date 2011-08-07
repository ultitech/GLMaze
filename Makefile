OBJ = main.o maze.o mesh.o camera.o math.o walker.o
LIB = -lGL -lGLU -lSDL -lMatrixLib
FLAGS = -g

GLMaze: $(OBJ)
	gcc $(FLAGS) -o $@ $(OBJ) $(LIB)

%.o: %.c
	gcc $(FLAGS) -c $<
