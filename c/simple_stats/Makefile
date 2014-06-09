#

EXECUTABLE=sstats
SOURCES=main.c ss_options.c simple_stats.c

CC=gcc
STD_CFLAGS=--std=c99
NOISY_CFLAGS=-Werror -Wall -Wextra -pedantic
OPTIMIZER_CFLAGS=-ggdb -O0

CFLAGS=$(STD_CFLAGS) $(OPTIMIZER_CFLAGS) $(NOISY_CFLAGS)

# "-lm" needed by simple_stats.c see also: man 3 sqrt
LDFLAGS=-lm

OBJECTS=$(SOURCES:.c=.o)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) -c $(CFLAGS) $^ -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

check:
	cat ./data.txt
	./sstats --file=./data.txt --skip_rows=1 --skip_cols=1 --channels=4
