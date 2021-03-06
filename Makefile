#root文件夹的makefile
CC=g++
GDB=gdb
CFLAG=
TARGET=out
DEBUG_DIR=debug
OBJS_DIR=$(DEBUG_DIR)/obj
BIN_DIR=$(DEBUG_DIR)/bin
VALGRIND=valgrind
VALGRIND_PARAM=--tool=memcheck --leak-check=full
START=start

MKDIR_P=@mkdir -p
SUB_DIRS=$(shell ls -l | grep ^d | awk '{if ($$9 != "'$(DEBUG_DIR)'") print $$9}')
ROOT_DIR=$(shell pwd)
CUR_OBJS=$(patsubst %.cc, %.o, $(wildcard *.cc))

export CC ROOT_DIR OBJS_DIR BIN_DIR TARGET

.PHONY: all run gdb clean

all: $(DEBUG_DIR) $(SUB_DIRS) $(CUR_OBJS) $(TARGET)

$(DEBUG_DIR) $(OBJS_DIR) $(BIN_DIR): $(START)
	$(MKDIR_P) $(DEBUG_DIR)
	$(MKDIR_P) $(OBJS_DIR)
	$(MKDIR_P) $(BIN_DIR)

$(SUB_DIRS): $(START)
	make -C $@

%.o: %.cc
	$(CC) -c -g $< -o $(OBJS_DIR)/$@

$(TARGET): $(DEBUG_DIR)
	make -C $(DEBUG_DIR)

$(START):
	@echo $(CUR_OBJS)

run:
	$(VALGRIND) $(VALGRIND_PARAM) $(ROOT_DIR)/$(BIN_DIR)/$(TARGET)

gdb:
	$(GDB) $(ROOT_DIR)/$(BIN_DIR)/$(TARGET)

clean:
	@-rm -rf $(OBJS_DIR)/*
	@-rm -rf $(BIN_DIR)/*
