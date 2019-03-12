server : src/*.cpp
	g++ -c src/*.cpp src/*/*.cpp -std=c++11 -Wall -W
	ar -crv libwebserver.a webserver.o request.o http_common.o file_utils.o string_utils.o

clean:
	rm -f *.o *.a
