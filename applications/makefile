TEST    =   hello

DIRS    =   \
	    app \
	    calculator \
	    color \
	    dialog \
	    draw \
	    desktop \
	    er \
	    filebrowser \
	    filebrowserII \
	    hello \
	    layout \
	    micky \
	    miniedit \
	    multiundo \
	    polydocapp \
	    threeshapes \
	    tree \
	    trofftool \
	    twoshapes \
	    twoviews \
	    typescript \
	    vobedit

all:
	(cd $(TEST); $(MAKE) $(TEST))
	
clean clobber depend::
	-@for i in $(DIRS); do (cd $$i; $(MAKE) $@); done

rmcore:
	rm -f */core

