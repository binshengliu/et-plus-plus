ET_DIR  =   ../../..
BMCUT   =   $(ET_DIR)/bin/bmcut

ALLIM   =  FuncImage.im \
	   FuncImageInv.im


all:    $(ALLIM)
	

FuncImage.im:    FuncImage.image
	$(BMCUT) FuncImage.FuncImage 16 16 > $@

FuncImageInv.im:    FuncImageInv.image
	$(BMCUT) FuncImageInv.FuncImage 16 16 > $@

clean:
	rm -f core
	
clobber:    clean
	
