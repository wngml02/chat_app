CC = gcc
CFLAGS = -Wall -g

SERVER_SRC = server.c
CLIENT_SRC = client.c
COMMON_SRC = common.c
COMMON_HDR = common.h

SERVER_OBJ = $(SERVER_SRC:.c=.o) $(COMMON_SRC:.c=.o)
CLIENT_OBJ = $(CLIENT_SRC:.c=.o) $(COMMON_SRC:.c=.o)

TARGETS = server client

all: $(TARGETS)

server: $(SERVER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

client: $(CLIENT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(COMMON_HDR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGETS) *.o
