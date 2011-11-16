TARGET = GLMaze
OBJ = main.o scene.o maze.o mesh.o camera.o walker.o drawer.o noise.o file.o window.o config.o
LIB = -lGL -lGLU -lIL -lSDL -lMathLib -lGLEW
FLAGS = -g

$(TARGET): $(OBJ)
	gcc $(FLAGS) -o $@ $(OBJ) $(LIB)

clean:
	rm $(OBJ) $(TARGET)

%.o: %.c
	gcc $(FLAGS) -c $<
