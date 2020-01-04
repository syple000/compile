SUB_DIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_SOURCES=$(wildcard *.cc)
CUR_OBJS=$(patsubst %.cc, %.o, $(CUR_SOURCES))
START=start

.PHONY: all

all: $(SUB_DIRS) $(CUR_OBJS)

$(SUB_DIRS): $(START)
	make -C $@

$(START):

$(CUR_OBJS):$(CUR_SOURCES)
	$(CC) -c -g $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@
