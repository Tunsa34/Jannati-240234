#**************************************************************************************************
#
#   raylib makefile for Desktop platforms, Raspberry Pi, Android and HTML5
#
#   Copyright (c) 2013-2019 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

.PHONY: all clean

# Define required raylib variables
PROJECT_NAME       ?= minesweeper
RAYLIB_VERSION     ?= 5.1-dev
RAYLIB_PATH        ?= ..\..

# Define compiler path on Windows
COMPILER_PATH      ?= C:/raylib/w64devkit/bin

# Define default options
# One of PLATFORM_DESKTOP, PLATFORM_ANDROID, PLATFORM_WEB
PLATFORM           ?= PLATFORM_DESKTOP

# Locations of your newly installed library and associated headers. See ../src/Makefile
DESTDIR ?= /usr/local
RAYLIB_INSTALL_PATH ?= $(DESTDIR)/lib
RAYLIB_H_INSTALL_PATH ?= $(DESTDIR)/include

# Library type used for raylib: STATIC (.a) or SHARED (.so/.dll)
RAYLIB_LIBTYPE        ?= STATIC

# Build mode for project: DEBUG or RELEASE
BUILD_MODE            ?= RELEASE

# Use external GLFW library instead of rglfw module
USE_EXTERNAL_GLFW     ?= FALSE

# Use Wayland display server protocol on Linux desktop
USE_WAYLAND_DISPLAY   ?= FALSE

# ---- Updated source and output directories ----
SRC_DIR   = src
BUILD_DIR = build

# Determine PLATFORM_OS in case PLATFORM_DESKTOP selected
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(OS),Windows_NT)
        PLATFORM_OS=WINDOWS
        export PATH := $(COMPILER_PATH):$(PATH)
    else
        UNAMEOS=$(shell uname)
        ifeq ($(UNAMEOS),Linux)
            PLATFORM_OS=LINUX
        endif
        ifeq ($(UNAMEOS),FreeBSD)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),OpenBSD)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),NetBSD)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),DragonFly)
            PLATFORM_OS=BSD
        endif
        ifeq ($(UNAMEOS),Darwin)
            PLATFORM_OS=OSX
        endif
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    UNAMEOS=$(shell uname)
    ifeq ($(UNAMEOS),Linux)
        PLATFORM_OS=LINUX
    endif
endif

# RAYLIB_PATH adjustment for different platforms.
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),LINUX)
        RAYLIB_PREFIX ?= ..
        RAYLIB_PATH    = $(realpath $(RAYLIB_PREFIX))
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    RAYLIB_PATH       ?= /home/pi/raylib
endif

ifeq ($(PLATFORM),PLATFORM_WEB)
    EMSDK_PATH         ?= C:/raylib/emsdk
    EMSCRIPTEN_PATH    ?= $(EMSDK_PATH)/upstream/emscripten
    CLANG_PATH          = $(EMSDK_PATH)/upstream/bin
    PYTHON_PATH         = $(EMSDK_PATH)/python/3.9.2-nuget_64bit
    NODE_PATH           = $(EMSDK_PATH)/node/20.18.0_64bit/bin
    export PATH         = $(EMSDK_PATH);$(EMSCRIPTEN_PATH);$(CLANG_PATH);$(NODE_PATH);$(PYTHON_PATH):$$(PATH)
endif

RAYLIB_RELEASE_PATH 	?= $(RAYLIB_PATH)/src
EXAMPLE_RUNTIME_PATH   ?= $(RAYLIB_RELEASE_PATH)

# Define default C++ compiler
CC = g++

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),OSX)
        CC = clang++
    endif
    ifeq ($(PLATFORM_OS),BSD)
        CC = clang++
    endif
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    CC = emcc
endif

# Define default make program
MAKE = mingw32-make

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),LINUX)
        MAKE = make
    endif
    ifeq ($(PLATFORM_OS),OSX)
        MAKE = make
    endif
endif

# Define compiler flags
CFLAGS += -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -s -O1
endif

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        CFLAGS += $(RAYLIB_PATH)/src/raylib.rc.data -Wl,--subsystem,windows
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        ifeq ($(RAYLIB_LIBTYPE),STATIC)
            CFLAGS += -D_DEFAULT_SOURCE
        endif
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            CFLAGS += -Wl,-rpath,$(EXAMPLE_RUNTIME_PATH)
        endif
    endif
endif

# Define include paths
INCLUDE_PATHS = -I. -I$(SRC_DIR) -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),BSD)
        INCLUDE_PATHS += -I/usr/local/include
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        INCLUDE_PATHS = -I$(RAYLIB_H_INSTALL_PATH) -isystem. -isystem$(SRC_DIR) -isystem$(RAYLIB_PATH)/src -isystem$(RAYLIB_PATH)/release/include -isystem$(RAYLIB_PATH)/src/external
    endif
endif

# Define library paths
LDFLAGS = -L. -L$(RAYLIB_RELEASE_PATH) -L$(RAYLIB_PATH)/src

ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),BSD)
        LDFLAGS += -L. -L$(SRC_DIR) -L/usr/local/lib
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        LDFLAGS = -L. -L$(RAYLIB_INSTALL_PATH) -L$(RAYLIB_RELEASE_PATH)
    endif
endif

ifeq ($(PLATFORM),PLATFORM_RPI)
    LDFLAGS += -L/opt/vc/lib
endif

# Define libraries to link
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
        LDLIBS = -lraylib -lopengl32 -lgdi32 -lwinmm
    endif
    ifeq ($(PLATFORM_OS),LINUX)
        LDLIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
        ifeq ($(USE_WAYLAND_DISPLAY),TRUE)
            LDLIBS += -lwayland-client -lwayland-cursor -lwayland-egl -lxkbcommon
        endif
        ifeq ($(RAYLIB_LIBTYPE),SHARED)
            LDLIBS += -lc
        endif
    endif
    ifeq ($(PLATFORM_OS),OSX)
        LDLIBS = -lraylib -framework OpenGL -framework OpenAL -framework Cocoa -framework IOKit
    endif
    ifeq ($(PLATFORM_OS),BSD)
        LDLIBS = -lraylib -lGL -lpthread -lm -lX11 -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor
    endif
    ifeq ($(USE_EXTERNAL_GLFW),TRUE)
        LDLIBS += -lglfw
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
    LDLIBS = -lraylib -lbrcmGLESv2 -lbrcmEGL -lpthread -lrt -lm -lbcm_host -ldl
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
    LDLIBS = $(RAYLIB_RELEASE_PATH)/libraylib.a
endif

# ---- Source files (all in src/) ----
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/Game.cpp $(SRC_DIR)/Board.cpp

# For Android platform we call a custom Makefile.Android
ifeq ($(PLATFORM),PLATFORM_ANDROID)
    MAKEFILE_PARAMS = -f Makefile.Android
    export PROJECT_NAME
    export SRC_DIR
else
    MAKEFILE_PARAMS = $(PROJECT_NAME)
endif

# Default target
all: $(BUILD_DIR) $(MAKEFILE_PARAMS)

# Ensure build directory exists
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build the project directly from source files
$(PROJECT_NAME): $(SOURCES)
ifeq ($(PLATFORM_OS),WINDOWS)
	$(CC) -o $(BUILD_DIR)/$(PROJECT_NAME).exe $(SOURCES) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)
else
	$(CC) -o $(BUILD_DIR)/$(PROJECT_NAME) $(SOURCES) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)
endif

# Clean build output only
clean:
ifeq ($(PLATFORM),PLATFORM_DESKTOP)
    ifeq ($(PLATFORM_OS),WINDOWS)
		del /Q /F build\*.exe build\*.o 2>nul
    endif
    ifeq ($(PLATFORM_OS),LINUX)
		rm -f $(BUILD_DIR)/$(PROJECT_NAME) $(BUILD_DIR)/*.o
    endif
    ifeq ($(PLATFORM_OS),OSX)
		rm -f $(BUILD_DIR)/$(PROJECT_NAME) $(BUILD_DIR)/*.o
    endif
endif
ifeq ($(PLATFORM),PLATFORM_RPI)
	rm -f $(BUILD_DIR)/$(PROJECT_NAME) $(BUILD_DIR)/*.o
endif
ifeq ($(PLATFORM),PLATFORM_WEB)
	del $(BUILD_DIR)\*.o $(BUILD_DIR)\*.html $(BUILD_DIR)\*.js 2>nul
endif
	@echo Cleaning done
