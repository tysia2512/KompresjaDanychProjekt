CXX = g++
CXXFLAGS = -Wall -g

main: main.o huffman.o
	$(CXX) $(CXXFLAGS) -o main main.o huffman.o