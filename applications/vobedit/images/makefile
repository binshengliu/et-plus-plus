BMCUT   =   ../../../bin/bmcut
ALLIM   =   AnnotatedItem.im \
	    AnnotatedItemFeedback.im \
	    sun.im


all:    $(ALLIM)
	

AnnotatedItem.im:    AnnotatedItem.image
	$(BMCUT) AnnotatedItem.image 20 16 > $@

AnnotatedItemFeedback.im:    AnnotatedItemFeedback.image
	$(BMCUT) AnnotatedItemFeedback.image 20 16 > $@

sun.im:    sun.image
	$(BMCUT) sun.image 23 23 > $@

clean:
	rm -f core
	
clobber:    clean
	
