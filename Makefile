CC = gcc
CXX = g++
CFLAGS += -I./include
CXXFLAGS += -I./include

objects = fork_test.o

main : $(objects)
	$(CXX) -o main $(objects)

.PHONY : clean
clean :
	-rm main $(objects)