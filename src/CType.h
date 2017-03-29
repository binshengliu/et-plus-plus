#ifndef CType_First
#ifdef __GNUG__
#pragma once
#endif
#define CType_First

#define _U_      01
#define _L_      02
#define _N_      04
#define _S_      010
#define _P_      020
#define _C_      040
#define _X_      0100
#define _B_      0200
#define _W_      0400
#define _A_      01000

struct CType {
    unsigned short code;
    char *name;
};

extern struct CType _C_Type_[];

#define Isalpha(c)      ((_C_Type_+1)[c].code & (_U_|_L_))
#define Isupper(c)      ((_C_Type_+1)[c].code & _U_)
#define Islower(c)      ((_C_Type_+1)[c].code & _L_)
#define Isdigit(c)      ((_C_Type_+1)[c].code & _N_)
#define Isxdigit(c)     ((_C_Type_+1)[c].code & _X_)
#define Isspace(c)      ((_C_Type_+1)[c].code & _S_)
#define Ispunct(c)      ((_C_Type_+1)[c].code & _P_)
#define Isalnum(c)      ((_C_Type_+1)[c].code & (_U_|_L_|_N_))
#define Isprint(c)      ((_C_Type_+1)[c].code & (_P_|_U_|_L_|_N_|_B_))
#define Isgraph(c)      ((_C_Type_+1)[c].code & (_P_|_U_|_L_|_N_))
#define Iscntrl(c)      ((_C_Type_+1)[c].code & _C_)
#define Isascii(c)      ((_C_Type_+1)[c].code & _A_)
#define Isinword(c)     ((_C_Type_+1)[c].code & (_N_|_L_|_U_|_W_))

#endif CType_First
