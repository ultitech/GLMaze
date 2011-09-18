TARGET = GLMaze
OBJ = main.o maze.o mesh.o camera.o walker.o drawer.o
LIB = -lGL -lGLU -lIL -lILU -lSDL -lMathLib -lMatrixLib -lShaderLib
FLAGS = -g

$(TARGET): $(OBJ)
	gcc $(FLAGS) -o $@ $(OBJ) $(LIB)

clean:
	rm $(OBJ) $(TARGET)

%.o: %.c
	gcc $(FLAGS) -c $<
