/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include "ast.h"
#define BUCKET_PRIME 17

struct sym_table_node;
typedef struct sym_table_node *SYM_NODE;

//char typeStr[][20] = {"bot", "point", "dataType", "function", "variable", "constant_value", "op", "assign", "conditional", "loop", "botOps", "io", "stmts", "ret", "params", "brk", "cnt", "PROGRAM"};

/******************************   STRUCTURES FOR VARIABLES *********************************/
struct var_attributes;
typedef struct var_attributes *VAR_ATTR;
struct var_attributes {
	// ENTRY AT INDEX UID  ->	UNIQUE ID OF IDENTIFIER
	int offset;				// BASE ADDRESS
	int line_decl;			// LINE NO. WHERE VARIABLE HAS BEEN DECLARED
	int reg;
	int *line_ref;			// ARRAY OF LINE NUMBERS WHERE IT HAS BEEN REFERENCED
	int *modRef;			// 0 => REFERENCED, 1 => MODIFIED 
	int line_ref_num;
	int size1;				// 0 ==> variable, >0 ==> array
	int size2;				// 0 ==> variable, >0 ==> array
};
/* NOTE:
	There is a separate attribute table storing attributes for each variable against its uid (primary key)
*/

struct hashtbl_entry_var;
typedef struct hashtbl_entry_var *HASH_VAR;
struct hashtbl_entry_var {
	int hash;				// hash of character value
	char *id;				// name of the variable
	int type;				// integer of floating ONLY			
	int uid;				// UNIQUE ID OF IDENTIFIER 		=> 		PRIMARY KEY
	/*NOTE:
		all bot and point declarations have already been resolved
		all boolean declarations have been resolved.
		NO BOT / POINT / BOOLEAN ENTRIES IN THE SYMBOL TABLE
	*/
	
	//  ???? DOUBLY ??? LINKED LIST OF HASHTABLE ENTRIES
	// HASH_VAR prev;			// debatable if prev pointer is required or not
	HASH_VAR next;
};

/***********************	END OF STRUCTURES FOR VARIABLES *********************************/


/***********************	STRUCTURES FOR FUNCTION + SCOPE RESOLUTION TABLE *****************/ 
struct _scEntry {
	char *fn_name;
	int hash;
	int no_of_return_vals;
	int *returnValTypes;
	int no_of_params;
	//int *paramTypes;
	AST_NODE ast_node_ptr;
	SYM_NODE sym_node_ptr;
};
typedef struct _scEntry scopeResTableEntry;


/************************	END OF STRUCTURES FOR FUNCTIONS *******************************/

struct sym_table_node {
	int num_children;
	int active_child;
	int depth;

	SYM_NODE parent;
	SYM_NODE *children;

	HASH_VAR *buckets;
};

extern scopeResTableEntry *scopeResTable;

extern int INT_SIZE;
extern int FLOAT_SIZE;

extern VAR_ATTR var_attr_table;