/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

const int BLOCK_SIZE = 10;

// GRAMMAR RULES
GRULE *firsts;
int num_firsts;

GRULE *grammar;
int num_rules;

int last_stack_rem = 0;
// FIRST SETS
int ERROR_FLAG = 0;

// FOLLOW SETS
char ch;
GRULE *follows;
int num_follows;

// HASHTABLE STORING THE TERMINALS AND NON-TERMINALS
HASHTABLE tnt_hash;

// STACK FOR PUSHING THE GRAMMAR RULES
ST_NODE stack = NULL;

// NUMBER OF TERMINALS AND NON-TERMINALS
int noOfTerminals = 0;
int noOfNonTerminals = 0;

// ARRAY OF TERMINALS AND NON-TERMINALS
char tnt_arr[147][40];

// PARSE TABLE => STORES A 2-D ARRAY OF RULE NUMBERS
int **parseTable;

// PARSE TREE
PTREE parseTree;

// BUFFER FOR MAINTAINING THE TOKENS GIVEN BY LEXER
TK *tkList;
int token_buf[10];
int token_buf_pos;
int cur_tok;

// START SYMBOL
char start_symbol[12];
int EPS;
char eps_arr[5];

/*int getTokens(TK tkList[], int size) {

}*/

/* *******************************************************************************************************************************
						COMMON FUNCTION THAT GENERATES BOTH GRAMMAR RULES AND FIRST, FOLLOW SETS
*********************************************************************************************************************************/
void hash_grammar(char fileIp[], int rev, int grammar_or_first, GRULE **ruleset, int *nRules) {
	// left and right hand side strings
	//grammar_or_first = 1;
	char lhs_str[100];
	char rhs_str[300];					// buffer for reading current grammar rule right side
	int line_size = 0;					// position in the rhs_str
	int rhsCount = 0;

	// left and right hand sides hashed
	int lhs;
	int *rhs;
	char c;
	
	FILE *fp = fopen(fileIp, "r");			// open file for grammar rules

	// allocate space in heap for grammar rules
	fscanf(fp, "%d", nRules);		// assumes that first line of the file is the number of rules

	while ((c=fgetc(fp)) != '\n');

	*ruleset = (GRULE *)(malloc(sizeof(struct grammar_rule)*(*nRules)));

	int i, j, k, rule_no=0;			// count variables
	int flag = 0;
	int rulesParsed = 0; 
	
	while (rulesParsed < *nRules) {
		// read a grammar rule
		// LHS
		////printf("\nstart: %d, %d, parsed: %d\n", rule_no, *nRules, rulesParsed);
		line_size = 0;
		fscanf(fp, "%s", lhs_str);
		lhs = find_hash(tnt_hash, lhs_str);
		if (lhs == -1) {
			//printf("ERROR:::: %s\n", lhs_str);
			//exit(1);
		}
		if (grammar_or_first == 1) {
			rule_no = lhs;
		}
		//printf("R:%d\n", rule_no);

		rhsCount = 0;
		flag = 0;

		// RHS ==> read till \n is found into rhs_str buffer
		while ((c = fgetc(fp)) == ' ');
		ungetc(c, fp);

		while ((c = fgetc(fp)) != '\n') {
			if (c == ' ') {
				rhsCount++;
				flag = 1;
			}
			else {
				flag = 0;
			}

			rhs_str[line_size++] = c;
		}
		if (flag == 0) {
			rhsCount++;
		}
		/*if (rhsCount == 0) {
			//printf("*********************  ZERO COUNT  *******************\n");
			char fun[20];
			gets(fun);
		}*/
		//if (rule_no == 0)
		//printf("rule: %d, rhsCount = %d\n", rule_no, rhsCount);

		rhs_str[line_size++] = ' ';
		rhs_str[line_size] = '\0';
		//printf("rhs string: %s\n", rhs_str);


		j=0;
		char temp[40];									// stores current RHS token
		rhs = (int *)(malloc(sizeof(int)*rhsCount));	// stores hashes of rhs tokens
		
		// Find each rhs token from rhs_str and hash it into the array => rhs 
		for (i=0; i<rhsCount; i++) {
			k=0;										// i => token no. in rhs, j => index of rhs_str, k=> index of temp(current_token)
			while (rhs_str[j] != ' ') {
				////printf("%d %d\n", k, j);
				temp[k++] = rhs_str[j];
				j++;
			}
			temp[k] = '\0';
			j++;
			if (rev == 0) {
				////printf("temp: %s\n", temp);
				rhs[i] = find_hash(tnt_hash, temp);
				////printf("rhs[i]: %d\n", rhs[i]);
			}
			else {
				rhs[rhsCount - 1 - i] = find_hash(tnt_hash, temp);	
			}
		}

		/*if (rule_no == 99) {
			printf("RULE 99: *********************** \n");
			printf("lhs: %s\n", lhs_str);
			printf("%c %c %c %c\n", lhs_str[0], lhs_str[1], lhs_str[2], lhs_str[3]);
			printf("rhs: %s\n", rhs_str);
			printf("rhsCount: %d\n", rhsCount);
			printf("rhs[0]: %d\n", rhs[0]);
		}*/

		////printf("rhsCount: %d\n", i);

		// assign to the grammar rule the lhs and the rhs
		////printf("assign to struct\n" );
		((*ruleset)[rule_no]).lhs = lhs;
		////printf("one\n");
		((*ruleset)[rule_no]).rhs = rhs;
		((*ruleset)[rule_no]).rhs_len = rhsCount;

		//printf("rhsCount: %d %d\n", rhsCount, (*ruleset)[rule_no].rhs_len);
		int u;
		for (u=0; u < (*ruleset)[rule_no].rhs_len; u++) {
			////printf("rhs string: %d\n", (*ruleset)[rule_no].rhs[u]);

		}
		/*//printf("Rule %d => lhs: %d :: rhs:", rule_no, lhs);
		
		for (i=0; i<rhsCount; i++) {
			//printf("%d, ", rhs[i]);
		}
		//printf("\nrhs len: %d\n", rhsCount);
*/
		if (grammar_or_first == 0) {
			rule_no++;	
		}

		rulesParsed++;
		////printf("parsed: %d\n", rulesParsed);
		////printf("\ngrammar_or_first: %d\n", grammar_or_first);
		//fgetc(fp);										// skip the \n character
		//char fun[10];
		//gets(fun);		
	}
	////printf("out of loop\n");
}

void print_grule(int rule_num) {
	int i;
	printf("Grammar Rule %d => lhs: %d :: rhs:", rule_num, grammar[rule_num].lhs);

	int rhsCount = grammar[rule_num].rhs_len;
	for (i=0; i<rhsCount; i++) {
		printf("%d, ", grammar[rule_num].rhs[i]);
	}
	printf("\nrhs len: %d\n", grammar[rule_num].rhs_len);
}

void print_frule(int rule_num) {
	int i;
	printf("First set Rule %d => lhs: %d :: rhs:", rule_num, firsts[rule_num].lhs);

	int rhsCount = firsts[rule_num].rhs_len;
	if (rhsCount == 0) {
		printf("ZERO COUNT\n");
	}
	for (i=0; i<rhsCount; i++) {
		printf("%d, ", firsts[rule_num].rhs[i]);
	}
	printf("\nrhs len: %d\n", firsts[rule_num].rhs_len);
}

void print_forule(int rule_num) {
	int i;
	printf("Follow set Rule %d => lhs: %d :: rhs:", rule_num, follows[rule_num].lhs);

	int rhsCount = follows[rule_num].rhs_len;
	for (i=0; i<rhsCount; i++) {
		printf("%d, ", follows[rule_num].rhs[i]);
	}
	printf("\nrhs len: %d\n", follows[rule_num].rhs_len);
}

/* *******************************************************************************************************************************
									N-ARY TREE CREATION, INSERTION AND TRAVERSAL FUNCTIONS
*********************************************************************************************************************************/

TR_NODE getNewNode(leaf is_leaf)
{
	TR_NODE temp = (TR_NODE)malloc(sizeof(struct tr_node));	
	temp->children = NULL;	 
	temp->active_child = 0;
	temp->num_children = 0;
	temp->parent = NULL;
	temp->is_leaf = is_leaf;
	temp->gr_rule = -1;
	return temp;	
}

void initialiseRoot() {
	TR_NODE temp = getNewNode(nt);
	parseTree.root = temp;
	parseTree.cur = temp;

	// WHERE IS PROGRAM
}

TR_NODE goToNextChildOfParent() {					//VERIFY
//set cur to next node which has to be parsed. Should be whats on top of stack?
//returns current pointer -> NULL if end of parsing.
	//printf("dummy \n");
	TR_NODE ptr = parseTree.cur->parent;
	
	//printf("dummy2\n");
	//int gr = parseTree.cur->gr_rule;
	//int lhs = grammar[gr].lhs;
	//printf("went to parent %s, active_child=%d\n", tnt_arr[lhs], parseTree.cur->active_child);	
	int num;

	while(ptr && (++(ptr->active_child) == ptr->num_children)){
		//printf("goto parent children over\n");
		ptr = ptr->parent;
		//printf("done\n");
	}
	if(!ptr){								//reached end of parse tree! Nothing more to do! 
		//printf("ret null\n");
		return NULL;
	}
	else {
		num = ptr->active_child;
		//printf("active child: %d\n", ptr->active_child);
		parseTree.cur = ptr->children[num];
		//printf("child alloc\n");
		//int gr = parseTree.cur->gr_rule;
		//int lhs = grammar[gr].lhs;
		//printf("went to parent %s\n", tnt_arr[lhs]);
		return parseTree.cur;
	}

}

TR_NODE insert_Nary(int gr_rule) {
//insert rule rhs into parse tree
//returns current pointer -> NULL if end of parsing.
	//int EPS = 0;

	int i, num;
	TR_NODE par = parseTree.cur;
	num = grammar[gr_rule].rhs_len;
	par->num_children = num;
	par->children = (TR_NODE*)malloc(sizeof(TR_NODE)*num);	
	par->gr_rule = gr_rule;
	//printf("parent made with rule: %d\n", gr_rule);

	for (i=0; i<num; i++) {
		TR_NODE temp = 	getNewNode(nt);	
		temp->parent = par;		
		par->children[i] = temp;
		//printf("%dth child added\n", i);
	}
	//update current pointer and check if its eps rule : A -> EPS
	if(num==1 && grammar[gr_rule].rhs[0] == EPS){
		par->children[0]->is_leaf = eps;
		parseTree.cur = par->children[0];
		//printf("EPS added before calling goToNextChildOfParent.\n");
		return goToNextChildOfParent();
	}
	else {
		parseTree.cur = par->children[0];
		return parseTree.cur; 
	}
}

void insertTerminal(TK token) {
//Inserts a terminal into the tree, and progresses cur to next non-terminal or end of parse tree!
	//TR_NODE temp = getNewNode(t);
	TR_NODE temp = parseTree.cur;
	temp->token.type   = token.type;
	temp->token.num    = token.num;
	temp->token.rnum   = token.rnum;
	temp->token.lineNo = token.lineNo;
	strcpy(temp->token.str, token.str);
	temp->is_leaf = t;
	printf("%s", tnt_arr[token.type]);
	//temp->parent = parseTree.cur;
	//parseTree.cur[active_child] = temp;
	//return goToNextChildOfParent();
}



/* *******************************************************************************************************************************
									PARSE TABLE CREATION FUNCTIONS
*********************************************************************************************************************************/


void setParseTableEmpty() {
//Initialise parse table by setting it empty 
	int i,j;
	for (i=0; i<noOfNonTerminals; i++)
	{
		for(j=0; j<noOfTerminals; j++)
			parseTable[i][j] = -1;
	}
}

void getRuleRhs(int rhs[], int *revRhs, int num) {
//helper fn. Reverse and store rhs from grammar rule
	int i;
	for(i=0; i<num; i++)
	{
		rhs[i] = revRhs[num-i-1];
	}
}

void getFirst(int symbol, int first[], int *firstLen){
//Get first set corresponding to symbol 
	int l = firsts[symbol].rhs_len;
	//printf("The first set of symbol: %d is:\n", symbol);
	//print_frule(symbol);
	
	int *f = firsts[symbol].rhs;
	int i;
	//printf("The returned first set is:\n");
	for(i=0; i<l ; i++){
		first[i] = f[i];
		//printf("%d\t", first[i]);
	}
	//printf("\n");
	*firstLen = l;
}

int getTableValue(int x){
	return x-noOfNonTerminals;
}

void checkAndFill (int nt, int t, int ruleNo){
/*
Check if requested entry for parseTable[nt][t] is valid and fill. 
Possible clash in table indicated by this function.
*/
	//TODO: ITS NOT parseTable[NT][T], RIGHT? CHECK! and reset nt or t.
	t = getTableValue(t);
	//printf("inserting at nt: %d, t:%d, rule no: %d\n", nt, t, ruleNo);
	if(parseTable[nt][t] == -1 || parseTable[nt][t] == ruleNo){	
		parseTable[nt][t] = ruleNo;
	}
	else {
		printf("\nClash in parse table at entry [%d][%d], rule number prev: %d, rule number new: %d\n", nt, t, parseTable[nt][t], ruleNo);
	}
}

/*int contains(int list[], int size, int symbol) {
	// HELPER FUNCTION TO TEST IF A LIST CONTAINS AN INTERGER
	int i=0;
	while (i < size) {
		if (list[i] == symbol) {
			return 1;
		}
		i++;
	}

	return 0;
}*/

void fillFollow(int symbol, int ruleNo) {
/*
In pt's row corresponding to symbol, fill 'ruleNo' for all terminals in follow of symbol.
Used when ruleNo can generate epsilon. 
*/
	EPS = find_hash(tnt_hash, "EPS");

	//printf("Adding follow set.\n");
	int *follow   = follows[symbol].rhs;
	int followLen = follows[symbol].rhs_len;
	int i;
	for(i=0; i<followLen; i++) 
	{
		//printf("follow[i]: %d)\n", follow[i]);
		if (follow[i] != EPS) {
			checkAndFill(symbol, follow[i] ,ruleNo);
		} 
		else {
			//printf("Follow set can't be empty! Check symbol: %d\n", symbol);
		}
	}
}


void createParseTable(){
/*
Creates parsing Table for LL(1) grammar using first and follow sets.
Populate the table rulewise.
*/ 

	//printf("creating parse table.\n");
	//int EPS = 0;
	EPS = find_hash(tnt_hash, eps_arr);
	////printf("hash of: %s (EPS_arr) in createTable: %d\n", eps_arr, EPS);
	int i, ruleNo, pos;
	GRULE rule;
	bool epsFlag = false;
	int rhs[12], firstLen;			//assumed max rhs_len is 12
	int first[50]; //assume above
	char ch;

	setParseTableEmpty();
	//printf("set parse table empty\n");
	for (ruleNo = 0; ruleNo < num_rules; ruleNo++)
	{
		
		
		rule = grammar[ruleNo];
		//printf("Currently inserting grammar rule: %d\n", ruleNo);
		//print_grule(ruleNo);

		getRuleRhs(rhs, rule.rhs, rule.rhs_len);	//reverse and pointer resolution
		//printf("rule rhs length: %d\n", rule.rhs_len);
		pos = 0;

		do
		{
			epsFlag = false;
			if (rhs[pos] != EPS) {
				getFirst(rhs[pos], first, &firstLen);	//get first set
				//printf("first set length: %d\n", firstLen);
				for(i=0; i<firstLen ; i++) 
				{
					if(first[i] != EPS) {
						checkAndFill(rule.lhs, first[i] ,ruleNo);
						//printf("checked and Filled\n");
					}
					else {
						epsFlag = true;
					}
				}
			}
			else {
				epsFlag = true;
			}
			pos++;

		}while(pos < rule.rhs_len && epsFlag);

		if(pos == rule.rhs_len && epsFlag) {		//rules of the form A->w and eps is in FIRST* (w)
			fillFollow(rule.lhs, ruleNo);
		}	
	}

}

// *********************************************************************************************************************************

int getCurRule() {
	int t_num = getTableValue(token_buf[cur_tok]);
	return parseTable[last_stack_rem][t_num];					// needs review
}

void pushRuleOnStack(int rule_num) {
	int rule = rule_num; //getCurRule();
	//printf("rule being pushed: %d\n", rule);
	int i;

	last_stack_rem = getTop_stack(stack);
	stack = pop_stack(stack);
	for (i=0; i < grammar[rule].rhs_len; i++) {
		if (grammar[rule].rhs[i] != EPS)
			stack = push_stack(stack, grammar[rule].rhs[i]);
	}
}

void populateTokens(int block_size) {
	int i;
	for (i=0; i<block_size; i++) {
		token_buf[i] = tkList[i].type;	
	}
}

void setParser() {
	
	char fun[10];
	tkList = (TK *)malloc(sizeof(TK)*BLOCK_SIZE);
	// INPUT FILES
	char file_gr[10] = "g3.txt";
	char file_firsts[20] = "firsts.csv";
	char file_follows[15] = "follows.csv";
	
	FILE *fp = fopen("tnt.txt", "r");
	int count = 0;
	
	tnt_hash = createHashtable();

	char str[40];
	int i=0, j;
	int num_tnt;
	strcpy(eps_arr, "EPS");
	
	fscanf(fp, "%d %d %d\n", &num_tnt, &noOfNonTerminals, &noOfTerminals);

	while (!feof(fp)) {
		fscanf(fp, "%s\n", str);

		strcpy(tnt_arr[count], str);
		insert_hash(tnt_hash, str, count);
		count++;
	}
	//strcpy(tnt_arr[count], "NOT DEFINED");  
	//fclose(fp2);
	EPS = find_hash(tnt_hash, eps_arr);
	strcpy(start_symbol, "<program>");
	//print_hash(tnt_hash);
	//printf("next up grammar\n");
	//gets(fun);
	//printf("start symbol hash: %d\n", find_hash(tnt_hash, start_symbol));
	//printf("hashtable created.\n");

	//****************** Parse the grammar rules *******************************/
	hash_grammar(file_gr, 1, 0, &grammar, &num_rules);
	/*for (i=0; i<num_rules; i++) {
		print_grule(i);
	}

	for (i=0; i<num_firsts; i++) {
		print_frule(i);
	}

	for (i=0; i<num_follows; i++) {
		print_forule(i);
	}*/
	//printf("\n ****************** Now to first sets ************************\n");
	hash_grammar(file_firsts, 0, 1, &firsts, &num_firsts);
	//printf("\n ******************* Now to follow sets ************************\n");
	hash_grammar(file_follows, 0, 1, &follows, &num_follows);
	
	firsts[99].rhs[0] = 99;

	// INITIALSISES THE PARSE TREE ROOT
	initialiseRoot();

	parseTable = (int **)malloc(sizeof(int *) * noOfNonTerminals);
	for (i=0; i<noOfNonTerminals; i++) {
		parseTable[i] = (int *)malloc(sizeof(int) * noOfTerminals);
	}

	//printf("hash of start symbol: %d\n", find_hash(tnt_hash, "UNARY_INCR"));
	createParseTable();
	
	printf("Number of rules: %d\n", num_rules);
	// FOR PRINTING / CHECKING PARSE TABLE ENTRIES
	/*for (i=0; i<noOfNonTerminals; i++) {
		for (j=0; j<noOfTerminals; j++) {
			if (parseTable[i][j] == 4) {
				printf("entry at position: [%d, %d] = %d\n", i, j, parseTable[i][j]); 
			}
		}
	}*/
	
	//printf("num_rules: %d\n", num_rules);
	
	//fclose(fp2);
}

void printStackStatus() {
	int i;
	ST_NODE stk = stack;
	printf("Stack status after pushing new rule\n");
	while (stk != NULL) {
		printf("%s\n", tnt_arr[stk->val]);
		stk = stk->next;

	}
	printf("\n");
}

TR_NODE makeParseTree() {
	// INITIALISATIONS
	// will call setParser that makes all initiaslisation - grammar, first / follow / parse table generation
	// will be called by AST main
	
	setParser();
	//printf("num rules 1: %d\n", num_rules);

	int cur_sym = find_hash(tnt_hash, start_symbol);		// Push the start symbol on stack 
	stack = push_stack(stack, cur_sym);
	//getTop_stack(stack);
	//printf("num rules 2: %d\n", num_rules);

	cur_tok = -1;
	int tk_from_lexer;
	TR_NODE cur_p;

	//printf("num rules 3: %d\n", num_rules);
	int i;
	/*for (i=0; i<10; i++) {
		printf("%d\n", tkList[i].type);
	}*/
	tk_from_lexer = getTokens(tkList, BLOCK_SIZE);
	//printf("\n**************************\n");
	int k=0;
	
	if (tk_from_lexer < BLOCK_SIZE) {
		token_buf[tk_from_lexer] = -1;
	}
	//printf("num rules 5: %d\n", num_rules);

	populateTokens(tk_from_lexer);
	cur_tok = 0;
	//printf("num rules 6: %d\n", num_rules);
	
	while (token_buf[cur_tok] != -1) {


		//printf("Stack top: %d\n", getTop_stack(stack));
		if (getTop_stack(stack) == token_buf[cur_tok]) {
			//printf("terminal match.\n");

			last_stack_rem = getTop_stack(stack);
			stack = pop_stack(stack);
			
			insertTerminal(tkList[cur_tok]);
			cur_tok++;
			cur_p = goToNextChildOfParent();
			if (cur_p == NULL) {
				//printf("breaking out !\n");
				break;
			}
		}
		else {
			if (getTop_stack(stack) == find_hash(tnt_hash, "SEMI_CL")) {
				// TOP OF STACK IS A ; AND IT DOES NOT MATCH
				printf("ERROR! Expected Semi Colon before line: %d\n\n", tkList[cur_tok].lineNo);
				char fun[25];
				gets(fun);
				ERROR_FLAG = 1;

				stack = pop_stack(stack);
				cur_p = goToNextChildOfParent();
				if (cur_p == NULL) {
					printf("\n\n Parse tree successfully created ....\n\n\n");
					break;
				}
			}		
			else if (getTop_stack(stack) < noOfNonTerminals) {				// NON TERMINAL ON TOP OF STACK
				//printf("Non terminal on stack top.\n");

				// CHEKCKING FOR ERROR RECOVERY
				int t_num = getTableValue(token_buf[cur_tok]);
				int rule_number = parseTable[getTop_stack(stack)][t_num];
				
				// PANIC MODE ERROR RECOVERY
				if (rule_number == -1) {
					printf("Error at Line no: %d ", tkList[cur_tok].lineNo);
					if(cur_tok>=1){
						printf(" =>missing symbol at %s ", tnt_arr[tkList[cur_tok-1].type]);
					}
					char fun[25];
					gets(fun);
					ERROR_FLAG = 1;
					while (contains(follows[getTop_stack(stack)].rhs, follows[getTop_stack(stack)].rhs_len, cur_tok) == 0 && token_buf[cur_tok] != -1) {
						cur_tok++;
						
						if (cur_tok >= BLOCK_SIZE) {
							tk_from_lexer = getTokens(tkList, BLOCK_SIZE);
							if (tk_from_lexer < BLOCK_SIZE) {
								token_buf[tk_from_lexer] = -1;
							}
							populateTokens(tk_from_lexer);
							cur_tok = 0;
						}
					}
					stack = pop_stack(stack);
					cur_p = goToNextChildOfParent();
					continue;
				}

				pushRuleOnStack(rule_number);
				
				//printStackStatus();
				//printf("calling insert_nary for rule: %d\n", getCurRule());
				insert_Nary(getCurRule());
				//printf("insertNary done\n");
			}
			else {
				ERROR_FLAG = 1;
				printf("Some other terminal!! Error!");
				cur_tok++;
			}
		} 
		

		if (cur_tok >= BLOCK_SIZE) {
			tk_from_lexer = getTokens(tkList, BLOCK_SIZE);
			if (tk_from_lexer < BLOCK_SIZE) {
				token_buf[tk_from_lexer] = -1;
			}
			populateTokens(tk_from_lexer);
			cur_tok = 0;
		}
	}
	if (getTop_stack(stack) != NULL) {
		ERROR_FLAG = 1;
		printf("ERROR. MAIN function NOT TERMINATED.\n");		
	}
	return parseTree.root;
}
/*
int main() {
	makeParseTree();
	return 0;
}*/
/*int main() {
	char file_gr[10] = "g3.txt";
	char file_firsts[20] = "firsts.csv";
	char file_follows[15] = "follows.csv";
	
	FILE *fp = fopen("tnt.txt", "r");
	FILE *fp2 = fopen("hasht.txt", "w");
	int count = 0;
	
	tnt_hash = createHashtable();
	
	char str[40];
	int i=0, j;
	int num_tnt;
	strcpy(eps_arr, "EPS");
	
	fscanf(fp, "%d %d %d\n", &num_tnt, &noOfNonTerminals, &noOfTerminals);
	
	tnt_arr = (char *)malloc(sizeof(char)*num_tnt*40);
	while (!feof(fp)) {
		////printf("count: %d\n", count);
		fscanf(fp, "%s\n", str);
		//str[strlen(str)] = '\0';
		if (count == 0) {
			printf("%c\n", str[0]);
		}
		fprintf(fp2, "%s\n", str);
		//strcpy(tnt_arr[i], str);
		insert_hash(tnt_hash, str, count);
		////printf("symbol scanned: %s, id: %d\n", str, find_hash(str));
		count++;
	}
	fclose(fp2);
	//noOfTerminals-=1;
	strcpy(start_symbol, "<program>");
	char fun[10];
	gets(fun);
	//print_hash(tnt_hash);
	//printf("next up grammar\n");
	gets(fun);
	//printf("start symbol hash: %d\n", find_hash(tnt_hash, start_symbol));
	////printf("hashtable created.\n");

	hash_grammar(file_gr, 1, 0, &grammar, &num_rules);
	//printf("\n ****************** Now to first sets ************************\n");
	hash_grammar(file_firsts, 0, 1, &firsts, &num_firsts);
	//printf("\n ******************* Now to follow sets ************************\n");
	hash_grammar(file_follows, 0, 1, &follows, &num_follows);
	////printf("grammar hashed.\n");
	char ch;
	//grammar[0].lhs = 0;
	//printf("num rules: %d\n", num_rules);
	//printf("num firsts: %d\n", num_firsts);
	//printf("num follows: %d\n", num_follows);
	//printf("\n ******************* Confirm first sets ************************\n");
	firsts[99].rhs[0] = 99;
	for (i=0; i<num_firsts; i++) {
		//print_frule(i);
		
	}

	//printf("\n ******************* Confirm follow sets ************************\n");
	scanf("%c", &ch);
	for (i=0; i<num_follows; i++) {
		//print_forule(i);
	}

	////printf("EPS: %s, hash of: %d\n", eps_arr, find_hash(tnt_hash, eps_arr));
	//printf("Terminals: %d, non-terminal: %d\n", noOfTerminals, noOfNonTerminals);
	initialiseRoot();

	parseTable = (int **)malloc(sizeof(int *) * noOfNonTerminals);
	for (i=0; i<noOfNonTerminals; i++) {
		parseTable[i] = (int *)malloc(sizeof(int) * noOfTerminals);
	}
	scanf("%c", &ch);

	//printf("hash of start symbol: %d\n", find_hash(tnt_hash, "UNARY_INCR"));
	createParseTable();
	
	for (i=0; i<noOfNonTerminals; i++) {
		for (j=0; j<noOfTerminals; j++) {
			if (parseTable[i][j] == 4) {
				printf("entry at position: [%d, %d] = %d\n", i, j, parseTable[i][j]); 
			}
		}
	}
	
	//printf("num_rules: %d\n", num_rules);
	
	fclose(fp2);
	return 0;
}*/
