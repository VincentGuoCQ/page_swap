CC=gcc

all:double_bit

double_bit: double_bit.c page.h list.h
	$(CC) -lpthread -o double_bit double_bit.c
