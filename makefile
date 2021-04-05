all:
	gcc main.c li.c -oapp -std=c11 -Wall -Wno-unused-variable -Wno-unused-function -DDEBUG 