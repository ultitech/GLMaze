OBJ = main.o maze.o mesh.o camera.o walker.o drawer.o
LIB = -lGL -lGLU -lIL -lILU -lSDL -lMathLib -lMatrixLib -lShaderLib
FLAGS = -g

GLMaze: $(OBJ)
	gcc $(FLAGS) -o $@ $(OBJ) $(LIB)

%.o: %.c
	gcc $(FLAGS) -c $<
