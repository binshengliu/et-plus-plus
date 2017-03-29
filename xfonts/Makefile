# for X11R4:

all:    
	-@for i in ../fonts/*.bdf; do \
	    echo $$i; \
	    bdftosnf $$i >> `basename $$i .bdf`.snf ; \
	done
	-mkfontdir
#        -xset +fp `pwd`

depend:

clean:  
	rm -f core *.o

clobber:  clean  
	rm -f *.snf

