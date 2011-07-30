OBJ = main.o maze.o mesh.o camera.o
LIB = -lGL -lGLU -lSDL -lMatrixLib
FLAGS = -g

GLMaze: $(OBJ)
	gcc $(FLAGS) -o $@ $(OBJ) $(LIB)

%.o: %.c
	gcc -c $<
