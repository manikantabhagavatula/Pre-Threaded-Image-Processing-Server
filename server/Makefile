CC = g++
CFLAGS = -fpermissive -O2 -Wall -I . `pkg-config --cflags opencv`


LIB = -lpthread `pkg-config --libs opencv`

all: server thread_priority

server: server.cpp csapp.o
	$(CC) $(CFLAGS) -o server server.cpp echo.cpp process.cpp csapp.o $(LIB)

thread_priority: priority.cpp csapp.o
	$(CC) $(CFLAGS) -o thread_priority priority.cpp echo.cpp process.cpp csapp.o $(LIB)

csapp.o: csapp.cpp
	$(CC) $(CFLAGS) -c csapp.cpp

clean:
	rm -rf server csapp.o thread_priority
