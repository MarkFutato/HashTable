CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = chash

SRCS = chash.c hash.c rwlock.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(TARGET).exe hash.log