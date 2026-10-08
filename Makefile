CC = cc
CFLAGS = -Wall -Wextra -std=c99
CPPFLAGS = -Iinclude

TARGET = ls

SRC = src/main.c \
      src/options.c \
      src/directory.c \
      src/fileinfo.c \
      src/sorting.c

all:
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
