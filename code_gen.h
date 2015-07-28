/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include "symbol_table.h"
#include "semantic_check.h"

enum _registers {
	AX, BX, CX, DX,	noReg	// noReg logic migh have to be changed in code_gen.c
};
typedef enum _registers Register;

char reg_name[5][5] = {"AX", "BX", "CX", "DX", "None"};

struct _regVars {
	Register reg;	// enum register
	int vars[5];	// uids of variables stored in that register 
	int isDirty;
	int num_vars;
};
typedef struct _regVars register_uids;

extern register_uids vars_in_register[5];

 