server : src/*.cpp
	g++ -c src/*.cpp src/*/*.cpp -std=c++11 -Wall -W
	ar -crv libwebserver.a *.o

clean:
	rm -f *.o *.a
