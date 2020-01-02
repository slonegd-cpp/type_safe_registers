all: submodule
	g++ -std=c++17 -I./value_based_meta main.cpp

submodule:
	git submodule update --init
	cd value_based_meta/ && git checkout master
	cd value_based_meta/ && git pull origin master
	cd value_based_meta/ && git submodule update --init

clean:
	rm -fR ./value_based_meta