
objs=pthread1 mutex1 pthread2-no-mutex join1 cond1
#all: pthread1 pthread2 pthread2-no-mutex join1 cond1
all: ${objs}

pthread1: pthread1.o
	gcc $? -o $@ -lpthread
mutex1: mutex1.o
	gcc $? -o $@ -lpthread
pthread2-no-mutex: pthread2-no-mutex.o
	gcc $? -o $@ -lpthread
join1: join1.o
	gcc $? -o $@ -lpthread
cond1: cond1.o
	gcc $? -o $@ -lpthread
	
clean:
	-rm ${objs} -fr
	-rm *.o -fr
