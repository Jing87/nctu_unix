# the compiler: gcc for C program, define as g++ for C++
CC = gcc
CPP = g++
# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall 
LDFLAGS = -ldl
  # the build target executable:
TARGET = logger logger.so

all: $(TARGET)

logger: hw2.cpp
	$(CPP) $(CFLAGS) $< -o $@

logger.so: hw2_so.c
	$(CC) -shared -o $@ -fPIC $< $(LDFLAGS)
clean:
	$(RM) $(TARGET)