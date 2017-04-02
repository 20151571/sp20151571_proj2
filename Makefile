CC = gcc
CFLAGS = -Wall
SRCS = function.c 20151571.c sp_proj1.c assemble.c
OBJECTS=$(SRCS:.c=.o)

TARGET = 20151571.out

all: $(SRCS) $(TARGET)
	
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm $(OBJECTS) $(TARGET) *.obj *.lst

