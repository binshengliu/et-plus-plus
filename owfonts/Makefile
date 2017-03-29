# for Sun's Merged X11/NeWS server:

# after generating the fonts set the fontpath with: xset +fp `pwd`

all:    
	-@for i in ../fonts/*.bdf; do \
	    convertfont -f 9 -o `basename $$i .bdf` $$i ; \
	done
	-bldfamily
#        -xset +fp `pwd`

depend:

clean:  
	rm -f core *.o

clobber:  clean  
	rm -f *.snf *.fb *.ff *.list 

