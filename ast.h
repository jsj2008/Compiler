/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include "parser.h"

enum _astNodeType {bot, point, dataType, function, functionCall, variable, 
				  constant_value, op, assign, conditional, 
				  loop, botOps, io, stmts, ret, params, brk, cnt, 
				  PROGRAM, lft, rght, op_splice};
typedef enum _astNodeType astNodeType;

/*dataType used inside:-
  params - handled!
  in declaration stmts	

*/
/*enum _typeExtract {none = 200, Int = 113, Float, Bool, Point = 108, Bot = 110};
typedef enum _typeExtract typeExtract;
*/
struct _typeExtract
{
	int type;
	int isArray;
	int bound1;
	int bound2;
};
typedef struct _typeExtract* typeExtract;
//type_extract->type

struct _ast_node;
typedef struct _ast_node ast_node;
typedef struct _ast_node* AST_NODE;

struct _ast_node{
	astNodeType type;
	bool isDummy;
	typeExtract type_extract;
	AST_NODE* children;
	AST_NODE parent;
	int num_children;
	TK token;
	int Lheight;
	int Rheight;
};

extern int IS_DECL;
extern int IS_ASSIGN;
extern int type_int;
extern int type_float;
extern int type_boolean;
extern int type_bot;
extern int type_point;
