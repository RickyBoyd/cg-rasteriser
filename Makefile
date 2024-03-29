# BOOST_ROOT on snowy: /home/crypto/linux.x86_64/boost_1_62_0/include/

PROGRAM = rasteriser

S_DIR=src
B_DIR=build

EXEC=$(B_DIR)/$(FILE)

# default build settings
CC_OPTS=-c -pipe -Wall -Wno-switch -ggdb -g3 -O3 -std=c++11
LN_OPTS=-L $(BOOST_ROOT)/lib -lboost_system -lboost_filesystem
CC=g++

SDL_CFLAGS := $(shell sdl-config --cflags)
GLM_CFLAGS := -I$(GLMDIR)
BOOST_CFLAGS := -I$(BOOST_ROOT)
SDL_LDFLAGS := $(shell sdl-config --libs)
LDFLAGS :=


SOURCES = $(wildcard $(S_DIR)/*.cpp)
OBJECTS = $(patsubst $(S_DIR)/%.cpp,$(B_DIR)/%.o,$(SOURCES))

all: dir $(B_DIR)/$(PROGRAM)
Build: all

dir:
	mkdir -p $(B_DIR)

$(B_DIR)/$(PROGRAM): $(OBJECTS)
	$(CC) -I$(S_DIR)/ $^ -o $@ $(LN_OPTS) $(SDL_LDFLAGS) $(LDFLAGS)

$(OBJECTS): $(B_DIR)/%.o : $(S_DIR)/%.cpp
	$(CC) $(CC_OPTS) $(CFLAGS) $(SDL_CFLAGS) $(GLM_CFLAGS) $(BOOST_CFLAGS) $< -o $@

clean:
	rm -f $(B_DIR)/*
