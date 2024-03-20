CC = g++
CFLAGS += -Wall -std=c++23
LDFLAGS +=
OBJDIR = obj/
SRCDIR = src/

all: test

test: obj/main.o
	$(CC) -o test obj/main.o $(LDFLAGS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm obj/*.o -f
	clear
	
.PHONY: clean