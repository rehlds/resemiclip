HLSDK = cssdk
METAMOD = metamod
M_INCLUDE = include

NAME = resemiclip

COMPILER = /opt/intel/bin/icpc

OBJECTS = src/precompiled.cpp src/h_export.cpp src/gamedll_api.cpp src/engine_rehlds_api.cpp \
		src/meta_api.cpp src/main.cpp src/config.cpp cssdk/public/interface.cpp

LINK = -static-intel -static-libgcc -no-intel-extensions

OPT_FLAGS = -O3 -msse3 -ipo -no-prec-div -fp-model fast=2 -funroll-loops -fomit-frame-pointer -fno-stack-protector

INCLUDE = -I. -I$(M_INCLUDE)/ -I$(HLSDK)/common -I$(HLSDK)/dlls -I$(HLSDK)/engine \
		-I$(HLSDK)/game_shared -I$(HLSDK)/pm_shared -I$(HLSDK)/public -I$(METAMOD)

BIN_DIR = Release
CFLAGS = $(OPT_FLAGS)

CFLAGS += -g -DNDEBUG -Dlinux -D__linux__ -D__USE_GNU -D_vsnprintf=vsnprintf -std=c++0x -shared -wd147,274 -fasm-blocks -m32

OBJ_LINUX := $(OBJECTS:%.c=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.c
	$(COMPILER) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)

	$(MAKE) $(NAME) && strip -x $(BIN_DIR)/$(NAME)_mm_i386.so

$(NAME): $(OBJ_LINUX)
	$(COMPILER) $(INCLUDE) $(CFLAGS) $(OBJ_LINUX) $(LINK) -o$(BIN_DIR)/$(NAME)_mm_i386.so

check:
	cppcheck $(INCLUDE) --quiet --max-configs=100 -D__linux__ -DNDEBUG .

debug:
	$(MAKE) all DEBUG=false

default: all

clean:
	rm -rf Release/*.o
	rm -rf Release/$(NAME)_mm_i386.so
