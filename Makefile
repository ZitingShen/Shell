CC = gcc
CFLAGS = -g -Wall
TARGET1 = shell
TARGET2 = tester

all: $(TARGET1) $(TARGET2) sleeper

$(TARGET1) : $(TARGET1).c  parsecmd.o
	$(CC) $(CFLAGS) -o $(TARGET1) $(TARGET1).c parsecmd.o

$(TARGET2) : $(TARGET2).c  parsecmd.h parsecmd.o
	$(CC) $(CFLAGS) -o $(TARGET2) $(TARGET2).c parsecmd.o

sleeper: sleeper.c
	$(CC) $(CFLAGS) -o sleeper sleeper.c

# this is the command to build the parsecmd library as a .o file
parsecmd.o: parsecmd.h parsecmd.c
	$(CC) $(CFLAGS) -c parsecmd.c

clean:
	$(RM) $(TARGET1) sleeper $(TARGET2) parsecmd.o
