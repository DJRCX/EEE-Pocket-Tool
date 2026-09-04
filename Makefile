CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -O2
LDFLAGS = -lm
TARGET  = eee_pocket_tool

.PHONY: all clean demo test

all: $(TARGET)

$(TARGET): eee_pocket_tool.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

demo: $(TARGET)
	./scripts/run_demo.sh

test: $(TARGET)
	./scripts/run_tests.sh
