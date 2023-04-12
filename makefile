all: fget fget-server heartbeat

fget: fget.c
	gcc fget.c config.c -o fget

fget-server: fget-server.c
	gcc fget-server.c config.c -o fget-server

heartbeat: heartbeat.c
	gcc heartbeat.c config.c -o heartbeat

.PHONY: clean

clean:
	rm fget-server fget heartbeat