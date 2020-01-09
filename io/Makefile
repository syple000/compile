SUB_DIRS=$(shell ls -l | grep ^d | awk '{print $$9}')
CUR_OBJS=$(patsubst %.cc, %.o, $(wildcard *.cc))
START=start

.PHONY: all

all: $(SUB_DIRS) $(CUR_OBJS)

$(SUB_DIRS): $(START)
	make -C $@

$(START):

%.o: %.cc %.h
	$(CC) -c -g $< -o $(ROOT_DIR)/$(OBJS_DIR)/$@