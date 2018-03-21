server : main.cpp src/*
	g++ main.cpp src/*.cpp -lboost_system -std=c++11

clean:
	rm a.out
