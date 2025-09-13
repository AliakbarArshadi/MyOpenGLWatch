CXX = g++
CXXFLAGS = -std=c++17 -g

INCLUDES = -I./Dependencies

LDFLAGS = -lglfw -lGLEW -lGL -lassimp

EXEC = watch

SRCS = main.cpp Shader.cpp Mesh.cpp Model.cpp

all: $(EXEC)

$(EXEC): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(EXEC) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(EXEC) 