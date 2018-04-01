server : src/*.cpp
	g++ -c src/*.cpp -std=c++11
	ar -crv libwebserver.a webserver.o http.o
	rm webserver.o http.o

clean:
	rm libwebserver.a
