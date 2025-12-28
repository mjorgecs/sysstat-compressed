CFLAGS=-Wall -Wextra -std=c99 -I sysstat-repo/ sysstat-repo/activity.c

all: read_file

run:
	gcc $(CFLAGS) read_file.c -o read_file -lm

clean:
	rm read_file