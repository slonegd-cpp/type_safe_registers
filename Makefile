all: submodule
	g++ -std=c++17 -g -I./type_pack main.cpp

submodule:
	git submodule update --init
	cd type_pack/ && git checkout master
	cd type_pack/ && git pull origin master
	cd type_pack/ && git submodule update --init

clean:
	rm -fR ./type_pack