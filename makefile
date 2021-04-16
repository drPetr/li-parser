all:
	gcc main.c liio.c listr.c liutil.c limem.c li.c -O0 -oapp -std=c11 -Wall -Wno-unused-variable -Wno-unused-function -DDEBUG