/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "semantic_check.h"

// sizes of variable types
int INT_SIZE = 2;
int FLOAT_SIZE = 8;

// SCOPE RESOLUTION TABLE
scopeResTableEntry *scopeResTable = NULL;  		/* DYNAMIC ARRAY THAT GROWS WHEN NUMBER OF CHILDREN > 75% */
int scopeResTable_size = 20;
int scopeResTable_current = 0;
int scopeOffset = 100;
int code_scope = 0;

// Structure for storing errors
Error_vars error_flags;

// HEAD AND CURRENT SCOPE POINTERS
SYM_NODE *head_scope = NULL;
SYM_NODE cur_scope = NULL;

int uid = 0;

// structure for storing attributes of variables declared
VAR_ATTR var_attr_table;
int attr_table_size = 30;

typeExtract makeNewTypeExtract(int type, int isArray, int b1, int b2) {
	typeExtract temp = (typeExtract)malloc(sizeof(struct _typeExtract));
	temp->type = type;
	temp->isArray = isArray;
	temp->bound1 = b1;
	temp->bound2 = b2;

	return temp;
}

void initialiseSymbolTable() {
	scopeOffset = 100;
	var_attr_table = (VAR_ATTR)malloc(sizeof(struct var_attributes)*attr_table_size);
	scopeResTable = (scopeResTableEntry *)(malloc(sizeof(scopeResTableEntry)*scopeResTable_size));
	initialiseSemChk();
	addNewScope();
}

int hashSymTable(char str[30], int prime) {
	int c=0;
	int sum=0;

	int len = strlen(str);
	//printf("len: %d\n", len);
	while (c<len) {
		sum = sum + (int)(str[c]*(c+1));
		c+=1;
	}

	return sum % prime;
}

void doubleScopeResTable() {
	if (scopeResTable_size == 0) {
		scopeResTable = (scopeResTableEntry *)(malloc(sizeof(scopeResTableEntry)*5));
	}
	else {
		//realloc(scopeResTable, scopeResTable_size*2);
		scopeResTable_size *= 2;
		//scopeResTable = (scopeResTableEntry *)(sizeof(scopeResTableEntry)*5);	
	}
};

int getPrimeNumber(int depth) {
	int a[] = {13, 11, 7, 5};
	if (depth < 4) {
		return a[depth];
	}	

	return a[3];
}

void increaseNumChildrenInScope(SYM_NODE cur_scope) {
	/* INCREASES THE NUMBER OF CHILDREN OF CURRENT SCOPE */
	if (cur_scope->active_child > cur_scope->num_children*0.75) {
		//	DYNAMIC ARRAY => DOUBLES WHEN 75% FILLED
		// realloc(cur_scope->children, cur_scope->num_children*2);
	}
}

void addNewScope() {
	// THIS FUNCTION MAKES A NEW SCOPE AS THE CHILD OF CUR_SCOPE
	// AND PUTS THE NEWLY CREATED SCOPE AS THE CUR_SCOPE

	printf("\n************Adding new Scope\n**********");
	SYM_NODE temp = (SYM_NODE)malloc(sizeof(struct sym_table_node));
	temp->num_children = 5;
	temp->children = (SYM_NODE *)malloc(sizeof(SYM_NODE)*(temp->num_children));
	temp->parent = cur_scope;
	if (cur_scope == NULL) { 
		temp->depth = 0;
	} else {
		temp->depth = cur_scope->depth+1;
		//printf("depth received: %d\n", temp->depth);
	}
	temp->active_child = 0;
	temp->buckets = (HASH_VAR *)malloc(sizeof(HASH_VAR)*getPrimeNumber(temp->depth));
	// NOTE: THE number of buckets in hash table is variable
	printf("depth of table: %d\n", temp->depth);

	if (head_scope == NULL) {
		cur_scope = temp;
		//printf("head_scope is NULL: %d\n", head_scope == NULL);
		head_scope = &cur_scope;
		return;
	}
	cur_scope->children[cur_scope->active_child] = temp;
	cur_scope->active_child += 1;

	increaseNumChildrenInScope(cur_scope);

	cur_scope = temp;		// CHANGE THE CURRENT SCOPE TO THE NEWLY MADE SCOPE

	//printf("leaving\n");
}

void gotoPrevScope() {
	//printf("going to parent scope\n");
	//printCurScope(cur_scope);
	cur_scope = cur_scope->parent;
}

int lookupScopeResTable(char str[]) {
	/*
	RETURNS THE SCOPERESTABLE ENTRY 
	*/
	int fn_hash = hashSymTable(str, BUCKET_PRIME);
	int i=0;

	while (i < scopeResTable_current) {
		if (scopeResTable[i].hash == fn_hash) {
			//printf("%s\n", scopeResTable[i].fn_name);
			if (strcmp(scopeResTable[i].fn_name, str) == 0) {
				return i;
			}

		}
		i+=1;
	}
	return -1;
}

void printScopeResTable() {
	int i;
	printf("Functions present in scope resolution table.\n");
	for (i=0; i<scopeResTable_current; i++) {
		printf("f: %s, ret: %d, params:%d\n", scopeResTable[i].fn_name, scopeResTable[i].no_of_return_vals, scopeResTable[i].no_of_params);
	}
	printf("****************** END OF TABLE ******************** \n");
}

void insertFunction(char fn_name[30], int no_of_return_vals, int no_of_params, AST_NODE ast_node_ptr) {
	/* FUNCTION DOES 2 THINGS:
	1. INSERT AN ENTRY IN THE SCOPE RESOLUTION TABLE FOR NEW FUNCTION
	2. MAKE A NEW SCOPE NODE FOR THE FUNCTION AND MAKE IT CHILD OF GLOBAL SCOPE NODE. 
	*/
	
	//scopeResTableEntry *already;
	printf("\n***************\nInserting new function in scope res table: %s\n************\n", fn_name);
	printf("ret:%d, p:%d\n", no_of_return_vals, no_of_params);
	int temp = lookupScopeResTable(fn_name);
	if (temp != -1) {
		printf("Function: %s already defined earlier.\n", fn_name);
		return;
	}
	// STEP 1: CREATE NEW ENTRY IN SCOPE RESOLUTION TABLE
	//scopeResTable[temp] = scopeResTable[scopeResTable_current];

	scopeResTable[scopeResTable_current].fn_name = (char *)malloc(sizeof(char)*(strlen(fn_name)));
	strcpy(scopeResTable[scopeResTable_current].fn_name, fn_name);
	scopeResTable[scopeResTable_current].hash = hashSymTable(fn_name, BUCKET_PRIME);

	scopeResTable[scopeResTable_current].no_of_params = no_of_params;
	//scopeResTable[scopeResTable_current].paramTypes = (malloc)(sizeof(int)*no_of_params);

	scopeResTable[scopeResTable_current].no_of_return_vals = no_of_return_vals;
	scopeResTable[scopeResTable_current].returnValTypes = (malloc)(sizeof(int)*no_of_return_vals);
	int i;
	AST_NODE rets = ast_node_ptr->children[0];
	printf("%d, children: %d\n", rets->type, rets->num_children);
	for (i=0; i<no_of_return_vals; i++) {
		//printf("here\n");
		scopeResTable[scopeResTable_current].returnValTypes[i] = rets->children[i]->token.type;
		printf("%s \n", tnt_arr[scopeResTable[scopeResTable_current].returnValTypes[i]]);
	}

	scopeResTable[scopeResTable_current].ast_node_ptr = ast_node_ptr;
	
	//printf("current: %d\n", scopeResTable_current);
	
	printf("inserted - ret: %d, p: %d, current: %d\n", scopeResTable[scopeResTable_current].no_of_return_vals, scopeResTable[scopeResTable_current].no_of_params, scopeResTable_current);

	if (scopeResTable_current >= 0.7*scopeResTable_size) {
		//doubleScopeResTable();
	}
	
	// STEP 2: CREATE NEW SCOPE NODE AND MAKE IT CHILD OF GLOBAL
	/*SYM_NODE temp1 = (SYM_NODE)malloc(sizeof(struct sym_table_node));;
	temp1->num_children = 5;
	temp1->children = (SYM_NODE *)(malloc(sizeof(SYM_NODE)*(temp1->num_children)));
	temp1->parent = head_scope;
	temp1->depth = 1;
	temp1->active_child = 0;
	temp1->buckets = (HASH_VAR *)(malloc(sizeof(HASH_VAR)*getPrimeNumber(1)));
	
	head_scope->children[head_scope->active_child+1] = temp1;
	head_scope->active_child+=1; 
	*/
	
	scopeResTable[scopeResTable_current].sym_node_ptr = cur_scope;
	// SEE IF NUMBER OF CHILDREN NEEDS TO BE INCREASED IN HEAD SCOPE AFTER ADDING THE NEW CHILD
	increaseNumChildrenInScope(head_scope);

	scopeResTable_current+=1;
	printScopeResTable();

	//uid+=1;
}

int getNoOfReturnValues(char str[]) {
	int fn_entry;
	fn_entry = lookupScopeResTable(str);

	return scopeResTable[fn_entry].no_of_return_vals;	
}

int getNoOfParams(char str[]) {
	int fn_entry;
	fn_entry = lookupScopeResTable(str);

	return scopeResTable[fn_entry].no_of_params;
}

int getFunctionId(char str[]) {
	int fn_entry;
	fn_entry = lookupScopeResTable(str);	

	return fn_entry;
}

SYM_NODE getScopeNode(char str[]) {
	int fn_entry = lookupScopeResTable(str);

	return scopeResTable[fn_entry].sym_node_ptr;		
}

SYM_NODE changeScope(char str[]) {
	int fn_entry = lookupScopeResTable(str);
	cur_scope = scopeResTable[fn_entry].sym_node_ptr;

	return scopeResTable[fn_entry].sym_node_ptr;		
}

AST_NODE getASTNode(char str[]) {
	int fn_entry = lookupScopeResTable(str);

	return scopeResTable[fn_entry].ast_node_ptr;		
}

/******************************** hashtable *************************************/

void growAttrTable() {

}

int insertInSymbolTable(int dataType, TK token, int arrBound1, int arrBound2) {
	if (findInSymbolTable(token.str, 1) != -1) {
		//error
		//printf("\nRedeclaration of symbol: %s in line number: %d which is already inserted in current scope.\n", token.str, token.lineNo);
		//char fun[20];
		//gets(fun);
		setErrorFlags(variableRedeclaration, token.str, token.lineNo);
		semantic_error();
		return -1;
	}
	HASH_VAR *sym_hashtbl = cur_scope->buckets;
	return insertAtHead_hashtbl(sym_hashtbl, dataType, token, arrBound1, arrBound2);
}

int insertAtHead_hashtbl(HASH_VAR *sym_hashtbl, int dataType, TK token, int arrBound1, int arrBound2) {
	/* STEPS:
	1. MAKE A HASHTABLE _scEntry		-> 	INSERT AT HEAD OF BUCKET
	2. MAKE AN ATTRIBUTE TABLE ENTRY 	->	
	*/
	HASH_VAR temp = (HASH_VAR)(malloc(sizeof(struct hashtbl_entry_var)));
	if (temp == NULL)
		return 0;

	temp->id = (char *)malloc(sizeof(char)*(strlen(token.str)+1));
	strcpy(temp->id, token.str);			//	
	temp->hash = hashSymTable(temp->id, BUCKET_PRIME);		//
	temp->type = dataType;					// datatype is an integer in tnt_hash

	// ToDo: add a reg field here, that stores the register where varaible is - 0, 1 , 2, 3, -1
	temp->uid = uid;
	temp->next = NULL;
	
	int index = hashSymTable(token.str, getPrimeNumber(cur_scope->depth));
	
	// ATTRIBUTE TABLE ENTRY
	var_attr_table[uid].line_decl = token.lineNo;
	var_attr_table[uid].size1 = arrBound1;
	var_attr_table[uid].size2 = arrBound2;
	var_attr_table[uid].line_ref = (int *)malloc(sizeof(int)*20);
	var_attr_table[uid].modRef = (int *)malloc(sizeof(int)*20);
	//printf("uid: %d, offset: %d", uid, scopeOffset);
	var_attr_table[uid].offset = scopeOffset;
	//printf("offset just ins: %d\n", var_attr_table[uid].offset);
	growAttrTable();

	int h;
	/*for (h=0; h<=uid; h++) {
		printf("offset of %d, %d\n", h, var_attr_table[h].offset);
	}*/
	if (strcmp(tnt_arr[dataType], "INT")==0 || strcmp(tnt_arr[dataType], "BOOLEAN")==0) {
		if (arrBound2 > 0) {
			scopeOffset += INT_SIZE * arrBound1*arrBound2;
			//printf("adding to scopeOffset: %d\n", INT_SIZE * arrBound1*arrBound2);
		}
		else if (arrBound1 > 0) {
			scopeOffset += INT_SIZE * arrBound1;	
			//printf("adding to scopeOffset: %d\n", INT_SIZE * arrBound1);
		}
		else {
			//printf("incremented scope offset by %d\n", INT_SIZE);
			scopeOffset += INT_SIZE;
			//printf("adding to scopeOffset: %d\n", INT_SIZE);
		}
	}
	else if (strcmp(tnt_arr[dataType],"FLOAT")==0) {
		if (arrBound2 > 0) {
			scopeOffset += FLOAT_SIZE * arrBound1 * arrBound2;
		}
		else if (arrBound1 > 0) {
			scopeOffset += FLOAT_SIZE * arrBound1;	
		}
		else {
			scopeOffset += FLOAT_SIZE;
		}
	}
	else if (strcmp(tnt_arr[dataType], "POINT")==0) {
		scopeOffset += 0;
		//scopeOffset += 4+4;
	}
	else if (strcmp(tnt_arr[dataType], "BOT")==0) {
		scopeOffset += 0;
		//scopeOffset += 4+4+4+4;
	}

	// INSERT AT HEAD OF THE NECESSARY BUCKET IN HASHTABLE
	temp->next = sym_hashtbl[index];
	sym_hashtbl[index] = temp;

	printf("Symbol table Inserting var: %s in symbol table, defined in line Number: %d, of datatype: %s, uid=%d, bounds=[%d, %d]\n\n", temp->id, token.lineNo, tnt_arr[temp->type], temp->uid, var_attr_table[uid].size1, var_attr_table[uid].size2);
	uid+=1;
	return 1;
}


void growLineRefArray(int *arr, int size) {
	
	if (size % 5 == 0) {
		//realloc(arr, size*2);
	}
}

int getOffset(int uid) {
	//printf("finding offset for: %d\n", uid);
	//printf("globalOffset: %d\n", scopeOffset);

	//printf("var_attr: %d\n", var_attr_table[2].offset);
	return var_attr_table[uid].offset;
	//return findInSymbolTable(arr, 2);
}

int getUID(char arr[]) {
	return findInSymbolTable(arr, 1);
}

void insertLineReference(TK token, AST_NODE cur_node) {
	/* STEPS:
	1. LOOKUP IN SYMBOL TABLE 				->		RETURNS THE UID
	2. FETCH ATTR TABLE ENTRY WITH UID 		->		
	3. EDIT LINE NUMBERS					->		
	*/
	int modRef = 1;
	if (IS_DECL == 1) {
		return;
	}
	if (IS_ASSIGN == 1) {
		modRef = 0;
	}

	int _type = findInSymbolTable(token.str, 0);
	cur_node->type_extract = makeNewTypeExtract(0,0,0,0);
	cur_node->type_extract->type = _type;

	int uid;	
	uid = findInSymbolTable(token.str, 1);
	//printf ("UID inserting line ref : %d, finding: %s\n", uid, token.str);
	// ERROR - REFERENCED BEFORE DECLARATION
	//symTableError("", token);

	if (uid == -1) {
		//printf("********************** ERROR *****************\n");

		setErrorFlags(referencedBeforeDecl, token.str, token.lineNo);

		semantic_error();
		return;
	}

	cur_node->type_extract->bound1 = var_attr_table[uid].size1;
	cur_node->type_extract->bound2 = var_attr_table[uid].size2;
	if (var_attr_table[uid].size1 == 0 && var_attr_table[uid].size2 == 0) {
		cur_node->type_extract->isArray = 0;
	}
	else if (var_attr_table[uid].size1 != 0 && var_attr_table[uid].size2 == 0) {
		cur_node->type_extract->isArray = 1;
	}
	else {
		cur_node->type_extract->isArray = 2;
	}
	sem_chk_array(cur_node);

	// SEMANTIC TYPE CHECKING 

	//if (var_attr_table[uid].line_ref_num > 0 && var_attr_table[uid].line_ref[var_attr_table[uid].line_ref_num-1] != token.lineNo) {
		var_attr_table[uid].line_ref[var_attr_table[uid].line_ref_num] = token.lineNo;
		var_attr_table[uid].modRef[var_attr_table[uid].line_ref_num] = modRef;
		var_attr_table[uid].line_ref_num += 1;
	/*}
	else if (var_attr_table[uid].line_ref_num == 0) {
		var_attr_table[uid].line_ref[var_attr_table[uid].line_ref_num] = token.lineNo;
		var_attr_table[uid].modRef[var_attr_table[uid].line_ref_num] = modRef;
		var_attr_table[uid].line_ref_num += 1;
	}
*/

	//growLineRefArray(var_attr_table[uid].line_ref, var_attr_table[uid].line_ref_num);
	//printf("out of insertLineRef\n");
}

int findInSymbolTable(char str[], int typeOrUID) {
	
	int str_hash = hashSymTable(str, BUCKET_PRIME);
	return lookup_SymTable(cur_scope, str, str_hash, typeOrUID);
}

int getTypeFromSymTable(char str[]) {
	int uid;
	return findInSymbolTable(str, 0);
}

int find_list(HASH_VAR cur, char str[], int hashVal, int typeOrUID) {
	/* FINDS AN ELEMENT IN A LIST OF HASH_VAR (variable hashtable bucket entries)
	   RETURNS:
	   1 	->		ENTRY FOUND 
	   -1 	->		ENTRY NOT FOUND IN LIST
	*/
	
	while(cur != NULL) {
		if (cur->hash == hashVal) {
			if (strcmp(cur->id, str) == 0) {
				if (typeOrUID == 1) {
					return cur->uid;
				}
				else if (typeOrUID == 0) {
					return cur->type;
				}
				/*else if (typeOrUID == 2){
					return cur->offset;
				}*/
			}
		}
		cur = cur->next;
	}
	return -1;
}

int lookup_SymTable(SYM_NODE lookupScope, char str[], int str_hash, int typeOrUID) {
	/*
	RECURSIVE FUNCTION THAT FINDS AN ENTRY IN A HASHTABLE OF CURRENT SYM_NODE
	IF NOT FOUND, LOOKS IN SYMBOL TABLE OF PARENT SYM_NODE
	
	TO FIND AN ELEMENT:
	1. HASH THE STRING USING THE PRIME NO ACCORDING TO DEPTH OF SYM_NODE
	2. INTRA BUCKET HASH 	->		LOOK IN THE BUCKET BY MATCHING HASH OF ENTRY WITH STR_HASH USING PRIME NO - BUCKET_PRIME
	3. IF STR_HASH MATCHES, THEN MATCH THE STRING

	RETURNS:
	1: 	ENTRY FOUND
	-1:	ENTRY NOT FOUND
	*/
	HASH_VAR *sym_hashtbl = lookupScope->buckets; 

	int index = hashSymTable(str, getPrimeNumber(lookupScope->depth));
	
	int cur_lookup = find_list(sym_hashtbl[index], str, str_hash, typeOrUID);
	if (cur_lookup != -1) {
		return cur_lookup;
	}
	else if (lookupScope->parent != NULL) {
		//printf("not found %s at depth $$$$$$$$$$$$$$$ : %d\n", str, lookupScope->depth);
		return lookup_SymTable(lookupScope->parent, str, str_hash, typeOrUID);
	}
	return -1;
}

void printCurScope(SYM_NODE scope) {
	int i=0;
	if (scope == NULL) {
		printf("NULL SCOPE%s\n");
	}
	int tbl_size = getPrimeNumber(scope->depth);
	printf("\n***************** CURRENT SCOPE ************\n");
	printf("table size: %d, scope depth: %d\n", tbl_size, scope->depth);
	
	while (i<tbl_size) {
		printf("Bucket number: %d\n", i);
		if (scope->buckets[i] == NULL) {
			i+=1;
			continue;
		}
		HASH_VAR cur = scope->buckets[i];		
		while (cur != NULL) {
			printf("%d, %s = %s -> ", cur->uid, cur->id, tnt_arr[cur->type]);
			cur = cur->next;
		}
		i+=1;
		printf("\n");
	}
	printf("\n***************** END OF SCOPE ************\n");
}
	
int main() {
	int choice;

	printf("Enter the choice:\n1. Do Semantic Analysis and Type Checking.\n2.Do Code Generation (8086)\n");
	scanf("%d", &choice);
	//char c;
	//getc(&c);
	if (choice <0 || choice > 2) {
		printf("Invalid choice. Exiting.\n");
		exit(0);
	}

	TR_NODE parseTree = makeParseTree();
	initialiseSymbolTable();
	initialise_codeGen();
	
	if (ERROR_FLAG == 0) {
		printf("\n\n Parse tree successfully created ....\n\n\n");
		AST_NODE ast= makeAST(parseTree);

		//printAST(ast, 0);
		//printf("Head Scope:: \n\n");
		//printCurScope(*head_scope);

		//printf("head children: %d, %d\n", (*head_scope)->num_children, (*head_scope)->active_child);		

		//printf("Scope after manipulation:: \n\n");
		//printCurScope(cur_scope);		

		finaliseSemChk();

		resetScope(*head_scope);
		cur_scope = (*head_scope)->children[0];
		
		if (ERROR_FLAG == 0 && choice == 2) {
			printf("Converting to .asm file\n");
			ASTtoCode(ast);
		}
		else {
			reopen_asm(choice);

			if (choice == 2) {
				printf("\n\nABORTING DUE TO SEMANTIC ERRORS. Check errors.txt\n");
			}
		}
	}
	else {
		printf("\n\nABORTING DUE TO LEXICAL / PARSE ERRORS. \n");
	}

	/*printf("returns: %d\n", scopeResTable[0].no_of_return_vals);
	printf("ret0: %s\n", tnt_arr[scopeResTable[0].returnValTypes[0]]);
	printf("ret1: %s\n", tnt_arr[scopeResTable[0].returnValTypes[1]]);*/
	//printScopeResTable();

	//printCurScope(cur_scope);
	return 0;
}

void resetScope(SYM_NODE scope) {
	SYM_NODE temp = scope;
	temp->num_children = temp->active_child;
	temp->active_child = 0;

	int i;
	for (i=0; i<temp->num_children; i++) {
		resetScope(temp->children[i]);
	}
}

void code_gotoNextScope() {
	printf("going to next scope\n");
	cur_scope = cur_scope->children[cur_scope->active_child];
	cur_scope->parent->active_child += 1;
}

void code_gotoParentScope() {
	printf("going to parent scope\n");

	if (cur_scope->parent != NULL) {
		cur_scope = cur_scope->parent;
	}
}
/*
int main() {
	
	Test case:

	Program 
	- int e, n, v
		- f1(a, b, c)
			- j, cycle, tank
		- f2()
			- train, w, e
		- f3(q, r)
			- lorry, jeep
		- f4(m, n, o)
		- main
			- car, bus, truck

	
	initialiseSymbolTable();
	TK token;
	token.type = 101;
	strcpy(token.str, "x");
	token.lineNo = 2;

	addNewScope();			// adds program
	insertInSymbolTable(113, token, 2, 2);
	strcpy(token.str, "j");
	token.lineNo = 5;

	insertInSymbolTable(113, token, 2, 1);
	strcpy(token.str, "van");
	token.lineNo = 8;

	insertInSymbolTable(113, token, 2, 1);
	strcpy(token.str, "i");
	
	insertInSymbolTable(113, token, 2, 1);
	strcpy(token.str, "i");
	
	insertInSymbolTable(113, token, 3, 1);
	printCurScope(cur_scope);

	char f1[] = "Function1";
	char f2[] = "carFunc";
	char f3[] = "oldFunction";
	AST_NODE n1 = NULL;
	insertFunction(f1, 2, 3, n1);
	strcpy(token.str, "cycle");
	insertInSymbolTable(113, token, 2, 2);
	strcpy(token.str, "tank");
	insertInSymbolTable(113, token, 2, 2);
	strcpy(token.str, "j");
	insertInSymbolTable(113, token, 2, 2);
	printCurScope(cur_scope);

	gotoPrevScope();
	printCurScope(cur_scope);

	insertFunction(f2, 1, 0, n1);
	insertFunction(f2, 1, 0, n1);
	insertFunction(f3, 1, 0, n1);
	//printScopeResTable();
}*/