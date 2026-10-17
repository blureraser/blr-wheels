CC := gcc
CFLAGS := -std=c17 -O2 -Wall -Wextra $(shell pkg-config --cflags raylib)
LDLIBS := $(shell pkg-config --libs raylib)
TARGET := build/main

$(TARGET): main.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
