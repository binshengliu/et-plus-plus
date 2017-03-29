all:    install

bmcut:  bmcut.o
	cc -o bmcut bmcut.o -lpixrect -lsunwindow -lsuntool

install:    bmcut
	cp bmcut ../../bin

clean:
	rm -f *.o core
	
clobber: clean
	rm -f bmcut ../../bin/bmcut

