CC=gcc
CFLAGS= -Wall -g

SOURCES=$(wildcard *.c)
TASKS=  $(patsubst %.c, task_% , $(SOURCES))

tasks: $(TASKS)

task_%: %.c
	$(CC) $(CFLAGS) -o $@ $^ -lpthread


clean:
	rm -f $(TASKS)

