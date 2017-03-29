ET_DIR  =   ../..
BMCUT   =   $(ET_DIR)/bin/bmcut

ALLIM   =  directory.im \
	   file.im \
	   et++.im \
	   ascii.im \
	   exec.im

all:    $(ALLIM)
	

file.im:    file.image
	$(BMCUT) file.image 16 16 > $@

ascii.im:    ascii.image
	$(BMCUT) ascii.image 16 16 > $@

directory.im:    directory.image
	$(BMCUT) directory.image 16 16 > $@
	
et++.im:    et++.image
	$(BMCUT) et++.image 16 16 > $@

exec.im:    exec.image
	$(BMCUT) exec.image 16 16 > $@

clean:
	
	
clobber:
	
