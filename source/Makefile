server : src/*.cpp
	g++ -c src/*.cpp src/*/*.cpp -std=c++11 -Wall -W
	ar -crv libwebserver.a webserver.o http.o file_utils.o string_utils.o
	rm webserver.o http.o file_utils.o string_utils.o

clean:
	rm -f libwebserver.a
