all: fget fget-server

fget: fget.c
	gcc fget.c -o fget

fget-server: fget-server.c
	gcc fget-server.c -o fget-server