/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "ast.h"
#include "semantic_check.h"

//*************************************GLOBAL VARIABLES********************************************

char typeStr[][20] = {"bot", "point", "dataType", "function", "functionCall", "variable", "constant_value", "op", "assign", "conditional", "loop", "botOps", "io", "stmts", "ret", "params", "brk", "cnt", "PROGRAM", "lft", "rght", "op_splice"};

int second_op_rules[] = { 	144,		// MUL_OPS
							139,		// SUM_OPS
							125,		// LOG_OR
							128,		// LOG_AND
							131,		// RELATIONAL_OP
							148,		// PREFIX_OP 
							155		// TYPECASTING
						};

int primitive_types_list[3];
int operator_symbols[13];

int second_op_size;
int primitive_types_size;
int operator_symbols_size;

int IS_DECL = 0;
int IS_ASSIGN = 0;

int type_int=0;
int type_float=0;
int type_boolean=0; 
int type_bot=0; 
int type_point=0; 

//*****************************************FUNCTION HEADERS**************************************
int contains(int list[], int size, int symbol);
void makeInitialisations();
AST_NODE makeReturnNode(TR_NODE parseNode);
AST_NODE makeParamListNode(TR_NODE parseNode);
AST_NODE makeTypeListNode(TR_NODE parseNode);
AST_NODE makeFuncDeclNode(TR_NODE parseNode);
AST_NODE makeFuncCallNode(TR_NODE parseNode);
AST_NODE makeVarNode(TR_NODE parseNode);
AST_NODE makeValueNode(TR_NODE parseNode);
AST_NODE makeConditional(TR_NODE parseNode);
AST_NODE makeUnaryStmtsNode(TR_NODE parseNode);
TR_NODE addChildtoParseNode(TR_NODE head_of_new_parent, TR_NODE new_child);
TR_NODE makeExpressionNode_slave(TR_NODE cur_ast_node, int child_num);
AST_NODE convertParseToAST(TR_NODE parse_head);
AST_NODE parseToAst(TR_NODE cur_parse_node);
AST_NODE makeExpressionNode(TR_NODE parseNode);

void parentify(AST_NODE par, int num, AST_NODE child);
void copyToken(AST_NODE temp, TK token);
int getNumberOfEntities(AST_NODE id);
AST_NODE makeNewAstNode(TR_NODE parseNode, astNodeType type, bool isDummy, int num_children);
AST_NODE makeBotAssignNode(AST_NODE id, TR_NODE colAssign, TR_NODE botpoint1);
AST_NODE makeBotDeclNode(TR_NODE parseNode);
AST_NODE makeDeclarationNode(TR_NODE parseNode);
AST_NODE makeAssignmentNode(TR_NODE parseNode);	
AST_NODE makeLoopNode(TR_NODE parseNode);
AST_NODE makeIONode(TR_NODE parseNode);
AST_NODE makeBotNode(TR_NODE parseNode);
AST_NODE getStmtNode(TR_NODE stmt);
void addStmtNode(AST_NODE stmts, TR_NODE stmt);
AST_NODE makeStmtsNode(TR_NODE parseNode);
AST_NODE makeFunctions(TR_NODE parseNode);
AST_NODE makeAST(TR_NODE parseTree);

//***********************************************end**************************************//

void parentify(AST_NODE par, int num, AST_NODE child){
//make child num of par as child
	if(child)
		child->parent = par;
	par->children[num] = child;
	//printf("ret parentofy\n");
}

void copyToken(AST_NODE temp, TK token){
//copy into par-> token token
	if(temp)
	{
		temp->token.type   = token.type;
		temp->token.num    = token.num;
		temp->token.rnum   = token.rnum;
		temp->token.lineNo = token.lineNo;
		strcpy(temp->token.str, token.str);
	}
}

void replaceString(TR_NODE n, char c[], int ind){
	c[ind] = '\0';
	int i = 0;
	while(i<=ind){
		n->token.str[i] = c[i];
		i++;
	}
}


int getNumberOfEntities(AST_NODE id){
//TODO: function calls return multiple values!!!!!!
//check the number of entities in id, ie, if it's a splice, return count, else, return 1
/*
variables:  	a, b[10], c[2:9], d[3][4] 
return values:  1,	1, 	, 7, 	1	
*/  

	//printf("in the fn\n");	
	if (id == NULL) {
		//printf("null yaar\n");
	}
	/*if(strcmp(id->token.str, "barr") == 0)
	{
		printf("CHECK BARR\n");
		char fun[20]; gets(fun);
		printAST(id, 0);
		printf("CHECK BARR\n");
		printf("num childen = %d", id->num_children);
		printf("\nchikdsvnj %s\n", id->children[0]->token.str);
	}*/


	if(id){
		//printf("kaka\n");
		if(id->type == function){
			return getNoOfReturnValues(id->token.str);
		}
		//printf("lala\n");
		if(id->num_children == 0){
			//printf("ret\n");
			return 1; 
		}

		int entity = id->children[0]->token.type;
			//printf("case 33\n");
		
		if(entity != find_hash(tnt_hash, "COLON"))
			return 1;

		//printf("n:, nn:\n", id->num_children, id->children[0]->num_children);
		
		//printf("splice case\n");	
		return (id->children[0]->children[1]->token.num - id->children[0]->children[0]->token.num);
	} 
	return 0;
}

AST_NODE makeNewAstNode(TR_NODE parseNode, astNodeType typeN, bool isDummy, int num_children){
/*returns AST node of type root and default values from here if not passed to the function!	
Take care: 	case 1: if colassign ---> then convert to assign 
			case 2: convert true false to 0/1 => BOOLEAN resolution!
			if parseNode.token.type is ID, insert into symbol table?
*/
	AST_NODE temp = (AST_NODE)malloc(sizeof(struct _ast_node));
	temp->num_children = num_children;
	temp->children = (AST_NODE*) malloc(sizeof(AST_NODE)*num_children);
	temp->parent = NULL;
	temp->isDummy = isDummy;
	temp->type = typeN;
	
	//handle cases 1 and 2
	int t = parseNode->token.type;
	if(parseNode){
		if (t == find_hash(tnt_hash, "COL_ASSIGN")) {
				parseNode->token.type = find_hash(tnt_hash, "ASSIGN_OP");
		}
		else if (t == find_hash(tnt_hash, "TRUE")){
				parseNode->token.num = 1;
		}
		else if (t == find_hash(tnt_hash, "FALSE")){
				parseNode->token.num = 0;
		}
		copyToken(temp, parseNode->token);
	}
	
	//TODO: ADD switch case here. temp->type_extract->type  
	//and isArray = 0;
	//printf("before make ast node\n");
	if (typeN == constant_value) {
		//should be INT, FLOAT or BOOLEAN based on type
		temp->type_extract = makeNewTypeExtract(0,0,0,0);
		//temp->type_extract->isArray = 0;
		if (parseNode->token.type == find_hash(tnt_hash, "NUM")) {
			temp->type_extract->type = type_int;
		}
			//break;
		else if (parseNode->token.type == find_hash(tnt_hash, "RNUM")) {
			temp->type_extract->type = type_float;
			//break;
		}
		else if (parseNode->token.type == find_hash(tnt_hash, "TRUE") || parseNode->token.type == find_hash(tnt_hash, "FALSE")) {
			temp->type_extract->type = type_boolean;
			//break;
		}
	}

	return temp;
}

AST_NODE makeBotAssignNode(AST_NODE id, TR_NODE colAssign, TR_NODE botpoint1){
//returns node with AssignOP as root
//handling nested structures! Resolve and store!
	AST_NODE root, child[5], par[5];				//initialise later depending on if it's a point or bot
	TR_NODE cur_ast_node = botpoint1, next_ast_node;
	int num,i,j;								//number of children : 2 if point, 5 if bot
	char *ch;
	switch(cur_ast_node->gr_rule)
	{
		case 68:	//to handle point assignment, has 2 children
					child[0] = makeValueNode(cur_ast_node->children[0]);		
					child[1] = makeValueNode(cur_ast_node->children[2]);
					num = 2;
					break;
		case 67:	//could be point or bot, check!
					next_ast_node = cur_ast_node->children[3];
					switch(next_ast_node->gr_rule)
					{
						case 69: 	//to handle point assignment, has 2 children
									child[0] = makeVarNode(cur_ast_node->children[0]); //makeID node  makeNewAstNode()/makevarnode/makevalnode/makeidnode
									child[1] = makeValueNode(cur_ast_node->children[2]);
									num=2; break;
						case 70: 	//its a bot. handle pointx, pointy, dir, color, active
									num=5;
									strcpy(ch, cur_ast_node->children[0]->token.str);
									strcat(cur_ast_node->children[0]->token.str, "_x");
									child[0] = makeVarNode(cur_ast_node->children[0]);	//make ID node , id is cur_ast_node->children[0]'s name _x
									strcpy(cur_ast_node->children[0]->token.str, ch);
									strcat(cur_ast_node->children[0]->token.str, "_y");
									child[1] = makeVarNode(cur_ast_node->children[0]);	//make ID node , id is cur_ast_node->children[0]'s name _y	
 									child[2] = makeValueNode(cur_ast_node->children[2]);
									child[3] = makeValueNode(next_ast_node->children[1]);
									child[4] = makeValueNode(next_ast_node->children[3]); 
									break;
						default: printf("Don't know what to do!");
					}

		default: printf("Don't know what to do!");
	}

	root = makeNewAstNode(colAssign, assign, false,num);
	//make lhs of assignments
	//make ID node , name is id_x (id is a parameter for this function)
	char idname[25], *name[5];
	strcpy(idname, id->token.str);
	for(i=0; i<num; i++){
		strcpy(name[i], idname);
	}
	strcat(name[0], "_x");
	strcat(name[1], "_y");
	strcat(name[2], "_dir");
	strcat(name[3], "_color");
	strcat(name[4], "_active");
	
	//assign lhs of assignments!
	for(i=0, j=0; i<2*num, j<num; i+=2, j++){
		strcpy(cur_ast_node->children[0]->token.str, name[j]);
		par[j] = makeVarNode(cur_ast_node->children[0]);		//make new id node of type var with name as name[j]
		parentify(root, i, par[j]);
		//root->children[i] = ;
		//root->children[i]->parent = root;
	}
	//assign rhs of assignments!
	for(i=1, j=0; i<2*num, j<num; i+=2, j++){
		parentify(root, i, child[j]);
		//root->children[i] = child[j];
		//root->children[i]->parent = root;
	}

	return root;
}

AST_NODE makeBotDeclNode(TR_NODE parseNode){
/*Example  Bot b;				  ......INT.............................................
								/    \    /   \      /  \       /    \			/     \
 							   b1_x  0   b1_y 0  b1_dir  0  b1_color  0    b1_active  1
 //bot is active by default
Example Bot b1 := {p,1,3,false};
								  ......INT...............................................
								 /   \      /    \     /    \    /       \      /       \ 
 							   b1_x  p_x   b1_y p_y  b1_dir 1  b1_color  3    b1_active 0
Example  Point p;				  ......INT......
								 /   \        /  \
								p_x	 0      p_y    0

Example Point p1 := {a,5};	      ......INT......
								 /  \        /  \
							   p_x	 a     p_y   5
*/
//Initialise with default values
	IS_DECL = 1;

 	int num,i, j;
 	num = (parseNode->gr_rule == 44)? 4:10;			//4 children if point, else bot => 10 children
 	TR_NODE cur;
 	TR_NODE chi = parseNode->children[0];				// points to bot or point	
 	
 	strcpy((chi->token).str, "INT");
 	chi->token.type = find_hash(tnt_hash, "INT");
 	AST_NODE root = makeNewAstNode(chi, dataType, false, num);
 	AST_NODE par[5], child[5];
 	AST_NODE temp;

 	chi->token.type = find_hash(tnt_hash, "NUM");
 	chi->token.num = 0;
  	TR_NODE ch = parseNode->children[1];
 	char idname[35], name[5][40];
	strcpy(idname, ch->token.str);

	//printf("IDNAME IS $$$$$$$$$$$$$$$$$$$$$$$: %s", idname);
	for(i=0; i<num; i++){
		strcpy(name[i], idname);
	}
	strcat(name[0], "_x");
	strcat(name[1], "_y");
	strcat(name[2], "_dir");
	strcat(name[3], "_color");
	strcat(name[4], "_active");
 	
 	cur = parseNode->children[2];
	switch(cur->gr_rule)
	{
		case 45: //<pv_rest> => SEMI_CL 	
				insertInSymbolTable(find_hash(tnt_hash, "POINT"), ch->token, 0, 0);
				for(j=0; j<num/2; j++){
					if(j == 4) 
						chi->token.num = 1;	//bot is ACTIVE by default.
					child[j] = makeNewAstNode(chi,constant_value, false, 0);
				}
				//printf("Inside point/ bot declaration. without initialisations %d\n", num);
				break;
		case 48: //<bot_rest> => SEMI_CL 
				//in both these cases, default values have to be assigned.
				insertInSymbolTable(find_hash(tnt_hash, "BOT"), ch->token, 0, 0);
				for(j=0; j<num/2; j++){
					if(j == 4) 
						chi->token.num = 1;	//bot is ACTIVE by default.
					child[j] = makeNewAstNode(chi,constant_value, false, 0);
				}
				//printf("Inside point/ bot declaration. without initialisations %d\n", num);
				break;
		case 46: //<pv_rest>  => COL_ASSIGN LBRACE <idNum> COMMA <idNum> RBRACE SEMI_CL
				 insertInSymbolTable(find_hash(tnt_hash, "POINT"), ch->token, 0, 0);
				 IS_DECL = 0;
				 child[0] = makeValueNode(cur->children[2]);		
				 child[1] = makeValueNode(cur->children[4]);
				 IS_DECL = 1;
				 //sem semantic check
				 if (child[0]->type_extract->isArray != child[0]->num_children) {
				 	// ERROR - INVALID TYPE of parameter 1- INTEGER EXPECTED, RECEIVED ARRAY
					error_flags.error_code = invalidArrayUse;
				 	error_flags.line_number = child[0]->token.lineNo;
					strcpy(error_flags.error_message, "Line No. %d :: Can't use arrays here. Expected a variable of type INT as first parameter.\n");
				 	
				 	semantic_error();
				 }
				 else if (child[0]->type_extract->type != type_int) {
				 	// ERROR - INVALID TYPE of parameter 1- INTEGER EXPECTED, RECEIVED <TYPE>
				 	error_flags.error_code = invalidOperandsBot;
				 	error_flags.line_number = child[0]->token.lineNo;
					strcpy(error_flags.variable_name, child[0]->token.str);
					error_flags.type_of_mismatched_variable = child[0]->type_extract->type;
				 	error_flags.correct_type = type_int;
				 	
				 	semantic_error();
				 }

				 if (child[1]->type_extract->isArray > child[1]->num_children) {
				 	// ERROR - INVALID TYPE of parameter 2- INTEGER EXPECTED, RECEIVED ARRAY
				 	error_flags.error_code = invalidArrayUse;
				 	error_flags.line_number = child[1]->token.lineNo;
					strcpy(error_flags.error_message, "Line No. %d :: Can't use arrays here. Expected a variable of type INT as second parameter.\n");
				 	
				 	semantic_error();
				 }
				 else if (child[1]->type_extract->type != type_int) {
				 	// ERROR - INVALID TYPE of parameter 2- INTEGER EXPECTED, RECEIVED <TYPE>
				 	error_flags.error_code = invalidOperandsBot;
				 	error_flags.line_number = child[1]->token.lineNo;
					strcpy(error_flags.variable_name, child[1]->token.str);
					error_flags.type_of_mismatched_variable = child[1]->type_extract->type;
				 	error_flags.correct_type = type_int;
				 	
				 	semantic_error();
				 }
				 
				 //else ERROR
				 break;	
		case 49: //<bot_rest> =>COL_ASSIGN LBRACE ID COMMA <idNum> COMMA <idNum> COMMA <idBool> RBRACE SEMI_CL			    
				 insertInSymbolTable(find_hash(tnt_hash, "BOT"), ch->token, 0, 0);
				 //for entry of p in symbol table
				 IS_DECL = 0;
				 temp = makeVarNode(cur->children[2]);
				 if (findInSymbolTable(temp->token.str, 0) != type_point) {
				 	// ERROR - INVALID TYPE of parameter 1- POINT EXPECTED, RECEIVED <TYPE>
				 	printf("point se hai\n");
				 	error_flags.error_code = invalidOperands;
				 	error_flags.line_number = child[0]->token.lineNo;
					strcpy(error_flags.variable_name, child[0]->token.str);
					error_flags.type_of_mismatched_variable = child[0]->type_extract->type;
				 	error_flags.correct_type = type_point;
				 	
				 	semantic_error();
				 }
				 //entry for p_x done by makeVarNode's call to insertLineReference, type also assigned				 
				 strcpy(idname, cur->children[2]->token.str);   //copy p
				 strcat(cur->children[2]->token.str, "_x");	    //p_x
				 child[0] = makeVarNode(cur->children[2]);		

				 //entry for p_y done by makeVarNode's call to insertLineRef, type also assigned
				 strcpy(cur->children[2]->token.str, idname);
				 strcat(cur->children[2]->token.str, "_y");	
				 child[1] = makeVarNode(cur->children[2]);
				 
				 child[2] = makeValueNode(cur->children[4]);	
				 child[3] = makeValueNode(cur->children[6]);	
				 child[4] = makeValueNode(cur->children[8]);
				 //printf("child 4 ttype: %s\n", tnt_arr[child[4]->type_extract->type]);
				 //char fun[20];
				 //gets(fun);

				 IS_DECL = 1;	
				 //sem semantic check
				 
				 if (child[2]->type_extract->isArray > child[2]->num_children) {
				 	// ERROR - INVALID TYPE of parameter 2- INTEGER EXPECTED, RECEIVED ARRAY

				 	error_flags.error_code = invalidArrayUse;
				 	error_flags.line_number = child[2]->token.lineNo;
					strcpy(error_flags.error_message, "Line No. %d :: Can't use arrays here. Expected a variable of type INT as second parameter.\n");
				 	
				 	semantic_error();
				 }
				 else if (child[2]->type_extract->type != type_int) {
				 	// ERROR - INVALID TYPE of parameter 2- INTEGER EXPECTED, RECEIVED <TYPE>
					//printf("param 1 se hai\n");
				 	error_flags.error_code = invalidOperandsBot;
				 	error_flags.line_number = child[2]->token.lineNo;
					strcpy(error_flags.variable_name, child[2]->token.str);
					error_flags.type_of_mismatched_variable = child[2]->type_extract->type;
				 	error_flags.correct_type = type_int;
				 	
				 	semantic_error();
				 }

				 if (child[3]->type_extract->isArray > child[3]->num_children) {
				 	// ERROR - INVALID TYPE of parameter 3- INTEGER EXPECTED, RECEIVED ARRAY
				 	error_flags.error_code = invalidArrayUse;
				 	error_flags.line_number = child[3]->token.lineNo;
					strcpy(error_flags.error_message, "Line No. %d :: Can't use arrays here. Expected a variable of type INT as 3rd parameter.\n");
				 	
				 	semantic_error();
				 }
				 else if (child[3]->type_extract->type != type_int) {
				 	// ERROR - INVALID TYPE of parameter 3- INTEGER EXPECTED, RECEIVED <TYPE>
					//printf("param 2 se hai\n");
				 	error_flags.error_code = invalidOperandsBot;
				 	error_flags.line_number = child[3]->token.lineNo;
					strcpy(error_flags.variable_name, child[3]->token.str);
					error_flags.type_of_mismatched_variable = child[3]->type_extract->type;
				 	error_flags.correct_type = type_int;
				 	
				 	semantic_error();
				 }

				 if (child[4]->type_extract->isArray > child[4]->num_children) {
				 	// ERROR - INVALID TYPE of parameter 4- BOOLEAN EXPECTED, RECEIVED ARRAY
				 	error_flags.error_code = invalidArrayUse;
				 	error_flags.line_number = child[4]->token.lineNo;
					strcpy(error_flags.error_message, "Line No. %d :: Can't use arrays here. Expected a variable of type BOOLEAN as 4th parameter.\n");
				 	
				 	semantic_error();
				 }
				 else if (child[4]->type_extract->type != type_boolean) {
				 	// ERROR - INVALID TYPE of parameter 4- BOOLEAN EXPECTED, RECEIVED <TYPE>

					printf("param 3 se hai\n");
				 	error_flags.error_code = invalidOperandsBot;
				 	error_flags.line_number = child[4]->token.lineNo;
					strcpy(error_flags.variable_name, ch->token.str);
					error_flags.type_of_mismatched_variable = child[4]->type_extract->type;
				 	error_flags.correct_type = type_boolean;
				 	
				 	semantic_error();
				 }
				 //type of these returns should be int, int, boolean, not Array!!
				 //else ERROR
				 //type of temp should be point
				 break;
	}
 	//printf("DEBUG4 after switch:  \n");

	for(i=0, j=0; i<num; i+=2, j++){
		strcpy(ch->token.str, name[j]);
		par[j] = makeVarNode(ch);
		parentify(root, i, par[j]);
		parentify(root, i+1, child[j]);
		if (j != 4) {
			insertInSymbolTable(find_hash(tnt_hash, "INT"), par[j]->token, 0, 0);
		}
		else {
			insertInSymbolTable(find_hash(tnt_hash, "BOOLEAN"), par[j]->token, 0, 0);
		}
	}
	//printf("\n BOT DECL Node made %s, children: %d\n", typeStr[root->type], root->num_children);
	//printAST(root);

	IS_DECL = 0;
	return root;
}

AST_NODE makeDeclarationNode(TR_NODE parseNode) {
/*Example:  int a, b =5, c[7], arr[5][6];
// 
//				...........int................
//				/ \		 / \     / \      /  \ 
//			   a  NULL  b  5	c  NULL  arr NULL
//			   					|		 / \
//							    7       5   6
//
RULE 23:  <other_stmt> <primitive_declaration_stmt>
parseNode is at rule number 29 */	
	IS_DECL = 1;
	TR_NODE cur = parseNode, next,id, dtype;
	AST_NODE root, child[2];
	AST_NODE lhs[10],rhs[10];
	int pos=0, nextrule,i, j, num;
	dtype = cur->children[0]->children[0];
	int symType = dtype->token.type;
	while(cur->gr_rule != 34){
	//This loop fills in the lhs and rhs array for declarations, "pos" is the number of decls
	//RULE 34 is assign_comma => SEMI_CL;
		//currently at rule number 29 or 33!!
		nextrule = cur->children[2]->gr_rule; 
		//following switch cases apply for assignmore!!!!
		switch(nextrule)
		{
			case 30:	lhs[pos] = makeVarNode(cur->children[1]);
						rhs[pos] = makeExpressionNode(cur->children[2]->children[1]);
						cur = cur->children[2]->children[2];
						//cur now at assign_comma
						break;
			case 32:	//GRAMMAR RULE <assign_more> => <assign_comma>
						lhs[pos] = makeVarNode(cur->children[1]);
						rhs[pos] = NULL; 
						cur = cur->children[2]->children[0];
						//cur now at assigncomma
						break;
			case 31:	//It's an array!!
						//GRAMMAR RULE <assign_more> => LSQUARE NUM RSQUARE <array2> 
						id = cur->children[1];
						cur = cur->children[2];	//now cur is at assign_more, rule 31
						child[0] = makeValueNode(cur->children[1]); num = 1;
						cur = cur->children[3];		//cur now at <array2>
						switch(cur->gr_rule)
						{
							case 37: rhs[pos] = NULL;	 
									 cur = cur->children[0];
									 //cur now at assign_comma	
									 break;	
							case 35: //GRAMMAR RULE <array2> => ASSIGN_OP <curly_values> <assign_comma>
									 next = cur->children[1]->children[1]; //next at <values>
									 //TODO: evaluate <values> and assign children in rhs[pos]
									 //For now, rhs[pos] is NULL
									 rhs[pos] = NULL;	
									 cur = cur->children[2];	
									 //cur now at assign_comma 
									 break;	
							case 36: //GRAMMAR RULE  <array2> LSQUARE NUM RSQUARE <assign_2d> 
									 //It's a 2D array!!
									 child[1] = makeValueNode(cur->children[1]); num = 2;
									 cur = cur->children[3];
									 switch(cur->gr_rule)
									 {
									 	case 38: next = cur->children[1]; //next at <meta_curly>
									 			 //TODO: evaluate <meta_curly> and assign children in rhs[pos]
									 			 //For now, rhs[pos] is NULL
									 			 rhs[pos] = NULL;
									 			 cur = cur->children[2];
									 			 break;	
									 	case 39: rhs[pos] = NULL; 
									 			 cur=cur->children[0];
									 			 break;
									 }
									 //cur is now at assign comma
									 break;
						}
						lhs[pos] = makeNewAstNode(id, variable, false, num);
						parentify(lhs[pos], 0, child[0]);
						if(num==2) parentify(lhs[pos], 1, child[1]);
						break;
		}
		pos++;
	}

	root = makeNewAstNode(dtype, dataType, false, 2*pos);
	//printf("Declaration root type: %s\n\n", tnt_arr[root->token.type]);

	for(i=0, j=0;i<2*pos;i+=2, j++)
	{
		parentify(root, i, lhs[j]);
		parentify(root, i+1, rhs[j]);
		// SEMANTIC TYPE CHECKING
		//printf("sem chjeck del\n");
		if (rhs[j] && rhs[j]->type_extract->type != root->token.type && !(root->token.type == type_float && rhs[j]->type_extract->type)) {
			//printf("invalid type declared.\n");
			// ERROR - VALUE ASSIGNED TO VARIABLE <NAME> IS NOT OF THE <TYPE>, INSTEAD FOUND VALUE OF <TYPE>	
			setErrorFlags2(invalidValueDecl, lhs[j]->token.str, root->token.lineNo, root->token.type, rhs[j]->type_extract->type);
			semantic_error();
		}
	}
	
	for(i=0; i<pos; i++){
		//printf("AST is Inserting into symbol table\n");
		// change the array dimensions
		int b1=0, b2=0;
		if (lhs[i]->num_children == 2) {
			b1 = lhs[i]->children[0]->token.num;
			b2 = lhs[i]->children[1]->token.num;
		}
		else if (lhs[i]->num_children == 1) {
			b1 = lhs[i]->children[0]->token.num;
		}

		insertInSymbolTable(symType,lhs[i]->token, b1, b2);
	}

	IS_DECL = 0;
	return root;
}

AST_NODE makeAssignmentNode(TR_NODE parseNode) {	
/*	Example:  a, arr[4:7] , d[5] = 5, 1,2,3, 6;

				........... ASSIGN_OP................
						/			\
					   /			|
				     left 		  right
				....................................	   
				    /   \     |      /  \ \ \ \
			   	   a   arr    d   	 5	1 2 3 6
			   			|     | 
			   		  COLON	  5  
			   		   / \
					  4   7
			  	........... ASSIGN_OP................
				/ \		 /   \  \  \      /  \
			   a  5     arr  1	2  3     d   6 
			   			 |               | 
			   		  COLON	             5  
			   		   / \
					  4   7
*/

// CALLED FROM
/*	case 1: RULE NO 24 WHICH IS ASSIGNMENT STATEMENT CALLED THIS FUNCTION, i.e. <func_ass> -> <array> <lefthandside1> 
			parseNode is func_ass
	case 2: RULE NO 63 <assignment_stmt> => <var> <lefthandSide1> 
			parseNode is assignment stmt
*/
	//TODO: function calls return multiple values!!!!!!
	IS_ASSIGN = 1;
	AST_NODE lhs[10];
	AST_NODE rhs[10];
	AST_NODE root, left, right;
	int lhscur = 1, rhscur = 0,i,j, num=0;

	TR_NODE cur_ast_node = parseNode, var_node, n1,n2;
	//printf("\narrived at makeAssignmentNode with ruleNumber %d\n\n\n", parseNode->gr_rule);
	switch(parseNode->gr_rule)
	{
		//make appropriate var node depedning on where it was called from
		case 24:	//<func_ass> -> <array> <lefthandside1> 
					n1 = cur_ast_node->parent->children[0];
					n2 = cur_ast_node->children[0];
					var_node = getNewNode(nt);
					var_node->gr_rule = 78;				//********************************************************
					var_node->num_children = 2;
					var_node->children = (TR_NODE*)malloc(sizeof(TR_NODE)*2);	
					var_node->children[0] = n1;
					n1->parent = var_node;
					var_node->children[1] = n2;
					n2->parent = var_node;
					var_node->is_leaf = nt;
					break;
		case 63:	//<assignment_stmt> => <var> <lefthandSide1> 
					var_node = cur_ast_node->children[0];
					break;
	} 
	
	
	//start: make lhs and rhs arrays to hold assignment statements
	lhs[0] = makeVarNode(var_node);
	cur_ast_node = cur_ast_node->children[1];
	//if(!lhs[0]) 
	//printf("\ndebug assignment: var node check %d\n", lhs[0]->type);
	if(cur_ast_node->gr_rule == 66){
		return makeBotAssignNode(lhs[0],cur_ast_node->children[0],cur_ast_node->children[2]);
	}

	while (cur_ast_node->gr_rule == 65 || cur_ast_node->gr_rule == 73){
		lhs[lhscur++] = makeVarNode(cur_ast_node->children[1]); 
		cur_ast_node = cur_ast_node->children[2];
	}
	
	//check if its rule 64 then in next line it will be cur_ast_node->children[0]->children[0]
	if(cur_ast_node->gr_rule == 64){
		root = makeNewAstNode(cur_ast_node->children[0]->children[0], assign, false, 2);			//GRAMMAR RULE <lefthandSide1> <assign_operators> <rightHandSide>  
	}
	else{
		root = makeNewAstNode(cur_ast_node->children[0], assign, false, 2);			//create root node 
	}
	
	cur_ast_node = cur_ast_node->children[1];
//	printf("debug after left side of make ast assignment\n");   char cpty[20]; gets(cpty);

	rhs[rhscur++] = makeExpressionNode(cur_ast_node->children[0]);
//	printf("debug after right side of make ast assignment\n");   cpty[20]; gets(cpty);
	
	/*if (strcmp(rhs[rhscur-1]->token.str, "barr") == 0) {
			printf("check barr ast in assign\n");
			char fun[20]; gets(fun);
			printAST(rhs[rhscur-1], 0);
			//printf("child 1 hai333: %d\n", rhs[rhscur-1]->num_children);
			//printf("nc: %d\n", rhs[rhscur-1]->children[0]->type);
	}*/
	cur_ast_node = cur_ast_node->children[1];

	while(cur_ast_node->gr_rule == 76){
		rhs[rhscur++] = makeExpressionNode(cur_ast_node->children[1]); 
		
		cur_ast_node = cur_ast_node->children[2];
	}

	//cur_ast_node = cur_ast_node->children[0];
	//end: filling of lhs and rhs complete
	
	//add children to root node, make dummy nodes for assignment
	left = makeNewAstNode(cur_ast_node->children[0], lft, true, lhscur);
	right = makeNewAstNode(cur_ast_node->children[0], rght, true, rhscur);
	parentify(root, 0, left);
	parentify(root, 1, right);

	//add lhs
	for(i=0; i<lhscur; ++i)
	{
		parentify(left, i, lhs[i]);
		//insertLineReference(lhs[i]->token, lhs[i]);
	}	 
	//add rhs
	IS_ASSIGN = 0;
	for(i=0; i<rhscur; ++i)
	{
		parentify(right, i, rhs[i]);
	}	 

/*	num = lhscur+rhscur;
	root->num_children = num;	//yes, it isn't lhscur-1 
	root->children = (AST_NODE*) malloc(sizeof(AST_NODE)*num);
	//check error
	int leftEntities=0, rightEntities=0, bug;
	for(i=0; i< lhscur; i++) {
		bug = getNumberOfEntities(lhs[i]);
		leftEntities+=bug;
	}
	for(i=0; i< rhscur; i++) {
		bug = getNumberOfEntities(rhs[i]);
		rightEntities+=bug;
	}

	if(leftEntities != rightEntities){
		//TODO: semanticError 
		printf("Error at line %d, Can't complete assignment, Unequal number of entities on both sides\n", cur_ast_node->token.lineNo);
		char fun[20];
		gets(fun);
	}
	else{
		//printf("\nNo of entities match...moving forth with assignment\n %d", leftEntities);
		//make the node to be returned
		int lhspos=0, rhspos=0,en;
		AST_NODE l;
		for(i=0; i< num;){
			l=lhs[lhspos];
			//insertLineReference(lhs[lhspos]->token);

			parentify(root, i, l);
			//root->children[i] = l;
			//root->children[i]->parent = root;
			i++;
			en = getNumberOfEntities(l);
			for(j=0;j<en;){
				j+=getNumberOfEntities(rhs[rhspos]);
				if(j>en) {
				//TODO: semanticError flag error, invalid assignment. sthg of this type a=b[2:5]					
				}
				parentify(root, i, rhs[rhspos]);
				//root->children[i] = rhs[rhspos];
				//root->children[i]->parent = root;
				i++;
				rhspos++;
			}
			lhspos++;
		}	
	}*/
//	printf("debug before sem chk assignment\n"); char fun[20]; gets(fun);
	sem_chk_assignment(root);
//	printf("debug after sem chk assignment\n");  gets(fun);
	return root;	
}

AST_NODE makeLoopNode(TR_NODE parseNode){
/*RETURN 
				...........FOR...............
					/	  |   \          \
 			assignment expr  update loopstmts

Note: Always has 4 children. These are its regular children. Loop statements might have break and continue statements inside if stmt.

//TRIGGER FUNCTION  called by RULE NUMBER 20. Grammar rule: <other_stmt> => <for_stmt>
//current parse node is at for_stmt, i.e. RULE NUMBER  177   */
//		<for_stmt> FOR LPARA <assignment_stmt> <expression> SEMI_CL <update_stmt> RPARA LBRACE <loop_stmts>
//		<update_stmt> <assignment_stmt> 
//		<update_stmt> <unary_stmts>	

	TR_NODE cur = parseNode;			//Rule here is 177 
	AST_NODE root = makeNewAstNode(cur->children[0], loop, false, 4);
	AST_NODE child[4];
	int i;
	//Allocate children
	/*	child[0] => assignment 
		<assignment_stmt> => <var> <lefthandSide1>
		<func_ass> <array> <lefthandSide1> 
		<var> = ID <array> */
	child[0] = makeAssignmentNode(cur->children[2]);	//rule number 63
	child[1] = makeExpressionNode(cur->children[3]);	
	//switch case on update statements
	switch(cur->children[5]->gr_rule)
	{
		case 178: 	child[2] = makeAssignmentNode(cur->children[5]->children[0]);
					break;
		case 179:	child[2] = makeUnaryStmtsNode(cur->children[5]->children[0]);	
					break; 
		default: printf("Don't know what to do!\n");
	}

 	/*	child[3] => loop_stmts*/
 	child[3] = makeStmtsNode(cur->children[8]);		
 	for (i = 0; i < 4; ++i)
 	{
 		parentify(root, i,child[i]);
 	}

 	sem_chk_loop(root);

 	return root;
}

AST_NODE makeIONode(TR_NODE parseNode){
/*			READI
			  |
			 ID  			*/
//TRIGGER FUNCTION  called by RULE NUMBER 19, Grammar rule: otherstmt => iostmt 
//parse node at rule number 171 <io_stmt> => READI INPOP ID SEMI_CL
	TR_NODE cur = parseNode;
	AST_NODE root;
	root = makeNewAstNode(cur->children[0], io, false, 1);
	AST_NODE c = makeVarNode(cur->children[2]);	//make new id node with name from cur->children[2]
	//insertLineReference(c->token);
	// INSERT LINE REF IN SYMBOL TABLE

	parentify(root, 0, c);
	
	sem_chk_io(root);
	return root;
}

AST_NODE makeBotNode(TR_NODE parseNode){
/*
returns node of type botOps
parse node at <bot_stmt>
TRIGGER FUNCTION called by RULE NUMBER 172 or 173 or 174.
RULE 172:	<bot_stmt> => ADDV ID COMMA ID SEMI_CL				....ADDV.... 
														    	 / 		\
														      ID(a)  ID(b)

RULE 173:	<bot_stmt> => FW ID COMMA <expression> SEMI_CL    ...FW...
															     |	\
															    ID	 expr
RULE 174:   <bot_stmt> => RT ID SEMI_CL    ...RT...
										      |
									         ID
*/
	TR_NODE cur = parseNode;
	AST_NODE root;
	int num, rule = parseNode->gr_rule;
	num = (rule == 174)? 1:2;
	root = makeNewAstNode(parseNode->children[0], botOps, false, num);
	//asign first child
	parentify(root, 0, makeVarNode(parseNode->children[1]));
	if(num == 2)
	{
		if (rule == 172) 
			{
				parentify(root, 1, makeVarNode(parseNode->children[3]));
			}
		else {
			parentify(root,1, makeExpressionNode(parseNode->children[3]));		
		}
	}
	
	sem_chk_botNode(root);
	return root;
}

AST_NODE getStmtNode(TR_NODE stmt)
{//call appropriate function according to ruleNumber of <stmt> node.
 /*  <stmt> => <gen_stmt> 
	 <stmt> => <condnal_stmt>  */
	TR_NODE cur = stmt, c;
	if(cur && cur->children) c = cur->children[0];
	
	if(cur)
	{	
		while(1){
			//printf("Entered getStmtNode() for rule number: %d, %s\n", cur->gr_rule, tnt_arr[grammar[cur->gr_rule].lhs]);
			switch(cur->gr_rule)
			{
				//cases for ALL TRIGGER FUNCTIONS
				case 6 :	//<stmt> => <condnal_stmt>
							return makeConditional(c);    
				case 17:    //<gen_stmt> => ID <func_ass>	
							if(cur->children[1]->gr_rule == 24) //<func_ass> <array> <lefthandSide1> 
								return makeAssignmentNode(cur->children[1]); 
							else								//<func_ass> <funcCall_stmt>
								return makeFuncCallNode(cur->children[1]->children[0]); 	//What arg does it expect? <funcCall_stmt>

				case 19: 	return makeIONode(c); 
				case 20: 	return makeLoopNode(c); 
				case 22:	return makeUnaryStmtsNode(c);	//What argument does this function expect? <unary_stmts>
				case 23:	return makeDeclarationNode(c); 
				case 172:	return makeBotNode(cur);
				case 173:	return makeBotNode(cur);
				case 174:	return makeBotNode(cur); 
				case 175:	return makeBotDeclNode(c); 
				case 176:	return makeBotDeclNode(c); 
				/*************** for handling <func_stmts> and <loop_stmts>  ***************/
				case 166:	return makeReturnNode(cur);	//returns type ret
				case 10:	return makeNewAstNode(c, brk, false, 0);	
				case 11:	return makeNewAstNode(c, cnt, false, 0);
				case 13:	return makeConditional(c);		//returns loop conditional
				
				default : 	if(cur == NULL){
								printf("Inside get stmt node, got NULL argument!");
								return NULL;
							} 
							cur = cur->children[0];	
							if(cur) c= cur->children[0];
			}
		}	
	}
	else {
		printf("Inside get stmt node, got NULL argument!");
	}
} 

void addStmtNode(AST_NODE stmts, TR_NODE stmt)
{//add stmt node to stmts at it's child number++
 //resize stmts if necessary!
	AST_NODE st;
	if(stmts->num_children % 40 ==0){
		//time to resize! 
		//TODO: Check syntax!
		//realloc(stmts->children);	
	}
	st = getStmtNode(stmt);
	//printAST(st);
	//printf("just before exit 1 %d",stmts->num_children);
	parentify(stmts, stmts->num_children, st);
	//printf("just before exit 2%d",stmts->num_children);

	stmts->num_children++;
	//printf("just before exit 3 %d",stmts->num_children);
}


AST_NODE makeStmtsNode(TR_NODE parseNode)
{
//handles <stmts> and <func_stmts>
//returns dummy node stmts!!
//parseNode grammar rule is <stmts> which can be rule 3 or 4
/* rule 3:	<mainFunction> => <stmts> 
   what if there's rule <func_stmts> => RBRACE   GRAMMAR RULE 180
*/
	if (parseNode->gr_rule != 165) {
		// DO NOT ADD NEW SCOPE IF IT IS A FUNCTION CALL 
		addNewScope();		
	}

	AST_NODE root;
	//for now, max num children is 40. Addchildren will automatically resize. 
	root = makeNewAstNode(parseNode, stmts, true, 70);
	root->num_children = 0;
	//increment numberOfChildren when a statement is added. 40 have been allocated.
	while(parseNode && (parseNode->gr_rule != 4 && parseNode->gr_rule != 2 && parseNode->gr_rule != 8))		//RULE 2 <func_stmts> => RBRACE
	{
		if(parseNode->gr_rule == 166)
		{
			addStmtNode(root, parseNode);
			break;	
		}
		if(parseNode->gr_rule == 7)						//GRAMMAR RULE
		{
			addStmtNode(root, parseNode->children[0]);
		}
		else 	//GRAMMAR RULE 165 										
				//GRAMMAR RULE 3 <stmts> => <stmt> <stmts> 		 
		{
			addStmtNode(root, parseNode->children[0]);
			//printf("Added another statement. Statements has %d children now.\n", root->num_children);
		}
		parseNode = parseNode->children[1];
	}
	//printf("Exiting make stmts node. Finally had %d children. ", root->num_children);

	gotoPrevScope();
	return root;
}

AST_NODE makeFunctions(TR_NODE parseNode)
{/* RETURN:             Program (..dummy)
					    /   |   \	
					   f1  f2   NULL
parseNode is rule number 162 <otherFunctions> => <function> <otherFunctions> 
returns a dummy node with text Program that has all user defined functions and LAST child will hold main function (stmts dummy node)
max #fns is 21
*/ 
	TR_NODE cur = parseNode;
	AST_NODE childFn[21], program;
	int numFns, pos=0, i;

	while(cur->gr_rule != 163){
		childFn[pos] = makeFuncDeclNode(cur->children[0]);		
		cur = cur->children[1];
		++pos;
	}
	numFns = pos+1; 	//add one for main function
	cur = cur->children[0];
	strcpy(cur->token.str, "PROGRAM");
	
	//Make dummy program node! take parse node of MAIN, replace text with PROGRAM and make!
	program = makeNewAstNode(cur, PROGRAM, true, numFns);
	for(i=0;i<numFns-1;i++){
		parentify(program, i, childFn[i]);
	}
	program->children[numFns-1] = NULL;	//Will eventually hold main function
	return program;
}


AST_NODE makeAST(TR_NODE parseTree)
{
//GRAMMAR RULE	<program> => <global_vars> <otherFunctions> <mainFunction>
/*					
					   	 Program (..dummy)
					    /   |   \	
					   f1  f2   stmts(..dummy) 

*/			

	makeInitialisations();

	TR_NODE cur = parseTree, globs;
	AST_NODE head;
	int mainChild;

	// Insert Global Variables
	globs = cur->children[0];
	while (globs->gr_rule != 15) {
		insertInSymbolTable(globs->children[0]->children[0]->children[0]->token.type, globs->children[0]->children[1]->token, 0, 0);
		//printf("GLOBAL VAR\n");
		if (globs->children[0]->children[3]->children[0]->token.type == find_hash(tnt_hash, "NUM")) {
			//printf("coding global \n");
			code_global(globs->children[0]->children[3]->children[0]->token.num);
		}
		else if (globs->children[0]->children[3]->children[0]->token.type == find_hash(tnt_hash, "TRUE")) {
			code_global(1);
		}
		else if (globs->children[0]->children[3]->children[0]->token.type == find_hash(tnt_hash, "FALSE")) {
			code_global(0);
		}
		globs = globs->children[1];
	}

	//initialise head from other functions
	head = makeFunctions(cur->children[1]);
	mainChild = head->num_children-1;
	cur = cur->children[2];
	//GRAMMAR RULE <mainFunction> => <stmts> 
	parentify(head, mainChild, makeStmtsNode(cur->children[0])); 
	return head;
}


void makeInitialisations() {
	type_int = find_hash(tnt_hash, "INT");
	type_float = find_hash(tnt_hash, "FLOAT");
	type_boolean = find_hash(tnt_hash, "BOOLEAN");
	type_bot = find_hash(tnt_hash, "BOT");
	type_point = find_hash(tnt_hash, "POINT");

	operator_symbols[0] = find_hash(tnt_hash, "PLUS");
	operator_symbols[1] = find_hash(tnt_hash, "LT");
	operator_symbols[2] = find_hash(tnt_hash, "GT");
	operator_symbols[3] = find_hash(tnt_hash, "LTE");
	operator_symbols[4] = find_hash(tnt_hash, "GTE");
	operator_symbols[5] = find_hash(tnt_hash, "MINUS");
	operator_symbols[6] = find_hash(tnt_hash, "LOG_EQ");
	operator_symbols[7] = find_hash(tnt_hash, "LOG_AND");
	operator_symbols[8] = find_hash(tnt_hash, "LOG_OR");
	operator_symbols[9] = find_hash(tnt_hash, "MULTIPLY");
	operator_symbols[10] = find_hash(tnt_hash, "DIVIDE");
	operator_symbols[11] = find_hash(tnt_hash, "UNARY_INCR");		// CHECK WITH GRAMMAR
	operator_symbols[12] = find_hash(tnt_hash, "UNARY_DECR");

	primitive_types_list[0] = find_hash(tnt_hash, "INT");
	primitive_types_list[1] = find_hash(tnt_hash, "FLOAT");
	primitive_types_list[2] = find_hash(tnt_hash, "BOOLEAN");

	second_op_size = 7;
	operator_symbols_size = 12;
	primitive_types_size = 3;
}

AST_NODE makeReturnNode(TR_NODE parseNode) {
	/* returns type ret
	   RETURN	
	  /   |  \
	ex1 ex2 ex3
	Rule No: 166
	<fn_stmts> 		->		RETURN <expressions> SEMI_CL RBRACE
	*/

	AST_NODE root;
	AST_NODE ch[20];			// MAX 20 RETURN EXPRESSIONS
	int c=1, i;					// AT LEAST ONE RETURN VALUE IS NECESSARY ELSE SKIP RETURN STATEMENT
	TR_NODE local = parseNode->children[0];	// STORE RETURN NODE

	parseNode = parseNode->children[1];
	ch[0] = makeExpressionNode(parseNode->children[0]);

	parseNode = parseNode->children[1];
	while (parseNode->gr_rule == 113) {			// GRAMMAR_RULE
		// INVARIANT RULE:
		// <more_expression> 		->		COMMA <expression> <more_expression> 
		ch[c] = makeExpressionNode(parseNode->children[1]);
		parseNode = parseNode->children[2];
		c+=1;
	}

	root = makeNewAstNode(local, ret, true, c);
	for (i=0; i<c; i++) {
		parentify(root, i, ch[i]);
	}

	return root;

}

int contains(int list[], int size, int symbol) {
	// HELPER FUNCTION TO TEST IF A LIST CONTAINS AN INTERGER
	int i=0;
	while (i < size) {
		if (list[i] == symbol) {
			return 1;
		}
		i++;
	}

	return 0;
}

AST_NODE makeParamListNode(TR_NODE parseNode) {
	/*
					DUMMY
					/ 	\
				  INT 	INT
				   |	 |
				   a 	 b
	*/
	AST_NODE root, ch_id[20], ch_type[20];				// MAX 20 PARAMETERS ALLOWED TO A FUNCTION
	int c = 0, i;

	if (parseNode->gr_rule == 167) {					// GRAMMAR_RULE
		// AT LEAST ONE PARAMETER EXISTS
		c=1;
		ch_id[0] = makeNewAstNode(parseNode->children[1], variable, false, 0);
		if (parseNode->children[0]->gr_rule == 52) {
			// PRIMITIVE TYPE -> INT OR FLOAT
			ch_type[0] = makeNewAstNode(parseNode->children[0]->children[0]->children[0], dataType, false, 1);	
		}
		else if (parseNode->children[0]->children[0]->gr_rule == 62){
			// BOT TYPE
			ch_type[0] = makeNewAstNode(parseNode->children[0]->children[0]->children[0], bot, false, 1);		
		}
		else {
			// POINT TYPE
			ch_type[0] = makeNewAstNode(parseNode->children[0]->children[0]->children[0], point, false, 1);
		}
		ch_type[0]->children[0] = ch_id;
		ch_id[0]->parent = ch_type;
		insertInSymbolTable(ch_type[0]->token.type, ch_id[0]->token, 0, 0);

		parseNode = parseNode->children[2];
		
		while(parseNode->gr_rule == 169) {				// GRAMMAR_RULE
			ch_id[c] = makeNewAstNode(parseNode->children[2], variable, false, 0);
			if (parseNode->children[1]->gr_rule == 52) {
				// PRIMITIVE TYPE -> INT OR FLOAT
				ch_type[c] = makeNewAstNode(parseNode->children[1]->children[0]->children[0], dataType, false, 1);	
			}
			else {
				// BOT TYPE
				ch_type[c] = makeNewAstNode(parseNode->children[1]->children[0]->children[0], bot, false, 1);		
			}
			ch_type[c]->children[0] = ch_id;
			ch_id[c]->parent = ch_type;
			
			insertInSymbolTable(ch_type[c]->token.type, ch_id[c]->token, 0, 0);

			parseNode = parseNode->children[3];	
			c+=1;
		}	
	}

	root = makeNewAstNode(parseNode, params, true, c);
	for (i=0; i<c; i++) {								// ASSIGN ALL PARAMETERS TO THE PARAMS ROOT DUMMY NODE
		//ch_type[i]->parent = root;
		//root->children[i] = ch_type[i];
		parentify(root, i, ch_type[i]);
	}

	return root;
}

AST_NODE makeTypeListNode(TR_NODE parseNode) {
	/* I WANT THE TYPE_LIST NODE AS INPUT TO THE FUNCTION
	<type_list> 	->		<type> <type_more> 
	*/
	AST_NODE root, types[20];									// CAN RETURN MAXIMUM 20 VARIABLES
	int c=0, i;
	if (parseNode->gr_rule == 55) {							// GRAMMAR_RULE
			parseNode = parseNode->children[1];
		}
	else if (parseNode->gr_rule == 54) {								// GRAMMAR_RULE	
		types[0] = makeNewAstNode(parseNode->children[0]->children[0]->children[0], dataType, false, 0);

		parseNode = parseNode->children[1];
		c=1;

		while (parseNode->gr_rule == 56) {						// GRAMMAR_RULE
			if (parseNode->children[1]->gr_rule == 52) {		// GRAMMAR_RULE
				// INT OR FLOAT
				types[c] = makeNewAstNode(parseNode->children[1]->children[0]->children[0], dataType, false, 0);
			}
			else if (parseNode->children[1]->gr_rule == 62) {	// GRAMMAR_RULE
				// BOT
				types[c] = makeNewAstNode(parseNode->children[1]->children[0]->children[0], bot, false, 0);	
			}
			else {
				// POINT
				types[c] = makeNewAstNode(parseNode->children[1]->children[0]->children[0], point, false, 0);		
			}
			c+=1;
			parseNode = parseNode->children[2];
		}
		
		if (parseNode->gr_rule == 57) {					// GRAMMAR_RULE
			parseNode = parseNode->children[0];
		}	
	}
	root = makeNewAstNode(parseNode, ret, true, c);
	for (i=0; i<c; i++) {								// ASSIGN ALL return_types TO THE RET ROOT DUMMY NODE
		//types[i]->parent = root;
		//root->children[i] = types[i];
		//printf("#$#$#$%s\n", types[i]->token.str);
		parentify(root, i, types[i]);
	}

	return root;
}


AST_NODE makeFuncDeclNode(TR_NODE parseNode) {
	// BOT TYPE --> REMEMBER
	/*<function> 	->		FUNCTION <type_list> LPARA <parameter_list> LBRACE <fn_stmts>
	RETURNS:
						FUNCTION
					/		|		\
				RET_TYPE   PARAMS 	FUNC_STMTS
	*/

	addNewScope();
	AST_NODE ch2 = makeParamListNode(parseNode->children[3]);
	// INSERT ALL PARAMETERS IN THE SYMBOL TABLE

	addNewScope();
	AST_NODE ch1 = makeTypeListNode(parseNode->children[1]);	
	AST_NODE ch3 = makeStmtsNode(parseNode->children[5]);
	AST_NODE root = makeNewAstNode(parseNode->children[0], function, false, 3);
	copyToken(root, ch1->token);	// The makeTypeListNode returns a dummy node which has the ID of the function defined here
	parentify(root, 0, ch1);
	parentify(root, 1, ch2);
	parentify(root, 2, ch3);

	insertFunction(root->token.str, root->children[0]->num_children, root->children[1]->num_children, root);
	printScopeResTable();
	gotoPrevScope();

	sem_chk_functionDecl(root);

	return root;
}

AST_NODE makeFuncCallNode(TR_NODE parseNode) {
	AST_NODE root, ids[20], vals[20];						// MAX 20 ARGUMENTS TO A FUNCTION
	TR_NODE parseNode_root = parseNode;
	int c;
	
	parseNode = parseNode->children[1]; 
	/*<funcCall_stmt> 		->		LPARA <funcCall_stmt_rest>*/


	if ((parseNode->gr_rule == 161) || (parseNode->gr_rule == 94)) {						// GRAMMAR_RULE
		// f(); ==> NO ARGUMENTS
		// RETURNS A FUNCTION AST NODE WITH 0 CHILDREN
		return makeNewAstNode(parseNode_root->parent->parent->children[0], functionCall, false, 0);
	}
	else if ((parseNode->gr_rule == 160) || (parseNode->gr_rule == 93)) {					// GRAMMAR_RULE
		parseNode = parseNode->children[0];
		// TODO - REARRANGE THE ARGUMENTS IN ORDER OF ENTRY IN SYMBOL TABLE AND REMOVE THE IDS

		/*<funcCall_stmt_rest> 		->		<arg_list> SEMI_CL
		RETURNS:
					    	FUNCTION(f)
					/		|	  	  |	  	\
				 ID1(a)  Value1(23) ID2(b)  Value2(c)
		
		NOTE: CANNOT PASS ARRAYS TO FUNCTION CALLS, CAN PASS ONLY NAME, VALUE PAIRS - 
		SAMPLE CALL: f(a=23, b=c)
		*/
		c = 1;
		ids[0] = makeNewAstNode(parseNode->children[0], variable, false, 0);
		//insertLineReference(ids[0]->token, ids[0]);
		// INSERTING LINE REF IN SYMBOL TABLE

		vals[0] = makeValueNode(parseNode->children[2]);

		while (parseNode->children[3]->gr_rule == 157) {			// GRAMMAR_RULE
			parseNode = parseNode->children[3]->children[1];
			ids[c] = makeNewAstNode(parseNode->children[0], variable, false, 0);
			//insertLineReference(ids[c]->token, ids[c]);
			// INSERTING LINE REF IN SYMBOL TABLE

			vals[c] = makeValueNode(parseNode->children[2]);
			c+=1;
		}
		
		int i=0;
		root = makeNewAstNode(parseNode_root->parent->parent->children[0], functionCall, false, c*2);
		//printAST(root, 0);
		//printAST(ids[0], 0);
		//printAST(vals[0], 0);
		
		while (i<c*2) {
			//printf("paret\n");
			if (i%2 == 0) {
				parentify(root, i, ids[i]);
			}
			else {
				parentify(root, i, vals[i-1]);	
			}
			i+=1;
		}
	}
		//printf("\nbefore sem chk func call\n");

	sem_chk_functionCall(root);
	//printAST(root, 0);

	return root;

}

AST_NODE makeVarNode(TR_NODE parseNode) {
	/*
	CASE 1: DIRECT ID
	CASE 2: <VAR> ->  ID <ARRAY>
	*/
	TR_NODE local = parseNode;
	AST_NODE root;
	AST_NODE ch1 = NULL;
	AST_NODE ch2 = NULL;

	if (parseNode->is_leaf == t) {
		//printf("case 0\n");
		AST_NODE temp = makeNewAstNode(parseNode, variable, false, 0);
		insertLineReference(parseNode->token, temp);
		//printf("lene aksjdad\n");
		//if (IS_DECL == 0)
			//printf("type: %s\n", tnt_arr[temp->type_extract->type]);
		return temp;
	}
	else if(parseNode->gr_rule == 78) {					// GRAMMAR_RULE
		if (parseNode->children[1]->gr_rule == 80) {		// GRAMMAR_RULE
			// 0 CHILDREN
			AST_NODE temp = makeNewAstNode(parseNode->children[0], variable, false, 0);
			// INSERT LINE REF IN SYMBOL TABLE
			insertLineReference(parseNode->children[0]->token, temp);

			return temp;
		}
		else if (parseNode->children[1]->gr_rule == 79) {		// GRAMMAR_RULE
			local = local->children[1];
			if (local->children[2]->gr_rule == 81) {		// GRAMMAR_RULE
				local = local->children[2];

				if (local->children[1]->gr_rule == 84) {		// GRAMMAR_RULE
					// 2-D ARRAY
					ch1 = makeExpressionNode(parseNode->children[1]->children[1]);
					ch2 = makeExpressionNode(local->children[1]->children[1]);
					//ch1->type = constant_value;
					//ch2->type = constant_value;	
					root = makeNewAstNode(parseNode->children[0], variable, false, 2);
					
					parentify(root, 0, ch1);
					parentify(root, 1, ch2);
					
					insertLineReference(parseNode->children[0]->token, root);
					// INSERT LINE REF IN SYMBOL TABLE
					return root;
				}

				// 1-D ARRAY
				//printf("\ndebug assign: 1 d array case \n");
				ch1 = makeExpressionNode(parseNode->children[1]->children[1]);
				//ch1->type = constant_value;
				root = makeNewAstNode(parseNode->children[0], variable, false, 1);

				parentify(root, 0, ch1);
				//root->children[0] = ch1;

				insertLineReference(parseNode->children[0]->token, root);
				// INSERT LINE REF IN SYMBOL TABLE
				
				return root;
			}
			else if (parseNode->children[1]->children[2]->gr_rule == 82) {						// GRAMMAR_RULE
				// [2:5]
				//printf("\ndebug assign: splice case \n");
				local = parseNode->children[1]->children[2];
				ch1 = makeExpressionNode(parseNode->children[1]->children[1]);
				ch2 = makeExpressionNode(local->children[1]);
				root = makeNewAstNode(parseNode->children[0], variable, false, 1);
				// INSERT LINE REF IN SYMBOL TABLE
				
				AST_NODE colon = makeNewAstNode(parseNode->children[1]->children[2]->children[0], op_splice, false, 2);
				//colon->children[0] = ch1;
				//colon->children[1] = ch2;
				parentify(colon, 0, ch1);
				parentify(colon, 1, ch2);
				parentify(root, 0, colon);
				//root->children[0] = colon;
				insertLineReference(root->token, root);
				
				/*printf("root type: %d\n", colon->type);
				printf("var_name, isArray: %s, %d\n", root->token.str, root->type_extract->isArray);
				printf("dim1 : %d\n", ch1->token.num);
				printf("dim2 : %d\n", ch2->token.num);
				*/
				return root;
			}
		}
	}
}

void printAST(AST_NODE ptr, int level) {
	int i;
	if(!ptr) {
		print_values_a(ptr, level);
		return;
	}
	print_values_a(ptr, level);
	char fun[10];
	gets(fun);
	for(i=0; i < ptr->num_children; i++)
	{
		//printf("No of children : %d", ptr->num_children);
		printAST(ptr->children[i], level+1);
	}
}


AST_NODE makeValueNode(TR_NODE parseNode /*int type_of_value*/) {
	// CASE 1: CONST VALUE 
	// CASE 2: ID + FUNC_CALL
	// CASE 3: IDNUM
	// CASE 4: DIRECT NUM / RNUM
	if (parseNode->is_leaf == t) {
		// CASE 4
		// I am given the NUM / RNUM / BOOL node and I return the "ID" type AST_NODE
		return makeNewAstNode(parseNode, constant_value, false, 0);
	}
	else if (parseNode->gr_rule == 88) {													// GRAMMAR_RULE
		// CASE 1
		/* 
		<value> 		->		<const_value>			********* WE ARE HERE **********
		<const_value> 	->		NUM 
		<const_value> 	->		RNUM 
		<const_value> 	->		<bool>		-> 		TRUE | FALSE
		*/
		if (parseNode->children[0]->gr_rule == 101) {
			return makeNewAstNode(parseNode->children[0]->children[0]->children[0], constant_value, false, 0);
		}
		return makeNewAstNode(parseNode->children[0]->children[0], constant_value, false, 0);
	}
	else if (parseNode->gr_rule == 89 && parseNode->children[1]->gr_rule == 90) {		// GRAMMAR RULE
		// CASE 2
		/* 	<value> 				 ->		ID <value_rest>      ********** WE ARE HERE ************
			<value_rest>  			 ->   	<funcCall_inside_Value> 
			<funcCall_inside_Value>  ->   	LPARA <funcCall_inside_Value_rest>
		*/
		return makeFuncCallNode(parseNode->children[1]->children[0]);
	}
	else if (parseNode->gr_rule == 89 && parseNode->children[1]->gr_rule == 91) {		// GRAMMAR RULE
		/*
		<value> 		->		ID <value_rest>
		<value_rest> 	->		<array>
		makes a var node using value->ID and value_rest->array	
		*/
		TR_NODE n1 = parseNode->children[0];
		TR_NODE n2 = parseNode->children[1]->children[0];
		
		TR_NODE var_node = getNewNode(nt);
		var_node->gr_rule = 78;				// GRAMMAR RULE 
		var_node->num_children = 2;
		var_node->children = (TR_NODE*)malloc(sizeof(TR_NODE)*2);	
		var_node->children[0] = n1;
		n1->parent = var_node;
		var_node->children[1] = n2;
		n2->parent = var_node;
		var_node->is_leaf = nt;
		
		//printf("\n shuchita varNode in value\n");
		
		AST_NODE temp = makeVarNode(var_node);
		/*if (strcmp(temp->token.str, "barr") == 0) {
			printf("check barr ast in value node\n");
			char fun[20]; gets(fun);
			printAST(temp, 0);
		}*/	
		return temp;
	}
	else if (parseNode->gr_rule == 72) {													// GRAMMAR RULE
		// CASE 3
		// IDNUM -> NUM 
		return makeNewAstNode(parseNode->children[0], constant_value, false, 0);
	}
	else if (parseNode->gr_rule == 96) {													// GRAMMAR_RULE
		// CASE 3
		// IDBOOL -> BOOL(T|F)
		return makeNewAstNode(parseNode->children[0]->children[0], constant_value, false, 0);
	}	
	else if (parseNode->gr_rule == 95 || parseNode->gr_rule == 71) {					// GRAMMAR_RULE
		// CASE 3
		// IDNUM -> ID  OR  IDBOOL -> ID
		return makeNewAstNode(parseNode->children[0], variable, false, 0);
	}
}



AST_NODE makeConditional(TR_NODE parseNode) {
	/* I AM PASSED THE <CONDNAL_STMT> NODE AS PARSENODE */

	AST_NODE root = NULL;
	AST_NODE ch1 = NULL;
	AST_NODE ch2 = NULL;
	AST_NODE ch3 = NULL;
	TR_NODE local = parseNode;

	/*
	107 <condnal_stmt> 		-> 		IF LPARA <expression> RPARA LBRACE <stmts> <condnal_stmt_more> 
	102 <loop_condnal_stmt> 	->		IF LPARA <expression> RPARA LBRACE <loop_stmts> <loop_condnal_stmt_more>
	*/

	//parseNode = parseNode->children[0];
	root = makeNewAstNode(parseNode->children[0], conditional, false, 3);
	ch1 = makeExpressionNode(parseNode->children[2]);
	if (parseNode->gr_rule == 107) {
		// CONDNAL STMT
		ch2 = makeStmtsNode(parseNode->children[5]);
	}
	else {
		// LOOP CONDNAL STMT
		ch2 = makeStmtsNode(parseNode->children[5]);	
	}
	/*root->children[0] = ch1;
	ch1->parent = root;
	root->children[1] = ch2;
	ch2->parent = root;*/
	parentify(root, 0, ch1);
	parentify(root, 1, ch2);
	
	if ((parseNode->children[6]->gr_rule == 109) || (parseNode->children[6]->gr_rule == 104)) {				// GRAMMAR_RULE
		// 2 CHILDREN  
		// <condnal_stmt_more> 		->		EPS
		root->num_children = 2;
	}
	else if((parseNode->children[6]->gr_rule == 108) || (parseNode->children[6]->gr_rule == 103)) {			// GRAMMAR_RULE
		// 3 CHILDREN
		local = parseNode->children[6];
		if (local->children[1]->gr_rule == 111) {					// GRAMMAR_RULE
			/*<condnal_stmt_even_more> 		->		LBRACE <stmts>
			IF .. THEN .. ELSE IF */
			ch3 = makeStmtsNode(local->children[1]->children[1]);
			parentify(root, 2, ch3);
			/*root->children[2] = ch3;
			ch3->parent = root;*/
		}
		else if (local->children[1]->gr_rule == 106) {
			ch3 = makeStmtsNode(local->children[1]->children[1]);
			parentify(root, 2, ch3);
		}
		else if ((local->children[1]->gr_rule == 110) || (local->children[1]->gr_rule == 105)) {			// GRAMMAR_RULE
			/* <condnal_stmt_even_more> 	->		<condnal_stmt> 
			IF .. THEN .. ELSE */
			ch3 = makeConditional(local->children[1]->children[0]); 
			parentify(root, 2, ch3);
			/*root->children[2] = ch3;
			ch3->parent = root;*/
		}
	}

	sem_chk_conditional(root);
	return root;
}

AST_NODE makeUnaryStmtsNode(TR_NODE parseNode) {
	/* I want <unary_stsmts> parse node to be passed into this function 
	<unary_stmts> <prefix_op> ID <unary_stmt_more> */

	//parseNode = parseNode->children[0];
	AST_NODE root = makeNewAstNode(parseNode->children[0]->children[0], op, false, 2);
	AST_NODE ch;
	ch = makeNewAstNode(parseNode->children[1], variable, false, 0);
	//printf("t:%d, l: %d, n:%s 1\n", ch->token.type, ch->token.lineNo, ch->token.str);
	parentify(root, 0, ch);
	insertLineReference(ch->token, ch);
	//printf("t:%d, l: %d, n:%s 2\n", ch->token.type, ch->token.lineNo, ch->token.str);

	AST_NODE sub_root = root;
	sem_chk_unary(root);

	parseNode = parseNode->children[2];
	while (1) {
		if (parseNode->gr_rule == 28) {														// GRAMMAR_RULE
			/*AFTER THIS STEP:
				   				PREF_OP  			********** WE ARE HERE **********
								   |
								  ID	 
			*/
			sub_root->children[1] = NULL;
			sub_root->num_children = 1;
			break;
		}
		else {
			// MAKES A NEW PREFIX_OP NODE AS RIGHT CHILD OF CURRENT PREFIX_OP NODE
			/*AFTER THIS STEP:
				   				PREF_OP  			********** WE ARE HERE **********
								 /	 \
								ID	 PREF_OP
										\
										ID
			*/
			AST_NODE ch1 = makeNewAstNode(parseNode->children[1]->children[0], op, false, 2);
			ch1->children[0] = makeNewAstNode(parseNode->children[2], variable, false, 0);			
			ch1->children[0]->parent = ch1;

			insertLineReference(ch1->children[0]->token, ch1->children[0]);			
			sem_chk_unary(ch1);

			sub_root->children[1] = ch1;
			ch1->parent = sub_root;
			sub_root = sub_root->children[1];

			parseNode = parseNode->children[3];
		}
	}

	return root;
}


TR_NODE addChildtoParseNode(TR_NODE head_of_new_parent, TR_NODE new_child) {
//helper function for make expression node
	int parent_children = head_of_new_parent->num_children;
	int i;
	TR_NODE old_children[30];
	for (i=0; i<parent_children; i++) {
		old_children[i] = head_of_new_parent->children[i];
		// free(head_of_new_parent->children[i]);
	}
	// RE-ASSIGN THE NUMBER OF CHILDREN
	head_of_new_parent->children = (TR_NODE *)malloc(sizeof(TR_NODE)*(parent_children+1));
	for (i=1; i<=parent_children; i++) {
		head_of_new_parent->children[i] = old_children[i-1];
		head_of_new_parent->children[i]->parent = head_of_new_parent;
	}
	head_of_new_parent->children[0] = new_child;
	head_of_new_parent->children[0]->parent = head_of_new_parent;
	head_of_new_parent->num_children +=1;
	return head_of_new_parent;
}

 
TR_NODE makeExpressionNode_slave(TR_NODE cur_ast_node, int child_num) {
	
	// BASE CASE
	int i;
	if(cur_ast_node->is_leaf == nt){
		if (cur_ast_node->gr_rule == 88 || cur_ast_node->gr_rule == 89 || cur_ast_node->gr_rule == 78){
			// IF lhs of my rule becomes value or var, return as the value will be evaluated later
			//printf("vexpr debug: alue base case returns\n");

			return cur_ast_node;
		}
	}
	else {
		if ((cur_ast_node->is_leaf == eps) || (cur_ast_node->token.type == find_hash(tnt_hash, "RPARA")) || (cur_ast_node->token.type == find_hash(tnt_hash, "LPARA"))) {
			//printf("vexpr debug: eps lpara returns\n");
			
			return NULL;
		}
		if((contains(operator_symbols, operator_symbols_size,cur_ast_node->token.type) == 1) || (contains(primitive_types_list, primitive_types_size, cur_ast_node->token.type) == 1)) {
			//printf("vexpr debug: operator / prim type return\n");
			
			return cur_ast_node;
		}		
	}
		
	int cur_children = cur_ast_node->num_children;

	// right most traversal
	for (i=cur_ast_node->num_children-1; i>=0; i--) {
		
		if(makeExpressionNode_slave(cur_ast_node->children[i], i) == NULL){
					if (i < cur_ast_node->num_children-1) {
						int j=i;
						while (j < cur_ast_node->num_children-2) {
							cur_ast_node->children[j] = cur_ast_node->children[j+1];
							j+=1;
						}
					}
					cur_ast_node->num_children -= 1;
					if (cur_ast_node->num_children == 0){
						return NULL; 			//propagate null
					}
		}
		else {
					cur_ast_node->children[i]->parent = cur_ast_node;
		}
	}
	cur_children = cur_ast_node->num_children;
	//ALL CHILDREN HAVE BEEN REDUCED. ANY POSSIBLE REARRANGING OF CHILDREN DONE. CAUTION: RULES MAY CHANGE!
	if (cur_children == 1) { 													// REPLACE PARENT's child by current node's child 0
		
		cur_ast_node->parent->children[child_num] = cur_ast_node->children[0];
		cur_ast_node->children[0]->parent = cur_ast_node->parent;
	}
	else if (contains(second_op_rules, second_op_size, cur_ast_node->gr_rule) == 1) {
		if (cur_ast_node->num_children == 3) {

			cur_ast_node->children[1] = addChildtoParseNode(cur_ast_node->children[2], cur_ast_node->children[1]);
			cur_ast_node->children[0] = addChildtoParseNode(cur_ast_node->children[0], cur_ast_node->children[1]);
			cur_ast_node->children[2] = NULL;
			cur_ast_node->children[1] = NULL;
			cur_ast_node->num_children = 1;

			cur_ast_node->parent->children[child_num] = cur_ast_node->children[0];
			cur_ast_node->parent->children[child_num]->parent = cur_ast_node->parent;
		}
		else {			// 2 children
			//printf("vexpr debug: 2 children with op on rhs: %d\n", cur_ast_node->gr_rule);

			cur_ast_node->children[0] = addChildtoParseNode(cur_ast_node->children[0], cur_ast_node->children[1]);
			cur_ast_node->children[1] = NULL;
			//free
			cur_ast_node->num_children = 1;

			cur_ast_node->parent->children[child_num] = cur_ast_node->children[0];
			cur_ast_node->children[0]->parent = cur_ast_node->parent;		
		}
	}
	else if (cur_children == 2) {
		// if gr_rule number has op on rhs
		//printf("vexpr debug: 2 children with op on lhs only: %d\n", cur_ast_node->gr_rule);

		cur_ast_node->children[0] = addChildtoParseNode(cur_ast_node->children[1], cur_ast_node->children[0]);
		cur_ast_node->children[1] = NULL;
		cur_ast_node->num_children = 1;

		cur_ast_node->parent->children[child_num] = cur_ast_node->children[0];
		cur_ast_node->children[0]->parent = cur_ast_node->parent;	
	}
	else {
		// ERROR
	}
	//printf("vexpr debug: returning: %d\n", cur_ast_node->token.type);

	return cur_ast_node->children[0];
}

AST_NODE convertParseToAST(TR_NODE parse_head) {
	// get type from grammar rule number
	// if type is addSub' or mulDiv' -> operator is on left side
	AST_NODE ast_head = NULL;
	
	//printParse(parse_head);
	ast_head = parseToAst(parse_head);
	return ast_head;
}

AST_NODE parseToAst(TR_NODE cur_parse_node) {
	// BASE CASE
	if (cur_parse_node == NULL || cur_parse_node->num_children == 0) {
		//printf("\nNULL call in parseToAst\n");
		return NULL;
	}

	AST_NODE cur_ast_node;
	int symbol = cur_parse_node->token.type;	

	if (contains(operator_symbols, operator_symbols_size, symbol) == 1) {
		//printf("\noperator  %s\n", tnt_arr[symbol]);
		cur_ast_node = makeNewAstNode(cur_parse_node, op, false, cur_parse_node->num_children);
	}
	else if (contains(primitive_types_list, primitive_types_size, symbol) == 1) {
		// FOR TYPECASTING NODES
		//printf("\ntypecast %s", tnt_arr[symbol]);
		cur_ast_node = makeNewAstNode(cur_parse_node, dataType, false, 1);
	}
	else if (cur_parse_node->gr_rule == 78) {							// GRAMMAR RULE
		//printf("\n shuchita varNode %s", tnt_arr[symbol]);

		cur_ast_node = makeVarNode(cur_parse_node);
	}
	else if (cur_parse_node->gr_rule == 88 || cur_parse_node->gr_rule == 89){
		//printf("\n value node %s", tnt_arr[grammar[cur_parse_node->gr_rule].lhs]);
		//printf("\n shuchita varNode %s case 89 \n", tnt_arr[symbol]);
		cur_ast_node = makeValueNode(cur_parse_node);
		/*if (strcmp(cur_ast_node->token.str, "barr") == 0) {
			printf("check barr ast after make value node\n");
			char fun[20]; gets(fun);
			printAST(cur_ast_node, 0);
		}*/
		//printf("type of ast node: %s, num_children %d", tnt_arr[cur_ast_node->token.type], cur_ast_node->num_children);
	}

	int i;
	if(cur_ast_node->type != variable && cur_ast_node->type != functionCall) 
	{
		for (i=0; i < cur_ast_node->num_children; i++) {
			//printf("\nPArentofying child %d of %s\n", i, tnt_arr[symbol]);
			parentify(cur_ast_node, i, parseToAst(cur_parse_node->children[i]));
			/*cur_ast_node->children[i] = parseToAst(cur_parse_node->children[i]);
			cur_ast_node->children[i]->parent = cur_ast_node;*/
		}
	}

	return cur_ast_node;
}

AST_NODE makeExpressionNode(TR_NODE parseNode) {

	/*AST_NODE expr_head;
	TR_NODE cur_ast_node = parseNode;*/
	TR_NODE old = parseNode->parent;
	TR_NODE dummy_node = getNewNode(nt);
	dummy_node->num_children = 1;
	dummy_node->children = (TR_NODE *)malloc(sizeof(TR_NODE));
	
	dummy_node->children[0] = parseNode;
	parseNode->parent = dummy_node;
	//TR_NODE parse_expr_head = makeExpressionNode_slave(parseNode, 0);
	makeExpressionNode_slave(parseNode, 0);
	TR_NODE parse_expr_head = dummy_node->children[0];
	//printf("\n\n\n\ndlsve returns: %s \n\n\n\n", tnt_arr[parse_expr_head->children[0]->token.type]);
	//printf("Printing the parse tree.....\n");
	//printParse(parse_expr_head);
	//printf("\n\n%d, children : %d\n\n", parse_expr_head->is_leaf, parse_expr_head->num_children);

	parse_expr_head->parent = old;

	
	/*expr_head = */
	AST_NODE tempExp =  convertParseToAST(parse_expr_head);
	/*if (strcmp(tempExp->token.str, "barr") == 0) {
			printf("check barr ast in expression\n");
			char fun[20]; gets(fun);
			printAST(tempExp, 0);
	}*/
	int fl = 0;
	//printf("before exr\n");
	sem_chk_expression(tempExp, &fl);
	//printf("after exr\n");

	return tempExp;
}

void print_values_p(TR_NODE p){
	
	int num;	//printf("callled for rule numver [%d]\t", num);
	if(!p) return;
	if(p->is_leaf == nt){
		num = p->gr_rule;
		num = grammar[num].lhs;
	}
	else if( p->is_leaf == eps) {
		num = find_hash(tnt_hash, "EPS");
		printf("EPS");		
	}
	else{
		num = p->token.type;
		printf("its a terminal =>  %d ", num);
	}

	printf("[%s]\t", tnt_arr[num]);
}

void printParse(TR_NODE ptr){
	int i;
	if(!ptr) return;
	print_values_p(ptr);
	for(i=0; i < ptr->num_children; i++)
	{
		//printf("No of children : %d", ptr->num_children);
		//printf("Going to child %d", i); 
		printParse(ptr->children[i]);
		char fun[10];
		gets(fun);
	}
}

void print_values_a(AST_NODE p, int sp){
	
	int num;
	sp = sp*4;
	while(sp-->0){
		printf("   ");
	}
	if(p){
		if(p->isDummy == false){
		num =  p->token.type;	
		printf("Type: %s, token str: %s type: [%s]\t", typeStr[p->type],p->token.str, tnt_arr[num]);
		} 
		else 
		printf("dummy [%s]\t", typeStr[p->type]);
	}
	else
	{
		printf("(NULL)\n");
	}
	
	//printf("callled for rule numver [%d]\t", num);
}

	
/*int main(){
	TR_NODE parseTree = makeParseTree();

	if (ERROR_FLAG == 0) {
		printf("\n\n Parse tree successfully created ....\n\n\n");
		AST_NODE ast= makeAST(parseTree);
		printf("\n");
		printAST(ast, 0);
	}
	else {
		printf("\n\nABORTING DUE TO LEXICAL / PARSE ERRORS. \n");
	}
	return 0;
}
*/