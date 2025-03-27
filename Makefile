all:
	gcc -I src/include -L src/lib -o main main.cpp -lmigw32 -lSDL3main -lSDL3
