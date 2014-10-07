#

EXECUTABLE=test-sorts
SOURCES=test_sort.c ts_options.c qsort_ints.c silly_sorts.c

CC=gcc
STD_CFLAGS=-std=c89
NOISY_CFLAGS=-Werror -Wall -Wextra -pedantic
OPTIMIZER_CFLAGS=-ggdb -O0

CFLAGS=$(STD_CFLAGS) $(OPTIMIZER_CFLAGS) $(NOISY_CFLAGS)

LDFLAGS=-pthread

OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

check: all
	./$(EXECUTABLE)
