ET_DIR  =   ../../..
BMCUT   =   $(ET_DIR)/bin/bmcut

ALLIM   =  ArcShape.im \
	   BezierShape.im \
	   BoxShape.im \
	   DynShape.im \
	   LineShape.im \
	   OvalShape.im \
	   PolyShape.im \
	   RcBoxShape.im \
	   RegionShape.im \
	   Selection.im \
	   SunLogo.im \
	   TextShape.im


all:    $(ALLIM)
	

ArcShape.im:    ArcShape.image
	$(BMCUT) ArcShape.image 40 28 > $@

LineShape.im:    LineShape.image
	$(BMCUT) LineShape.image 40 28 > $@

Selection.im:    Selection.image
	$(BMCUT) Selection.image 40 28 > $@

BezierShape.im:    BezierShape.image
	$(BMCUT) BezierShape.image 40 28 > $@

OvalShape.im:    OvalShape.image
	$(BMCUT) OvalShape.image 40 28 > $@

SunLogo.im:    SunLogo.image
	$(BMCUT) SunLogo.image 23 23 > $@

BoxShape.im:    BoxShape.image
	$(BMCUT) BoxShape.image 40 28 > $@

PolyShape.im:    PolyShape.image
	$(BMCUT) PolyShape.image 40 28 > $@

TextShape.im:    TextShape.image
	$(BMCUT) TextShape.image 40 28 > $@

RcBoxShape.im:    RcBoxShape.image
	$(BMCUT) RcBoxShape.image 40 28 > $@

DynShape.im:    DynShape.image
	$(BMCUT) DynShape.image 40 28 > $@

RegionShape.im:    RegionShape.image
	$(BMCUT) RegionShape.image 40 28 > $@


clean:
	rm -f core
	
clobber:    clean
	
