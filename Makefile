CC = gcc
CFLAGS = -Wall -Wextra
TARGET = t
SRC = t.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)