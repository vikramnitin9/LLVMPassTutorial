SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

.PHONY: all
all: clean out

out: $(OBJS)
	gcc $^ -o $@

%.o: %.c
	gcc -g -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJS) out