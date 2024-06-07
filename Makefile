CC = gcc
CFLAGS = -Wall -g

USER1_SRC = user1.c
USER2_SRC = user2.c
COMMON_SRC = common.c
COMMON_HDR = common.h

USER1_OBJ = $(USER1_SRC:.c=.o) $(COMMON_SRC:.c=.o)
USER2_OBJ = $(USER2_SRC:.c=.o) $(COMMON_SRC:.c=.o)

TARGETS = user1 user2

all: $(TARGETS)

user1: $(USER1_OBJ)
    $(CC) $(CFLAGS) -o $@ $^

user2: $(USER2_OBJ)
    $(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(COMMON_HDR)
    $(CC) $(CFLAGS) -c $< -o $@

clean:
    rm -f $(TARGETS) *.o
