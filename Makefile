OBJ = main.o maze.o mesh.o camera.o math.o walker.o texture.o
LIB = -lGL -lGLU -lIL -lILU -lSDL -lMatrixLib
FLAGS = -g

GLMaze: $(OBJ)
	gcc $(FLAGS) -o $@ $(OBJ) $(LIB)

%.o: %.c
	gcc $(FLAGS) -c $<
