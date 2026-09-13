TARGET = ddnn.exe

CXX = g++

INCLUDES = -Iinclude/raylib -Isrc
LIBRARIES = -Llib/windows

SOURCES = src/header.h src/algo.h src/physics.h src/graphics.h src/main.cpp

CXXFLAGS = -Wall -O3
LDFLAGS = -static -lraylib -lopengl32 -lgdi32 -lwinmm

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) -g $(SOURCES) -o $(TARGET) $(INCLUDES) $(LIBRARIES) $(CXXFLAGS) $(LDFLAGS)

clean:
	del /q src\ddnn.exe