#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
# Mac OS X:
#   brew install glfw
# MSYS2:
#   pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw
#

#CXX = g++
#CXX = clang++


INCLUDE = -I../src -I../src/external `pkg-config --cflags glfw3`
LDFLAGS = `pkg-config --static --libs glfw3`
SOURCES = 	../src/external/gl3w/*.c \
			../src/glgui/core/*.cpp \
			../src/glgui/view/*.cpp \
			../src/glgui/frame/*.cpp \
			../src/glgui/component/*.cpp \
			../src/slam/environment/*.cpp \
			../src/slam/lidar/*.cpp \
			../src/slam/math/*.cpp \
			../src/slam/scan_matching/*.cpp \
			../src/external/imgui/*.cpp \

UNAME := $(shell uname -s)
ifeq ($(UNAME), Linux)
	LDFLAGS += -lGL
endif
ifeq ($(UNAME), Darwin)
	LDFLAGS += -framework OpenGL
endif

glgui: 
	$(CXX) -O3 -DNDEBUG -Wall -Wformat -std=c++17 $(INCLUDE) main.cpp $(SOURCES) $(LDFLAGS) -o slam2d
