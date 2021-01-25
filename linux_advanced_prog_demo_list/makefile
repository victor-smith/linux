
objs=open.o openat.o openat1.o lseek.o
bins=open openat openat1 lseek

all: ${bins}

open: open.o
	gcc -o $@ $<

openat: openat.o
	gcc -o $@ $<

openat1: openat1.o
	gcc -o $@ $<

lseek: lseek.o
	gcc -o $@ $<

clean:
	-rm -fr ${objs} ${bins}
	-rm -fr *.txt *.log
