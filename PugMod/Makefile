CSSDK 	= include/cssdk
METAMOD = include/metamod
LIBCURL = include/curl

NAME = pugmod

COMPILER = g++

OBJECTS = *.cpp

LINK = -static-libstdc++ -lcurl -lssl -lcrypto -ldl -lm -lz

OPT_FLAGS = -g0 -O3 -s -flto -funroll-loops -fno-stack-protector

INCLUDE = -I. -I$(CSSDK)/common -I$(CSSDK)/dlls -I$(CSSDK)/engine -I$(CSSDK)/game_shared -I$(CSSDK)/pm_shared -I$(CSSDK)/public -I$(METAMOD) -I$(LIBCURL)/include

CFLAGS = $(OPT_FLAGS) -g -DNDEBUG -Dlinux -D__linux__ -D__USE_GNU -U_FORTIFY_SOURCE -std=gnu++14 -m32 -msse -msse2 -msse3 -mssse3 -mmmx -shared

BIN_DIR = Release

OBJ_LINUX := $(OBJECTS:%.c=$(BIN_DIR)/%.o)

$(BIN_DIR)/%.o: %.c

	$(COMPILER) $(INCLUDE) $(CFLAGS) -o $@ -c $<

all:
	mkdir -p $(BIN_DIR)

	$(MAKE) $(NAME)

$(NAME): $(OBJ_LINUX)

	$(COMPILER) $(INCLUDE) $(CFLAGS) $(OBJ_LINUX) $(LINK) -o$(BIN_DIR)/$(NAME)_mm.so

check:
	cppcheck $(INCLUDE) --quiet --max-configs=100 --std=gnu++14 -DNDEBUG -Dlinux -D__linux__ -D__USE_GNU .

default: all

clean:
	rm -rf Release/*.o
	rm -rf Release/*.so
