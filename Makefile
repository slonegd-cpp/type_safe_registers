all: submodule
	g++ -std=c++17 -I./value_based_meta main.cpp

submodule:
	git submodule update --init
	cd value_based_meta/ && git fetch
	cd value_based_meta/ && git checkout master
	cd value_based_meta/ && git submodule update --init