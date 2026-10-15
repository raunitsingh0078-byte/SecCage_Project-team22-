CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -O2 -Iinclude
LDLIBS := -lseccomp
SOURCES := src/main.c src/sandbox.c src/logger.c src/report.c
TARGET := seccage

.PHONY: all clean tests
all: $(TARGET) tests/allowed_test tests/network_test

$(TARGET): $(SOURCES) include/sandbox.h include/logger.h include/report.h
	$(CC) $(CFLAGS) $(SOURCES) -o $@ $(LDLIBS)

tests/allowed_test: tests/allowed_test.c
	$(CC) $(CFLAGS) $< -o $@

tests/network_test: tests/network_test.c
	$(CC) $(CFLAGS) $< -o $@

tests: all

clean:
	rm -f $(TARGET) tests/allowed_test tests/network_test
