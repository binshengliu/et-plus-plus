MAKE    = make
CFLAGS  = 

#---- AT&T C++2.0 and SUN C++2.0
CCFLAGS =
CC      = cc
LD      = ld
ETCC    = etCC
 
#---- configuration for g++
#CCFLAGS= -g -fminimal-debug -W 
#CCFLAGS= -O -fstrength-reduce -W 
#CFLAGS= -O -fstrength-reduce -W 
#CC     = /usr/local/bin/cc
#LD     = /usr/local/lib/gcc-ld


DIRS    = util src applications examples sunfonts xfonts owfonts

MFLAGS  = \
	CFLAGS="$(CFLAGS)" \
	CCFLAGS="$(CCFLAGS)" \
	CC="$(CC)" \
	MAKE="$(MAKE)" \
	ETCC="$(ETCC)" \
	LD="$(LD)" 

all depend clean clobber::
	-@for i in $(DIRS); do (cd $$i; $(MAKE) $(MFLAGS) $@); done

