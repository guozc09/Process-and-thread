CC = gcc
CXX = g++
CFLAGS += -I./include
CXXFLAGS += -I./include

CFLAGS += -lpthread -lrt

#objects += fork_test.o

#objects += error_plp.o producer_consumer.o

objects += msg_queue.o
#$(CXX) -o main $(objects)
main : $(objects)
	$(CC) -o main $(objects) $(CFLAGS)

.PHONY : clean
clean :
	-rm main $(objects)