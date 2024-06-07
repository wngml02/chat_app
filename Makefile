CC = gcc
CFLAGS = -Wall -g

COMMON_SRC = common.c
USER1_SRC = user1.c
USER2_SRC = user2.c
SERVER_SRC = server.c

COMMON_OBJ = $(COMMON_SRC:.c=.o)
USER1_OBJ = $(USER1_SRC:.c=.o) $(COMMON_SRC:.c=.o)
USER2_OBJ = $(USER2_SRC:.c=.o) $(COMMON_SRC:.c=.o)
SERVER_OBJ = $(SERVER_SRC:.c=.o) $(COMMON_SRC:.c=.o)

TARGETS = user1 user2 server

all: $(TARGETS)

user1: $(USER1_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

user2: $(USER2_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

server: $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
