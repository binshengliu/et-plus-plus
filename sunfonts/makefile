CFLAGS  =   -O

ALL =   bdftovfont 

all:    $(ALL)

bdftovfont:  bdftovfont.o fontutil.o
	$(CC) $(CFLAGS) -o bdftovfont bdftovfont.o fontutil.o ../src/CType.o;
	-@for i in ../fonts/*.bdf; do \
	    echo $$i; \
	    bdftovfont $$i > `basename $$i .bdf`.vf; \
	done

vfont2bdf:  vfont2bdf.o fontutil.o
	$(CC) $(CFLAGS) -o vfont2bdf vfont2bdf.o fontutil.o ../src/CType.o -lpixrect

depend:

clean:  
	rm -f core *.o

clobber:  clean  
	rm -f *.vf bdftovfont vfont2bdf
