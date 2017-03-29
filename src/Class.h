#ifndef Class_First
#ifdef __GNUG__
#pragma once
#endif
#define Class_First

#include "Object.h"

enum ClassFlags {
    eClassAbstract  =   BIT(eObjLast + 1),
    eClassObject    =   BIT(eObjLast + 2),
    eClassLast      =   eObjLast + 2
};

//---- MetaDef macros ----------------------------------------------------------

#define Meta(name) (((name*)0)->isa)

#define MetaDef(name)                                           \
    name(class _dummy*);                                        \
    static class Class *isa;                                    \
    Class *IsA();                                               \
    friend char *_NAME2_(name,DeclFileName)(char *p= __FILE__)  \
	{ return p; }                                           \
    friend int _NAME2_(name,DeclFileLine)(int i= __LINE__)      \
	{ return i; }                                           \
    friend istream &operator>> (istream &s, name *&op)          \
	{ return LoadPtr(s, (Object*&)op, Meta(name)); }        \
    void Members();                                             \
    friend class Class *_Type(name*)                            \
	{ return Meta(name); }

//---- Class -------------------------------------------------------------------

class Class: public Object {
    static class Class *classptr;
    
    Class *super;
    char *className, *declFileName, *implFileName;
    class ObjArray *instanceTable;
    Object *proto;
    int myId, size, declFileLine, implFileLine, instanceCount;
    class OrdCollection* subclasses;
    class Collection *methods;

friend class ClassManager;
    void Reset();

public:
    MetaDef(Class);
    Class(char *name, int sz= 0, Object *pro= 0, char *in= 0, char *dn= 0,
	    int il= 0, int dl= 0, bool abstract= FALSE, int t= 0);
    ~Class();

    Class *Super() const
	{ return super; }
    Class *SetSuper();
    void AddSubclass(Class*);
    class Iterator *SubclassIterator();
    char *Name() const
	{ return className; }
    int Size() const
	{ return size; }
    Object *Proto() const
	{ return proto; }
    Object *New();
    bool isKindOf(Class*);
    bool IsAbstract()
	{ return TestFlag(eClassAbstract); }
    void SavePtr(ostream&, Object*);
    void InvalidatePtr(Object*);
    int MakeIndex(ObjPtr p, bool *bp= 0);
    Object *LoadPtr(istream&);

    //---- comparing
    u_long Hash();
    bool IsEqual(Object*);
    int Compare(Object*);

    //---- converting
    char* AsString();

    void EnumerateMembers(class AccessMembers *accessor= 0);
    void EnumerateMyMembers(AccessMembers *accessor= 0);
    
    //---- source access
    const char *GetDeclFileName()
	{ return declFileName; }
    const char *GetImplFileName()
	{ return implFileName; }   
    int GetImplFileLine()
	{ return implFileLine; }
    int GetDeclFileLine()
	{ return declFileLine; }

    //---- input/output
    ostream& DisplayOn(ostream &s);
    
    //---- misc
    void InspectorId(char *buf, int bufSize);
    Object *SomeInstance();
    Object *SomeMember();
    virtual void Show(char *buf, void *addr);
    
    //---- statistics
    void AddInstance()
	{ instanceCount++; }
    int  GetInstanceCount()
	{ return instanceCount; }
    void ResetInstanceCount()
	{ instanceCount= 0; }

    //---- methods
    Collection *GetMethods();
    void SetMethods(Collection *col);
};

//---- member access -----------------------------------------------------------

void D_F(int, ...);

//---- MetaImpl macros ---------------------------------------------------------

#define _SimpleMetaImpl(name,printname)                          \
    class _NAME2_(name,Class): public Class {                    \
    public:                                                      \
	_NAME2_(name,Class)() : (_QUOTE_(printname), sizeof(name), 0,\
	    __FILE__, __FILE__, __LINE__,                        \
	    __LINE__, FALSE)                                     \
	    { }                                                  \
	void Show(char *buf, void *addr);                        \
    };                                                           \
    static _NAME2_(name,Class) _NAME2_(name,ClassMetaImpl0);     \
    Class *_NAME2_(__isa__,name)= &_NAME2_(name,ClassMetaImpl0); \
    void _NAME2_(name,Class)::Show(char *buf, void *addr)

#define SimpleMetaImpl(name)      _SimpleMetaImpl(name,name)

#define _MetaImpl0(name,abstract)                                \
    static Class _NAME2_(name,ClassMetaImpl0)(_QUOTE_(name), sizeof(name),  \
    new name((class _dummy*)0),                                     \
    __FILE__,                                                       \
    _NAME2_(name,DeclFileName)(),                                   \
    __LINE__,                                                       \
    _NAME2_(name,DeclFileLine)(),                                   \
    abstract);                                                      \
name::name(class _dummy *d) : (d)                                   \
    { isa= _NAME2_(name,ClassMetaImpl0).SetSuper(); }               \
Class *name::IsA()                                                  \
    { return &_NAME2_(name,ClassMetaImpl0); }

extern void *TheThis; 

#define MetaImpl0(name)     \
_MetaImpl0(name,FALSE)      \
void name::Members() { } 

#define MetaImpl(name,list) \
_MetaImpl0(name,FALSE)      \
void name::Members() { TheThis= this; D_F list; }

#define AbstractMetaImpl0(name) \
_MetaImpl0(name,TRUE)       \
void name::Members() { }

#define AbstractMetaImpl(name,list) \
_MetaImpl0(name,TRUE)       \
void name::Members() { TheThis= this; D_F list; }

//---- type specifiers for metaclass macros ------------------------------------

extern int gDebug;

#define _offset(in) (u_long)&in
#define _aoffset(in) (u_long)in
#define TC(in,cast) 'A', _QUOTE_(in), _Type((cast*)&in),  _offset(in)

#define T(in)       'A', _QUOTE_(in), _Type(&in),  _offset(in)
#define TE(in)      TC(in,int)
#define TB(in)      TC(in,_bool)
#define TX(in)      TC(in,_flags)

/* #define TT(in,en)   'A', _QUOTE_(in), _NAME2_(__isa__,en), _offset(in) */
#define TP(in)      'P', _QUOTE_(in), _Type(in),   _offset(in)
#define TV(in,len)  'B', _QUOTE_(in), _Type(in),   _offset(in),    _offset(len)
#define TA(in,len)  'D', _QUOTE_(in), _Type(in),   _aoffset(in),   len

#ifdef __GNUG__ // Thanks to fcolin@cenaath.cena.dgac.fr
#   define TPP(in)     'p', #in, _Type((typeof(*in))in),  _offset(in)
#   define TVP(in,len) 'b', #in, _Type((typeof(*in))in),  _offset(in), _offset(len)
#   define TAP(in,len) 'd', #in, _Type((typeof(*in))in),_aoffset(in), len
#   define TSS(in)     'D', #in, _Type((typeof(*in))*in),  _offset(in), -1
#else
#   define TPP(in)     'p', _QUOTE_(in), _Type(*in),  _offset(in)
#   define TVP(in,len) 'b', _QUOTE_(in), _Type(*in),  _offset(in), _offset(len)
#   define TAP(in,len) 'd', _QUOTE_(in), _Type(in[0]),_aoffset(in), len
#   define TSS(in)     'D', _QUOTE_(in), _Type(*in),  _offset(in), -1
#endif

#define I_C(in)     T(in)
#define I_S(in)     T(in)
#define I_I(in)     T(in)
#define I_F(in)     T(in)
#define I_D(in)     T(in)
#define I_X(in)     T(in)
#define I_B(in)     T(in)
#define I_P(in)     T(in)
#define I_R(in)     T(in)

#define I_FT(in)    TP(in)
#define I_O(in)     TP(in)
#define I_CS(in)    TP(in)

#define I_CSS(in)   TPP(in)

#define I_CV(in,len)    TV(in, len)
#define I_SV(in,len)    TV(in, len)
#define I_IV(in,len)    TV(in, len)
#define I_FV(in,len)    TV(in, len)
#define I_DV(in,len)    TV(in, len)
#define I_BV(in,len)    TV(in, len)
#define I_PV(in,len)    TV(in, len)
#define I_RV(in,len)    TV(in, len)
#define I_XV(in,len)    TV(in, len)

#define I_CSV(in,len)   TVP(in, len)
#define I_OV(in,len)    TVP(in, len)


#define I_CA(in,len)    TA(in, len)
#define I_SA(in,len)    TA(in, len)
#define I_IA(in,len)    TA(in, len)
#define I_FA(in,len)    TA(in, len)
#define I_DA(in,len)    TA(in, len)
#define I_BA(in,len)    TA(in, len)
#define I_PA(in,len)    TA(in, len)
#define I_RA(in,len)    TA(in, len)
#define I_XA(in,len)    TA(in, len)

#define I_CSA(in,len)   TAP(in, len)
#define I_OA(in,len)    TAP(in, len)

#endif Class_First
