TARGET=cabinet_inspector
SOURCES=cabinet_inspector.c
OBJECTS=$(SOURCES:%.c=%.o)
HEADERS=cabinet.h

UTILS=mmapper foo
UTIL_SRCS=mmapper.c
UTIL_OBJS=$(UTIL_SRCS:%.c=%.o)

CC=gcc

INCLUDE=
CFLAGS=-Wall -g $(INCLUDE)

LDFLAGS=
LDLIBS=

.PHONY: default util all clean foo
default: $(TARGET)

util: mmapper foo

all: default util

mmapper: mmapper.o

foo:
	echo "OS is hard" > foo

%.o: %.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -rf $(TARGET) $(OBJECTS) $(UTILS) $(UTIL_OBJS) .*.swp a.out main
