/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_gen.h"

#define startAddr 100
#define num_registers 4
#define regAlloc_lines 5

int op_plus;
int globalOffset = 100;
int labelCount = 0;

register_uids vars_in_register[5];

FILE *fp;

//FUNCTION PROTOTYPES
void initialise_codeGen();
int calculate_score(int uid, int lineNo);
void getMinScoreReg(Register *minScoreReg, int *minScore, int isExpr, int lineNo);
Register getMinCostRegister(int lineNo);
Register allocateReg(int uid, int lineNo);
void replaceVarsInRegister(Register regNo, int uid);
void flushVariable(Register regNo, int index);
void flushRegister(Register regNo); 
void insertVariableInRegister(Register regNo, int uid);
Register isInRegister(AST_NODE root);
Register code_Variable(AST_NODE root, int *offset, int getValueOrNot);
Register code_increment(AST_NODE root, int *offset);
void setDirty(int regNo);
void code_conditional(AST_NODE root, int endLabel);
//void code_expr(AST_NODE root);
int isIncrement(AST_NODE root);
void code_loop(AST_NODE root);
void code_botOps(AST_NODE root);
void finalise();
void ASTtoCode(AST_NODE root);
void code_stmts(AST_NODE root);
void code_Decl(AST_NODE cur_stmt);
void code_assign(AST_NODE root);
void code_io(AST_NODE root);
void code_expr(AST_NODE root);
int getHeight(AST_NODE root);

void initialise_codeGen() {
	fp = fopen("code_generated.asm", "w");

	fprintf(fp, ".MODEL TINY\n");
	fprintf(fp, ".386\n");
	fprintf(fp, ".STACK 100\n");
	fprintf(fp, ".DATA\n");
	
	// for user input
	fprintf(fp, "MAX1 DB 32\n");
	fprintf(fp, "ACT1 DB ?\n");
	fprintf(fp, "INP1 DB 32 DUP(0)\n");

	fprintf(fp, ".CODE\n");
	fprintf(fp, ".STARTUP\n");
	//fprintf(fp, "MOV SI, %d", startAddr);
	fprintf(fp, "\n");
	fprintf(fp, "; Global Variables\n");

	op_plus = find_hash(tnt_hash, "PLUS");

	int i;
	for (i=0; i<num_registers; i++) {
		vars_in_register[i].num_vars = 0;
	}
}

void reopen_asm(int choice) {
	fclose(fp);
	fp = fopen("code_generated.asm", "w");
	if (choice == 2) {
		fprintf(fp, "ERRORS IN YOUR CODE. PLEASE CHECK ERRORS.TXT\n");
	}
	fclose(fp);
}

int calculate_score(int uid, int lineNo) {
	printf("calculating score for uid: %d from line: %d\n", uid, lineNo);
	int *ref_arr = var_attr_table[uid].line_ref;
	int num_ref = var_attr_table[uid].line_ref_num;

	int j;
	printf("line ref array for uid: %d\n", uid);
	for (j=0; j<num_ref; j++) {
		printf("%d, ", ref_arr[j]);
	}
	printf("\n");

	/*for (j=0; j<num_ref; j++) {
		printf("%d, ", var_attr_table[uid].modRef[j]);
	}*/
	//printf("\n");

	int i=0, score = 0;

	while (i < num_ref && ref_arr[i] < lineNo) {
		i++;
	}

	while ((i < num_ref) && (ref_arr[i] < lineNo + regAlloc_lines)) {
		//printf("line ref for score: %d\n", ref_arr[i]);
		score +=  (regAlloc_lines - (ref_arr[i] - lineNo));
		i+=1;
	}

	//printf("score = %d\n", score);
	return score;
}

void getMinScoreReg(Register *minScoreReg, int *minScore, int isExpr, int lineNo) {
	*minScore = 1000;
	int i=0, j, sc;
	//Register minScoreReg;

	i = isExpr;
	for (; i<num_registers; i++) {
		printf("\nCalculating score for reg: %s\n", reg_name[i]);
		for (j=0; j<vars_in_register[i].num_vars; j++) {
			sc = calculate_score(vars_in_register[i].vars[j], lineNo);

			if (sc < *minScore /*&& j<vars_in_register[i].num_vars*/) {
				*minScore = sc;
				*minScoreReg = i;
			}
		}
	}
	printf("min score reg: %s, score = %d\n", reg_name[*minScoreReg], *minScore);
}

Register getMinCostRegister(int lineNo) {
	Register regNo;
	int minScore;

	getMinScoreReg(&regNo, &minScore, 1, lineNo);
	flushRegister(regNo);

	return regNo;
}

Register allocateReg(int uid, int lineNo/*, int isExpr*/) {
	int minScore;
	Register minScoreReg;

	//printf("\n*******************\nAllocate register called for uid: %d line.number %d\n\n", uid, lineNo);

	int score = calculate_score(uid, lineNo+1);
	if (score == 0) {
		return noReg;
	}

	int i;
	for (i=1/*ToDo: change this*/; i<num_registers; i++) {
		if (vars_in_register[i].num_vars == 0) {
			printf("**************** \nregister %s was empty, allocated.\n\n", reg_name[i]);
			vars_in_register[i].num_vars = 1;
			vars_in_register[i].vars[0] = uid;
			return i;
		}
	}

	//printRegisterStates();

	getMinScoreReg(&minScoreReg, &minScore, 1, lineNo);
	int new_sc = calculate_score(uid, lineNo+1);
	printf("\nScore of new var: %d = %d\n", uid, new_sc);
	printf("min score from regs: %d\n\n", minScore);

	if (new_sc > 0 && new_sc > minScore) {
		printf("\n****************\nReplacing in register: %s\n****************\n", reg_name[minScoreReg]);
		replaceVarsInRegister(minScoreReg, uid);

		return minScoreReg;
	}
	else {
		printf("\n*****************\nRegister not allocated.\n*****************\n");
		return noReg;
	}
}

void replaceVarsInRegister(Register regNo, int uid) {
	//fprintf(fp, "\n; Replacing variable in register: %s\n", reg_name[regNo]);
	flushRegister(regNo);
	insertVariableInRegister(regNo, uid);
}

void flushVariable(Register regNo, int index) {
	int uid, offset;

	if (vars_in_register[regNo].isDirty == 1) {
		uid = vars_in_register[regNo].vars[index];
		var_attr_table[uid].reg = noReg;

		offset = getOffset(uid);
		fprintf(fp, "MOV SI, %dD\n", offset);
		fprintf(fp, "MOV [SI], %s 	;Flushing %s\n", reg_name[regNo], reg_name[regNo]);	
	}	
}

void flushRegister(Register regNo) {
	int i;

	for (i=0; i<vars_in_register[regNo].num_vars; i++) {
		flushVariable(regNo, i);
	}
	vars_in_register[regNo].num_vars = 0;
}

void insertVariableInRegister(Register regNo, int uid) {
	int offset;

	offset = getOffset(uid);
	var_attr_table[uid].reg = regNo;
	vars_in_register[regNo].vars[vars_in_register[regNo].num_vars] = uid;
	vars_in_register[regNo].num_vars += 1;

	/*fprintf(fp, "MOV SI, %dD\n", offset);
	fprintf(fp, "MOV %s, [SI]\n", reg_name[regNo]);	*/
}

void removeFromReg(int regNo, int uid) {

	int i, j;
	printf("removing uid: %d from Reg: %s\n", uid, reg_name[regNo]);
	for (i=0; i<vars_in_register[regNo].num_vars; i++) {

		if (vars_in_register[regNo].vars[i] == uid) {
			//printf("found the uid in register\n");
			if (i == vars_in_register[regNo].num_vars-1) {
				vars_in_register[regNo].num_vars--;
				return;
			}
			else {
				for (j=i; j<vars_in_register[regNo].num_vars-1; j++) {
					vars_in_register[regNo].vars[j] = vars_in_register[regNo].vars[j+1];
				}
				vars_in_register[regNo].num_vars--;
				return;
			}
		}
	}
}

Register isInRegister(AST_NODE root) {
	if(root->type != variable)
	{
		return noReg;
	}

	int uid;
	uid = getUID(root->token.str);
	//printf("checking for variable uid: %d\n", uid);

	int i, j;

	for (i=0; i<num_registers; i++) {
		for (j=0; j<vars_in_register[i].num_vars; j++) {
			if (vars_in_register[i].vars[j] == uid) {
				printf("variable %s already present in register: %s, in lineNo:%d\n", root->token.str, reg_name[i], root->token.lineNo);
				return i;
			}
		}
	}

	return noReg;
}

void printRegisterStates(int lineNo) {
	int i, j;
	printf(" ***************** \n Register States :\n ***************** \n");

	for (i=0; i<num_registers; i++) {
		printf("Register %s :: number: %d\n", reg_name[i], vars_in_register[i].num_vars);
		for (j=0; j<vars_in_register[i].num_vars; j++) {
			printf("%d, \t", vars_in_register[i].vars[j]);
		}
		printf("\n");
	}
}

Register code_Variable(AST_NODE root, int *offset, int getValueOrNot) {
	Register r1;
	int uid = getUID(root->token.str);
	int prev_reg = -1;

	//printf("\ncode variable called from line %d. uid:%d, %s\n", root->token.lineNo,uid, root->token.str);

	if (root->num_children == 0) {
		// if already in register returns regNo, else noReg
		r1 = isInRegister(root);
		//printf("iSInRegister returns: %s, %d\n", reg_name[r1], getValueOrNot);
		if (r1 != noReg) {
			
			if(getValueOrNot == 1)
			{
				//printf("returning %s\n", reg_name[r1]);
				return r1;
			}
			
			if (vars_in_register[r1].num_vars == 1) {
				if(getValueOrNot ==0) 
				{
					setDirty(r1);
				} 
				//printf("returning %s\n", reg_name[r1]);
				return r1;
			}
			
			if(getValueOrNot ==0) 
			{
				//setDirty(r1);
				//printf("prev reg set to %s\n", reg_name[r1]);
				prev_reg = r1;
			} 
			//flushVariable(r1, uid);
		}

		// get a new register for the variable
		*offset = getOffset(uid);
		//printf("got offset: %d for uid: %d\n", *offset, uid);

		r1 = allocateReg(uid, root->token.lineNo);

		if (prev_reg != -1 ) {
			if (r1 != noReg)
			{
				fprintf(fp, "MOV %s, %s\n", reg_name[r1], reg_name[prev_reg]);
				setDirty(r1);
			}
			removeFromReg(prev_reg, uid);
			// sorted shift
		}
		else if (r1 != noReg && getValueOrNot == 1) {
			fprintf(fp, "MOV SI, %dD\n", *offset);
			fprintf(fp, "MOV %s, [SI]\n", reg_name[r1]); 
		}
		else if (r1 != noReg && getValueOrNot == 0) {
			setDirty(r1);
		}

		return r1;
	}
	else if (root->num_children == 1) {
		// case 1d array

		// case INT
		int type_offset = INT_SIZE;	
		int uid = getUID(root->token.str);

		//Register r1 = allocateReg(uid, root->token.lineNo);
		*offset = getOffset(uid); 
		//printf("got offset: %d for uid: %d\n", *offset, uid);


		*offset += (root->children[0]->token.num)*type_offset;
		
		return noReg;
		/*fprintf(fp, "MOV SI, %dD", arr_base);
		fprintf(fp, "MOV %s, [SI]", reg_name[r1]); */
	}
	else {
		// case 2d array
		// case INT
		int type_offset = INT_SIZE;
		int uid = getUID(root->token.str);		
		
		//Register r1 = allocateReg(uid, root->token.lineNo);
		*offset = getOffset(uid); 
		//printf("got offset: %d for uid: %d\n", *offset, uid);

		*offset += (root->children[0]->token.num*root->type_extract->bound2)*type_offset;
		*offset += root->children[1]->token.num*type_offset;
		
		return noReg;
		/*
		fprintf(fp, "MOV SI, %dD", arr_base);
		fprintf(fp, "MOV %s, [SI]", reg_name[r1]); 
		*/	
		//return r1;
	}
}

Register code_increment(AST_NODE root, int *offset) {
	int uid;

	uid = getUID(root->children[0]->token.str);
	*offset = getOffset(uid);
		//printf("got offset: %d for uid: %d in incr\n", *offset, uid);
	
	Register r1 = isInRegister(root->children[0]);
	
	if (r1 == noReg) {
		r1 = allocateReg(uid, root->children[0]->token.lineNo);
	}
	if (r1 != noReg) {
		// INCREMENT IN REGISTER
		fprintf(fp, "INC %s\n", reg_name[r1]);
		setDirty(r1);
	}
	else {
		// INCREMENT IN MEMORY OFFSET
		//fprintf(fp, "PUSH AX\n");

		fprintf(fp, "MOV SI, %dD\n", *offset);
		//fprintf(fp, "MOV AX, [SI]\n");
		fprintf(fp, "INC word ptr [SI]\n");
		//fprintf(fp, "MOV [SI], AX\n");

		//fprintf(fp, "POP AX\n", );
	}

	return r1;
}


void setDirty(int regNo) {
	vars_in_register[regNo].isDirty = 1;
}


void code_conditional(AST_NODE root, int endLabel)
{
	//works for lt and equal
	int else_label = labelCount;
	labelCount++;
	fprintf(fp, "\n;CONDITIONAL \n");
	code_expr(root->children[0]);
	fprintf(fp, "JGE L%d\n", else_label);
	code_stmts(root->children[1]);
	fprintf(fp, "JMP L%d\n", endLabel);
	fprintf(fp, "L%d:\n", else_label);
	//code_stmts(root->children[2]);
	
	if (root->children[2]->type == stmts) {
		// IF -- THEN -- ELSE
		code_stmts(root->children[2]);
	}
	else {
		// IF -- THEN -- ELSE IF 
		code_conditional(root->children[2], endLabel);
	}
}

void code_expr(AST_NODE root)
{
	//the three cases on siddhant's laptop
	
	//heuristic followed => Evaluate child of greater height first to facilitate regAlloc.
		//Min height is zero. If height > 0 => definitely an operator
	int other = -1;
	int Lt = 0;
	Register reg, r1;
	
		if (root->type == variable && root->num_children>0 && root->children[0]->type == op_splice) {
			AST_NODE temp  = root;
			//Its a splice!!
			int base = getOffset(getUID(temp->token.str));
			int last = base + (temp->children[0]->children[1]->token.num - 1)*2;
			int first = base + (temp->children[0]->children[0]->token.num)*2;
			int splice = labelCount;
			labelCount++;

			//traverse splice from end to first
			fprintf(fp, "\n;Splice encountered in expression, pushing values on stack.\n");
			fprintf(fp, "MOV SI, %dD\n", last);
			fprintf(fp, "MOV DI, %dD\n", first);
			fprintf(fp, "L%d:\n", splice);
			fprintf(fp, "PUSH [SI]\n");
			fprintf(fp, "DEC SI\n");
			fprintf(fp, "DEC SI\n");
			fprintf(fp, "CMP SI, DI\n");
			fprintf(fp, "JGE L%d\n\n", splice);

/*
			int uid = getuid(root->token.str);
			int offset = getOffset(uid);
			int i;

			// type = INT
			int type_offset = INT_SIZE;

			int lower_index = root->children[0]->children[0]->token.num; 
			int upper_index = root->children[0]->children[1]->token.num;

			for (i=lower_index; i<upper_index; i++) {
				fprintf(fp, "; Splice encountered in expression, pushing values on stack.\n");
				fprintf(fp, "MOV SI, %dD\n", offset + i*type_offset);
				fprintf(fp, "PUSH [SI]\n");
			}*/
		}
		else if (root->type == variable) {
			int offset;
			r1 = code_Variable(root, &offset, 1);
			if (r1 != noReg) {
				fprintf(fp, "PUSH %s\n", reg_name[r1]);
			}
			else {
				fprintf(fp, "MOV SI, %dD\n", offset);
				fprintf(fp, "PUSH [SI]\n");	
			}
		}
		else if (root->type == constant_value) {
			//code_constant(root);

			fprintf(fp, "PUSH %dD\n", root->token.num);
			//fprintf(fp, "PUSH SI\n");	
		}
		else if (isIncrement(root)) {
			int offset;
			Register r1;
			r1 = code_increment(root, &offset);

			if (r1 != noReg) {
				fprintf(fp, "PUSH %s\n", reg_name[r1]);
			}
			else {
				fprintf(fp, "MOV SI, %dD\n", offset);
				fprintf(fp, "PUSH [SI]\n");	
			}
		}
	
	else{
	AST_NODE child0 = root->children[0];
	AST_NODE child1 = root->children[1];
	int height0 = getHeight(child0);
	int height1 = getHeight(child1);
	//fprintf(fp, "\n; Code for expression evaluation.\n");
	if(root->token.type == find_hash(tnt_hash, "LT"))
	{
		Lt = 1;
	}


	if(height0 > height1)
	{
		code_expr(child0);
		other = 1;
	    //One value on stack
	}
	else if(height1 > height0)
	{
		code_expr(child1);
		other = 0;
		//One value on stack
	}
	else
	{
		//both heights are equal. Could be both zero or >0
		if(height1 > 0)
		{
			//both greater than zero ==> evaluate separately.
			code_expr(child0);		
			code_expr(child1);	

			/*  |___|
				|_1_|
				|_0_|	*/
 
			fprintf(fp, "POP AX\n");
			//i want a register with min cost. What function?	
			//can u show how u've writen the file print stmts? Spacing and formatting?
			reg = getMinCostRegister(child0->token.lineNo);
			fprintf(fp, "POP %s\n", reg_name[reg]);
			fprintf(fp, "ADD AX, %s\n", reg_name[reg]);
			fprintf(fp, "PUSH AX\n");
		}
		else
		{
			//means both are height zero!!
			//Case 1: Is any of them already in a regsiter?
			
			// is any of them a constant value ?
			int const1 = 0;
			
			if (root->children[0]->type == constant_value && root->children[1]->type == constant_value) {
				const1 = 3;
			}
			else if (root->children[0]->type == constant_value) {
				const1 = 1;
			}
			else if (root->children[1]->type == constant_value) {
				const1 = 2;
			}

			if (const1 == 3) {
				fprintf(fp, "MOV AX, %dD\n", root->children[0]->token.num);
				if (Lt){
					fprintf(fp, "CMP AX, %dD\n", root->children[1]->token.num);
				}
				else{
					fprintf(fp, "ADD AX, %dD\n", root->children[1]->token.num);
					fprintf(fp, "PUSH AX\n");
				}
			}
			else if (const1 == 1 || const1 == 2) {
				const1 -= 1;
				int other = 1-const1;
				Register r1;
				int offset;

				if (root->children[other]->type == variable) {
					
					r1 = code_Variable(root->children[other], &offset, 1);
				}	
				else if (isIncrement(root->children[other])) {
					r1 = code_increment(root->children[other], &offset);	
				}

				if (r1 == noReg) {
					fprintf(fp, "MOV SI, %dD\n", offset);
					fprintf(fp, "MOV AX, [SI]\n");
					if (Lt){
						fprintf(fp, "CMP AX, %dD\n", root->children[const1]->token.num);
					}
					else{
						fprintf(fp, "ADD AX, %dD\n", root->children[const1]->token.num);
						fprintf(fp, "PUSH AX\n");
					}
				}
				else {
					if(Lt){
						fprintf(fp, "CMP %s, %dD\n", reg_name[r1], root->children[const1]->token.num);
					}
					else
					{
						fprintf(fp, "MOV AX, %s\n", reg_name[r1]);
						fprintf(fp, "ADD AX, %dD\n", root->children[const1]->token.num);
						fprintf(fp, "PUSH AX\n");
					}
				}
			}
			else {
				int offset1, offset2;
				Register r1, r2;
				int reg_state = 0;
				char *oper;
				if(Lt){
					oper = "CMP";
				}
				else{
					oper = "ADD";
				}
				if (root->children[0]->type == variable) {
					r1 = code_Variable(root->children[0], &offset1, 1);
				}
				else {
					r1 = code_increment(root->children[0], &offset1);	
				}

				if (root->children[1]->type == variable) {
					r2 = code_Variable(root->children[1], &offset2, 1);
				}
				else {
					r2 = code_increment(root->children[1], &offset2);	
				}
					
					
				if (r1 != noReg && r2 != noReg) {
					reg_state = 3;
				}
				else if (r1 != noReg) {
					reg_state = 1;
				}
				else if (r2 != noReg) {
					reg_state = 2;
				}

				if (reg_state == 3) {
					fprintf(fp, "MOV AX, %s\n", reg_name[r1]);
					fprintf(fp, "%s AX, %s\n",oper, reg_name[r2]);
				}
				else if (reg_state == 1) {
					fprintf(fp, "MOV AX, %s\n", reg_name[r1]);
					fprintf(fp, "MOV SI, %dD\n", offset2);
					fprintf(fp, "%s AX, [SI]\n", oper);
				}
				else if (reg_state == 2) {
					fprintf(fp, "MOV AX, %s\n", reg_name[r2]);
					fprintf(fp, "MOV SI, %dD\n", offset1);
					fprintf(fp, "%s AX, [SI]\n", oper);
				}
				else {
					fprintf(fp, "MOV SI, %dD\n", offset1);
					fprintf(fp, "MOV AX, [SI]\n");
					fprintf(fp, "MOV SI, %dD\n", offset2);
					fprintf(fp, "%s AX, [SI]\n", oper);
				}
				if(!Lt)
				{
					fprintf(fp, "PUSH AX\n");
				}
			}
		}
		//fprintf(fp, "\n; End of expression evaluation.\n");	
	}

	if(other == 1 || other ==0)
	{
		//Means one child has already been evaluated. Evaluate the second.
		AST_NODE child = root->children[other];
		if (getHeight(child) > 0)
		{
			code_expr(child);
			fprintf(fp, "POP AX\n");
			reg = getMinCostRegister(child->token.lineNo);
			fprintf(fp, "POP %s\n", reg_name[reg]);
			fprintf(fp, "ADD AX, %s\n", reg_name[reg]);	
		}
		else
		{
			//Case 1: Constant
			if(child->type == constant_value)
			{
				if(child->token.num != 0)
				{
					//Optimisation : Ignore if zero is added
					fprintf(fp, "POP AX\n");
					/*if (child->token.num == 1){
						//Optimisation: USE INC instead
						fprintf(fp, "INC AX\n");
					}
					else{*/
					fprintf(fp, "ADD AX, %dD\n", child->token.num);
					//}
					fprintf(fp, "PUSH AX\n");				
				}
			}
			else 
			{
				int offset;
				//Case 2: Variable
				if (child->type == variable)
				{
					reg = code_Variable(child, &offset, 1);
				}
				//Case 3: Unary Increment
				else if (child->type == op && child->num_children == 1)
				{
					reg = code_increment(child, &offset);
				}
				
				if(reg == noReg)
				{
					fprintf(fp, "POP AX\n");
					fprintf(fp, "MOV SI, %dD\n", offset);
					fprintf(fp, "ADD AX, [SI]\n");
					fprintf(fp, "PUSH AX\n");			
				}
				else
				{
					fprintf(fp, "POP AX\n");
					fprintf(fp, "ADD AX, %s\n", reg_name[reg]);
					fprintf(fp, "PUSH AX\n");		
				}
			}
			
		}
	}

}
}	

int isIncrement(AST_NODE root) {
	if (root->token.type == find_hash(tnt_hash, "UNARY_INCR")) {
		return 1;
	}

	return 0;
}

void code_loop(AST_NODE root)
{
	//Break and continue later
	int for_label = labelCount, end_label = labelCount+1, offset;
	labelCount+=2;
	
	fprintf(fp, "\n;FOR STATEMENT\n");
	fprintf(fp, ";Initialisation =>");
	code_assign(root->children[0]);
	
	fprintf(fp, "L%d: 		;Loop\n", for_label);
	code_expr(root->children[1]);	
	fprintf(fp, "JGE L%d 	;Jump to end of loop if expr not satisfied\n", end_label);
	code_stmts(root->children[3]);
	code_increment(root->children[2], &offset);
	fprintf(fp, "JMP L%d 	;Jump back up!\n", for_label);

	fprintf(fp, "L%d:		;End of loop\n", end_label);
}


void finalise_codeGen()
{

	//flush all regs before exiting!
	int i;

	fprintf(fp, "\n");
	for (i=0; i<num_registers; i++) {
		flushRegister(i);
	}

	fprintf(fp, "\n.EXIT\n");
	fprintf(fp, "END\n" );
	fclose(fp);

	/*fp = fopen("code_generated.asm", "w");
	removePushPops(fp);
	fclose(fp);*/
} 

void ASTtoCode(AST_NODE root) {
	int mainChild = root->num_children - 1;
	AST_NODE main_node = root->children[mainChild];
	

	fprintf(fp, "; Following is the code of the main function.\n");
	code_stmts(main_node);
	
	finalise_codeGen();
}


void code_stmts(AST_NODE root) {
	int stmt_count = root->num_children;
	int cur_stmt_num = 0;

	while (cur_stmt_num < stmt_count) {
		AST_NODE cur_stmt;
		int offset;

		cur_stmt = root->children[cur_stmt_num];

		if (cur_stmt->type == dataType) {
			// declaration
			code_Decl(root->children[cur_stmt_num]);
		}
		else if (cur_stmt->type == conditional) {
			// IF
			int endLabel = labelCount;
			labelCount++;
			code_gotoNextScope();
			
			code_conditional(root->children[cur_stmt_num], endLabel);
			fprintf(fp, "L%d:\n", endLabel);

			code_gotoParentScope();
		}
		else if (cur_stmt->type == loop) {
			// FOR
			code_gotoNextScope();
			
			code_loop(root->children[cur_stmt_num]);	
			
			code_gotoParentScope();
		}
		else if (cur_stmt->type == io) {
			// IO
			//printf("got an io\n");
			code_io(root->children[cur_stmt_num]);
			//printf("Done the io\n");
		}
		else if (cur_stmt->type == assign) {
			// assignment
			printf("Before lineNo: %d\n", cur_stmt->token.lineNo);
			printRegisterStates(cur_stmt->token.lineNo);
			code_assign(root->children[cur_stmt_num]);
			printf("After lineNo: %d\n", cur_stmt->token.lineNo);
			printRegisterStates(cur_stmt->token.lineNo);
		}	
		else if (cur_stmt->type == botOps) {
			// bot, point
			code_botOps(root->children[cur_stmt_num]);
		}
		else if (isIncrement(cur_stmt)) {
			int offset;
			code_increment(root, &offset);
		}

	cur_stmt_num++;
	}
}


void code_global(int val) {
	
	fprintf(fp, "MOV SI, %dD\n", globalOffset);
	fprintf(fp, "PUSH %dD\n", val);
	fprintf(fp, "POP [SI]\n\n");

	globalOffset += INT_SIZE;
}

void code_Decl(AST_NODE cur_stmt) {

	int num_decl = cur_stmt->num_children;
	int type = cur_stmt->token.type;
	int sg_size;
	int i;
	int val = 0;
	int offset, bound1, bound2;	

	if (strcmp(tnt_arr[dataType], "INT")==0 || strcmp(tnt_arr[dataType], "BOOLEAN")==0) {
		sg_size = INT_SIZE;	
	}
	else if (strcmp(tnt_arr[dataType],"FLOAT")==0) {
		sg_size = FLOAT_SIZE;
	}
	else if (strcmp(tnt_arr[dataType], "POINT")==0) {
		sg_size = INT_SIZE;
	}
	else if (strcmp(tnt_arr[dataType], "BOT")==0) {
		sg_size = INT_SIZE;
	}

	for (i=0; i<num_decl; i+=2) {
		//offset = getOffset(cur_stmt->children[i]);

		if (cur_stmt->children[i]->num_children == 0) {
			
			int uid = getUID(cur_stmt->children[i]->token.str);
			var_attr_table[uid].offset = globalOffset;

			if (cur_stmt->children[i+1] == NULL) {
				// DS has the base address
				fprintf(fp, "\n; Declaration: %s = 0\n", cur_stmt->children[i]->token.str);
				fprintf(fp, "MOV SI, %dD\n", globalOffset);
				fprintf(fp, "PUSH 0D\n");
				fprintf(fp, "POP [SI]\n");		// change this to stosw
			}
			else {
				fprintf(fp, "\n; Declaration: %s = %d\n", cur_stmt->children[i]->token.str, cur_stmt->children[i+1]->token.num);

				code_expr(cur_stmt->children[i+1]);
				//fprintf(fp, "POP AX\n");
				fprintf(fp, "MOV SI, %dD\n", globalOffset);
				fprintf(fp, "POP [SI]\n");
			}
			globalOffset += INT_SIZE;
		}
		else if (cur_stmt->children[i]->num_children == 1) {
			//int b1 = cur_stmt->children[i]->type_extract->bound1;
			// change to var attr table
			bound1 = cur_stmt->children[i]->children[0]->token.num;
			fprintf(fp, "MOV AX, 0\n");
			for (i=0; i<bound1; i++) {
				fprintf(fp, "MOV SI, %dD\n", globalOffset);
				fprintf(fp, "MOV [SI], AX\n");
				globalOffset += INT_SIZE;
			}	
		}
		else if (cur_stmt->children[i]->num_children == 2) {
			bound1 = cur_stmt->children[i]->children[0]->token.num;
			bound2 = cur_stmt->children[i]->children[1]->token.num;

			fprintf(fp, "MOV AX, 0\n");
			for (i=0; i<bound1*bound2; i++) {
				fprintf(fp, "MOV SI, %dD\n", globalOffset);
				fprintf(fp, "MOV [SI], AX\n");
				globalOffset += INT_SIZE;
			}	
		}
	}

	fprintf(fp, "; ------------------ END OF DECLARATION OPERATIONS -------------------\n");
}

void code_assign(AST_NODE root)
{
	AST_NODE left = root->children[0];
	AST_NODE right = root->children[1];
	AST_NODE temp;
	int leftEntities=0, rightEntities=0, bug, prev = -1;
	int lhscur = left->num_children;
	int rhscur = right->num_children;
	//array for holding num entities covered yet.. left and right
	//int left_num_ele[10], right_num_ele[10];
	int lhspos = 0, rhspos = 0, i, offset, te, offset2;
	Register r1, r2;
	fprintf(fp, "\n;ASSIGNMENT STATEMENT, Line no : %d\n", root->token.lineNo);
	
	//ONE ENTITY	
	if(lhscur == 1 && getNumberOfEntities(left->children[0]) == 1 && left->children[0]->type_extract->isArray == 0)
	{
		//a = expr
		//a = b
		//a = a+0
		//a = a+1
		AST_NODE child = right->children[0];
		AST_NODE parent = left->children[0];

		int parUid = getUID(parent->token.str);
		
		r1 = code_Variable(parent, &offset, 0);
		//printf("after upper code_var\n");
		if(getHeight(child) == 0)
		{
			//CASES LIKE a=b || a= num || a = ++b

			if (child->type == variable  ||  isIncrement(child)) 
			{
				if(child->type == variable)
				{
					//CASE 1 : a=b
					//Heuristic: Ignore if of kind a=a
					//printf("sending string %s\n", child->token.str);
					int childUid = getUID(child->token.str);
					//printf("uid1: %d, uid2:%d\n", parUid, childUid);
					if(parUid	== childUid)
					{
						fprintf(fp, ";Ignoring assignment (of kind a=a)\n");
						return;
					}	
					//printf("going to code vars. uid not same\n");			
					r2 = code_Variable(child, &offset2, 1);	
				}
				else
				{
					//CASE 3 : a = ++b
					int childUid = getUID(child->token.str);
					r2 = code_increment(child, &offset2);
					if(parUid	== childUid)
					{
						//Heuristic
						fprintf(fp, ";Ignoring redundant assignment (of kind a= ++a), Increment done\n");
						return;
					}				
				}
				
				if (r2 != noReg) 
				{
					//Heuristic : Already in a register => just move parent also!
					insertVariableInRegister(r2, parUid);
					if( r1 != noReg)
					{	//If parent was in a register, then remove.
						removeFromReg(r1, parUid);
					}
					fprintf(fp, ";Added lhs to [  %s  ]'s register\n", child->token.str);
				}
				else {
					//child was not allocated register!
					if(r1 == noReg)
					{
						//parent also not in register.
						fprintf(fp, ";both sides not allocated register. Memory reference.\n");
						fprintf(fp, "MOV SI, %dD	\n", offset);
						fprintf(fp, "MOV DI, %dD	\n", offset2);
						fprintf(fp, "MOV AX, [DI]\n");
						fprintf(fp, "MOV [SI], AX\n");
					}
					else
					{
						fprintf(fp, ";[  %s  ] not allocated register. Get val and assign.\n", child->token.str);
						fprintf(fp, "MOV SI, %dD\n", offset2);
						fprintf(fp, "MOV %s, [SI]\n", reg_name[r1]);	
					}
				}
			}
			else if (child->type == constant_value) 
			{
				//CASE 2:  a = num
				if(r1 == noReg)
				{
					fprintf(fp, "MOV SI, %dD	\n", offset);
					fprintf(fp, "PUSH %d\n", child->token.num);
					fprintf(fp, "POP [SI]\n");
				}
				else
				{
					fprintf(fp, "MOV %s, %d\n", reg_name[r1], child->token.num);
				}				
			}
			
			return;
		}
		else 	//if height is 1 and some particular conditions 
		{
			if(getHeight(child) == 1 && child->children[1]->type == constant_value)
			{
				if(child->children[1]->token.num == 0)
				{
					//ok for type a= a+0. What happens for a=b+0?
					if(child->children[0]->type == variable)
					{
						//a = b+0 kind!!!!
						child = child->children[0];
						int childUid = getUID(child->token.str);
						if(parUid	== childUid)
						{
							fprintf(fp, ";Ignoring assignment (of kind a=a+0)\n");
							return;
						}				
						r2 = code_Variable(child, &offset2, 1);	
						if (r2 != noReg) 
						{
							//Heuristic : Already in a register => just move parent also!
							insertVariableInRegister(r2, parUid);
							if( r1 != noReg)
							{	//If parent was in a register, then remove.
								//printf("Sending to removeFrom Reg\n");
								removeFromReg(r1, parUid);
							}
							fprintf(fp, ";Added lhs to [  %s  ]'s register\n", child->token.str);
						}
						else {
							//child was not allocated register!
							if(r1 == noReg)
							{
								//parent also not in register.
								fprintf(fp, ";both sides not allocated register. Memory reference.\n");
								fprintf(fp, "MOV SI, %dD	\n", offset);
								fprintf(fp, "MOV DI, %dD	\n", offset2);
								fprintf(fp, "MOV AX, [DI]\n");
								fprintf(fp, "MOV [SI], AX\n");
							}
							else
							{
								fprintf(fp, ";[  %s  ] not allocated register. Get val and assign.\n", child->token.str);
								fprintf(fp, "MOV SI, %dD\n", offset2);
								fprintf(fp, "MOV %s, [SI]\n", reg_name[r1]);	
							}
						}
						return;
					}
				}
				else if(child->children[1]->token.num == 1)
				{
					printf("case a = a+1 on the right\n");
					if (child->children[0]->type == variable) {
						int offset;
						int childUid = getUID(child->children[0]->token.str);
						//printf("parid:%d uid:%d\n", parUid, childUid);
						if(parUid	== childUid)
						{
							code_increment(child, &offset);
							return;
						}
					}
				}
			}
		}
	}

	//MORE THAN ONE
	for(i=0; i< rhscur; i++) {
		//printf("making rhs: %d\n", i);
		code_expr(right->children[i]);
	}
	fprintf(fp, ";Right hand side pushed\n");	
	//printf("all rhs made\n");
	for(i=lhscur-1; i>=0; i--) {
		temp = left->children[i];
		bug = getNumberOfEntities(temp);
		leftEntities+=bug;
		//left_num_ele[i] = bug+prev;
		//on the left, it could be an array element/id => 1 element or a splice. 
		if(bug > 1 || (temp->num_children==1 && temp->children[0]->type == op_splice))
		{
			//Its a splice!!
			int base = getOffset(getUID(temp->token.str));
			int last = base + (temp->children[0]->children[1]->token.num - 1)*2;
			int first = base + (temp->children[0]->children[0]->token.num)*2;
			int splice = labelCount;
			labelCount++;

			//traverse splice from end to first
			fprintf(fp, "MOV SI, %dD\n", last);
			fprintf(fp, "MOV DI, %dD\n", first);
			fprintf(fp, "L%d:\n", splice);
			fprintf(fp, "POP [SI]\n");
			fprintf(fp, "DEC SI\n");
			fprintf(fp, "DEC SI\n");
			fprintf(fp, "CMP SI, DI\n");
			fprintf(fp, "JGE L%d\n", splice);
		}
		else
		{
			//It's a variable. Single entity. Pop into register or that memory location.
			r1 = code_Variable(temp, &offset, 0);
			if(r1 == noReg)
			{
				fprintf(fp, "MOV SI, %dD	\n", offset);
				fprintf(fp, "POP [SI]\n");
			}
			else
			{
				fprintf(fp, "POP %s\n", reg_name[r1]);
			}

		}

	}
}

/*void code_assign(AST_NODE root)
{
	AST_NODE left = root->children[0];
	AST_NODE right = root->children[1];
	AST_NODE temp;
	int leftEntities=0, rightEntities=0, bug, prev = -1;
	int lhscur = left->num_children;
	int rhscur = right->num_children;
	//array for holding num entities covered yet.. left and right
	//int left_num_ele[10], right_num_ele[10];
	int lhspos = 0, rhspos = 0, i, offset;
	Register r1;
	fprintf(fp, "\n;ASSIGNMENT STATEMENT, Line no : %d\n", root->token.lineNo);
	for(i=0; i< rhscur; i++) {
		printf("making rhs: %d\n", i);
		code_expr(right->children[i]);
	}	
	printf("all rhs made\n");
	for(i=lhscur-1; i>=0; i--) {
		temp = left->children[i];
		bug = getNumberOfEntities(temp);
		leftEntities+=bug;
		//left_num_ele[i] = bug+prev;
		//on the left, it could be an array element/id => 1 element or a splice. 
		if(bug > 1 || (temp->num_children==1 && temp->children[0]->type == op_splice))
		{
			//Its a splice!!
			int base = getOffset(getUID(temp->token.str));
			int last = base + (temp->children[0]->children[1]->token.num - 1)*2;
			int first = base + (temp->children[0]->children[0]->token.num)*2;
			int splice = labelCount;
			labelCount++;

			//traverse splice from end to first
			fprintf(fp, "MOV SI, %dD\n", last);
			fprintf(fp, "MOV DI, %dD\n", first);
			fprintf(fp, "L%d:\n", splice);
			fprintf(fp, "POP [SI]\n");
			fprintf(fp, "DEC SI\n");
			fprintf(fp, "DEC SI\n");
			fprintf(fp, "CMP SI, DI\n");
			fprintf(fp, "JGE L%d\n", splice);
		}
		else
		{
			//It's a variable. Single entity. Pop into register or that memory location.
			r1 = code_Variable(temp, &offset, 0);
			if(r1 == noReg)
			{
				fprintf(fp, "MOV SI %dD	\n", offset);
				fprintf(fp, "POP [SI]\n");
			}
			else
			{
				fprintf(fp, "POP %s\n", reg_name[r1]);
			}

		}

	}
}*/


void code_io(AST_NODE root) {
	
	// FLUSH ALL REGISTERS
	fprintf(fp, "\n; ----------------- USER INPUT STATEMENT ------------\n; Pushing all registers onto stack before user input.\n");
	fprintf(fp, "PUSHA\n");
	/*fprintf(fp, "PUSH BX\n");
	fprintf(fp, "PUSH CX\n");
	fprintf(fp, "PUSH DX\n");*/
	
	//printf("var for user io: %s\n", root->children[0]->token.str);
	int uid = getUID(root->children[0]->token.str);
	
	int offset = getOffset(uid);
	Register regNo = isInRegister(root->children[0]);


	if (regNo != noReg) {
		flushRegister(regNo);
	}
	
	int jmpLabel = labelCount++;
	fprintf(fp, "\n; User Input. Reading an integer using READI >> %s.\n", root->children[0]->token.str);
	fprintf(fp, "LEA DX,MAX1\n");
	fprintf(fp, "MOV AH,0AH\n");
	fprintf(fp, "INT 21H\n");

	fprintf(fp, "; Converting the input read as string to an integer.\n");
	fprintf(fp, "LEA SI, INP1\n");
	fprintf(fp, "MOV CL, ACT1\n");
	fprintf(fp, "MOV AX, 0\n");
	fprintf(fp, "MOV BX, 10\n");
	fprintf(fp, "MOV DX, 0\n");
	fprintf(fp, "L1%d:\n", jmpLabel);
	//FIRST
	//fprintf(fp, "MOV DL, byte ptr [SI+CX-1]\n");
	//SECOND
	fprintf(fp, "ADD SI, CX\n");
	fprintf(fp, "DEC SI\n");
	fprintf(fp, "MOV DL, byte ptr [SI]\n");
	
	fprintf(fp, "SUB DL, 30H\n");
	fprintf(fp, "ADD AX, DX \t\t;In the end AX has the input integer.\n");
	fprintf(fp, "MUL BX\n");
	fprintf(fp, "DEC CX\n");
	fprintf(fp, "JNZ L1%d\n", jmpLabel);

	fprintf(fp, "\n; Copy the value from AX register to the memory location of input variable.\n");
	fprintf(fp, "MOV SI, %dD\n", offset);
	fprintf(fp, "MOV [SI], AX\n");

	fprintf(fp, "\n; Restoring the values in registers as before the user input.\n");
	fprintf(fp, "POPA\n");
	/*fprintf(fp, "POP CX\n");
	fprintf(fp, "POP BX\n");
	fprintf(fp, "POP AX\n");*/
	fprintf(fp, "; ------------------ END OF READ OPERATION -------------------\n");
}


void code_botOps(AST_NODE root)
{
	//can be FW or ADDV 
	if (root->token.type == find_hash(tnt_hash, "ADDV"))
	{
		char * ch1 = root->children[0]->token.str;
		char * ch2 = root->children[1]->token.str;
		char temp[40], temp2[40];
		
		//printf("chk1 -----------------\n");
		strcpy(temp, ch1);	strcpy(temp2, ch1);
		strcat(temp, "_x");	strcat(temp2, "_y");
		//printf("chk2 -----------------\n");
		int ch1xOffset = getOffset(getUID(temp));
		int ch1yOffset = getOffset(getUID(temp2));
		strcpy(temp, ch2);	strcpy(temp2, ch2);
		strcat(temp, "_x");	strcat(temp2, "_y");
		int ch2xOffset = getOffset(getUID(temp));
		int ch2yOffset = getOffset(getUID(temp2));

		//printf("chk3 -----------------\n");
		fprintf(fp, "\n;ADDV STATEMENT \n");
		fprintf(fp, "MOV SI, %dD \n", ch1xOffset);
		fprintf(fp, "MOV DI, %dD \n", ch2xOffset);
		fprintf(fp, "MOV AX, [DI]\n");
		fprintf(fp, "ADD [SI], AX\n");

		fprintf(fp, "MOV SI, %dD \n", ch1yOffset);
		fprintf(fp, "MOV DI, %dD \n", ch2yOffset);
		fprintf(fp, "MOV AX, [DI]\n");
		fprintf(fp, "ADD [SI], AX\n");
		fprintf(fp, ";ADDV DONE\n");
	}

	if (root->token.type == find_hash(tnt_hash, "FW"))
	{
		char * ch1 = root->children[0]->token.str;
		char temp[40];
		strcpy(temp, ch1); strcat(temp, "_x");
		int ch1xOffset = getOffset(getUID(temp));
		int print_label = labelCount++;

		//INT 15H 86H
		fprintf(fp, "\n\n;FW :: MOVE AHEAD!\n");
		fprintf(fp, "PUSHA\n");

		//AX has present value. Has to be moved ahead DI times from here.		
		code_expr(root->children[1]);
		fprintf(fp, "POP DI\n");
		fprintf(fp, "MOV DX, %d\n", ch1xOffset);
		
		fprintf(fp, ";SET GRAPHICS MODE\n");
		fprintf(fp, "MOV AH, 00H\n");
 		fprintf(fp, "MOV AL, 3\n");
 		fprintf(fp, "INT 10H\n");
		
		fprintf(fp, "L%d:\n", print_label);
 		fprintf(fp, "MOV AH, 02H\n");
 		//fprintf(fp, "MOV DL, BL\n");
 		//fprintf(fp, "MOV DH, 40\n");
 		fprintf(fp, "MOV BH, 0\n");
 		fprintf(fp, "INT 10H\n");

 		/*AH = 09h
		AL = character to display.
		BH = page number.
		BL = attribute.
		CX = number of times to write character.

		*/
 		fprintf(fp, "MOV AH, 09H\n");
 		fprintf(fp, "MOV AL, 32		;display character\n");
 		fprintf(fp, "MOV BH, 0\n");
 		fprintf(fp, "MOV BL, 01111011B\n");
 		fprintf(fp, "MOV CX, 1\n");
 		fprintf(fp, "INT 10H\n");

		fprintf(fp, ";WAIT\n");
 		fprintf(fp, "MOV AH, 86H\n");
 		fprintf(fp, "MOV CX, 0FH\n");
 		fprintf(fp, "PUSH DX\n");
 		fprintf(fp, "MOV DX, 4240H\n");
 		fprintf(fp, "INT 15H\n");
 		fprintf(fp, "POP DX\n");
		
		fprintf(fp, "DEC DI\n");
		fprintf(fp, "INC DX\n");
		fprintf(fp, "CMP DI, 0\n");
		fprintf(fp, "JG L%d\n", print_label);
		
		//fprintf(fp, "MOV AH, 00H\n");
 		//fprintf(fp, "MOV AL, 00H\n");
 		//fprintf(fp, "INT 10H\n");

		fprintf(fp, "POPA\n\n");
	}
}


int getHeight(AST_NODE root) {
	if (root->type == variable || root->type == constant_value || isIncrement(root)) {
		return 0;
	}
	
	int lh = getHeight(root->children[0]);
	int rh = getHeight(root->children[1]);
	if (lh >= rh) {
		return lh+1;
	}
	
	return rh+1;
}


// When we load array values into registers, then what to do with the register data structure