CXX=g++ -O2 

all: clean encode run_test

encode: encode.o huffman.o huffman_node.o
	$(CXX) -o encode encode.o huffman.o huffman_node.o

run_test: run_test.o test.o huffman.o huffman_node.o
	$(CXX) -o run_test run_test.o test.o huffman.o huffman_node.o

clean:
	rm -f *.o code1 code2