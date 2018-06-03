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


INCLUDE = -I../src `pkg-config --cflags glfw3`
LDFLAGS = -lGL `pkg-config --static --libs glfw3`
SOURCES = 	../src/external/gl3w/*.c \
			../src/glgui/core/*.cpp \
			../src/glgui/view/*.cpp \
			../src/glgui/frame/*.cpp \
			../src/glgui/header/*.cpp \
			../src/slam/environment/*.cpp \
			../src/slam/lidar/*.cpp \
			../src/external/imgui/*.cpp \

glgui: 
	$(CXX) $(INCLUDE) main.cpp $(SOURCES) $(LDFLAGS) -o slam2d -O3 -DNDEBUG -Wall -Wformat
