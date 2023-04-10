all: fget fget-server

fget: fget.c
	gcc fget.c config.c -o fget

fget-server: fget-server.c
	gcc fget-server.c config.c -o fget-server

.PHONY: clean

clean:
	rm fget-server fget