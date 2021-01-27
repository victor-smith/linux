CPPFLAGS += -DDEBUG
objs=conf_util.o heartbeat.o socket.o
all: heartbeat

heartbeat: ${objs}
	gcc -o $@ $? -lpthread 

clean:
	-rm ${objs} -fr
	-rm heartbeat -fr
