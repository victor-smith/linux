CPPFLAGS += -DDEBUG
objs = long_socket_client.o long_socket_server.o long_socket_server_select.o long_socket_server_poll.o

bins = server client sserver pserver

all: server client sserver pserver

server: long_socket_server.o 
	gcc -o $@ $? -lpthread 

client: long_socket_client.o 
	gcc -o $@ $? -lpthread 

sserver: long_socket_server_select.o 
	gcc -o $@ $? -lpthread 

pserver: long_socket_server_poll.o 
	gcc -o $@ $? -lpthread 

clean:
	-rm ${objs} -fr
	-rm ${bins} -fr
