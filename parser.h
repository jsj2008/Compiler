/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include "stack.h"
#include "hashtable.h"
#include "lexer.h"

struct grammar_rule {
	int lhs;
	int *rhs;
	int rhs_len;
};
typedef struct grammar_rule GRULE;

enum _leaf {nt, t, eps};
typedef enum _leaf leaf;
/*
enum _boolean {false, true};
typedef enum _boolean bool;
*/
struct tr_node;
typedef struct tr_node *TR_NODE;
struct tr_node {
	TR_NODE parent;
	//TR_NODE children[30];
	TR_NODE* children;		
	int num_children;
	leaf is_leaf;
	int active_child;
	int gr_rule; 
	TK token;
};

struct PTree {
	TR_NODE root;
	TR_NODE cur;
};
typedef struct PTree PTREE;

extern GRULE* grammar;
extern GRULE* firsts;
extern GRULE* follows;
extern int EPS;
