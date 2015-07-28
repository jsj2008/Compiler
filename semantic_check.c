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

Error_vars error_flags;
int errno = 1;

void sem_chk_functionDecl(AST_NODE root);

FILE *fp_se;// = fopen("errors.txt", "w");

//int *expr_error_flag;// = (int *)malloc(sizeof(int));
//*expr_error_flag = 0;

void initialiseSemChk() {
	fp_se = fopen("errors.txt", "w");

	//expr_error_flag = (int *)malloc(sizeof(int));
	//*expr_error_flag = 0;
}

void finaliseSemChk() {
	fclose(fp_se);
}

void setErrorFlags(sem_er_types er_code, char varname[], int line_number) {
	error_flags.error_code = er_code;
	error_flags.line_number = line_number;
	strcpy(error_flags.variable_name, varname);
}

void setErrorFlags2(sem_er_types er_code, char varname[], int line_number, int correct_type, int type_of_mismatched_variable) {
	error_flags.error_code = er_code;
	error_flags.line_number = line_number;
	strcpy(error_flags.variable_name, varname);
	error_flags.correct_type = correct_type;
	error_flags.type_of_mismatched_variable = type_of_mismatched_variable;
}


void semantic_error() {
	if(error_flags.type_of_mismatched_variable == -1)
	{
		error_flags.type_of_mismatched_variable  = 113;
	}
	if(error_flags.correct_type == -1)
	{
		error_flags.correct_type  = 113;
	}
	switch(error_flags.error_code) {
		case funcReturnNumberMismatch:
			strcpy(error_flags.error_message, " Line No. %d :: Incorrect number of arguments returned, expected %d return values, instead found %d return values.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.correct_no_of_return_values, error_flags.passed_no_of_return_values);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.correct_no_of_return_values, error_flags.passed_no_of_return_values);
			break;
		case funcReturnTypeMismatch:
			strcpy(error_flags.error_message, " Line No. %d :: Incorrect type of return of expression no. %d, expected type %s, instead found return type %s.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.type_mismatch_position+1, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.type_mismatch_position+1, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case notAnArray:
			strcpy(error_flags.error_message, "Line No. %d :: Variable %s referenced as an array is not an array variable.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case incorrectArrayDimensions:
			strcpy(error_flags.error_message, "Line No. %d :: Variable %s is an array of %d dimensions, it is used as a %d dimensional array.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name, error_flags.max_no_of_array_dimensions, error_flags.error_in_dimension_num);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name, error_flags.max_no_of_array_dimensions, error_flags.error_in_dimension_num);
			break;
		case arrayOutOfBounds:
			strcpy(error_flags.error_message, "Line No. %d :: Array out of bounds exception at dimension %d. Max size %d, received size %d.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.error_in_dimension_num, error_flags.maximum_array_bound, error_flags.recvd_array_bound);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.error_in_dimension_num, error_flags.maximum_array_bound, error_flags.recvd_array_bound);
			break;
		case funcParameterNumMismatch:
			strcpy(error_flags.error_message, "Line No. %d :: Incorrect number of parameters to function. Expected %d arguments, received %d arguments.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.correct_no_of_parameters, error_flags.passed_no_of_parameters);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.correct_no_of_parameters, error_flags.passed_no_of_parameters);
			break;
		case funcParamTypeMismatch:
			strcpy(error_flags.error_message, "Line No. %d :: Incorrect type of value passed to parameter %d, expected type %s, instead found return type %s.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.type_mismatch_position, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.type_mismatch_position, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case conditionalTypeMismatch:
			strcpy(error_flags.error_message, "Line No. %d :: Incorrect type of conditional expression, expected type BOOLEAN, instead found type %s.\n");
			printf(error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case loopTypeMismatch:
			strcpy(error_flags.error_message, "Line No. %d :: EXPRESSION IN FOR LOOP MUST BE OF TYPE BOOLEAN, INSTEAD FOUND EXPRESSION OF type %s.\n");
			printf(error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case operandTypeMismatch:
			//strcpy(error_flags.error_message, "Line No. %d :: Type mismatch in parameter 1, expected type POINT, instead found type %s.");
			printf(error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case invalidParamName:
			strcpy(error_flags.error_message, "Line No. %d :: Unknown parameter %s found.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case invalidOperands:
			strcpy(error_flags.error_message, "Line No. %d :: Invalid operands to %s, got operands of type %s and %s\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case invalidOperandsBot:
			strcpy(error_flags.error_message, "Line No. %d :: Invalid operands to %s, expected type %s instead got %s\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case invalidOperandsLogEq:
			strcpy(error_flags.error_message, "Line No. %d :: Can't compare %s and %s\n");
			printf(error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			break;	
		case invalidFunctionCallInExpression:
			strcpy(error_flags.error_message, "Line No. %d :: Invalid call to function %s, functions in expressions should return only 1 value.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case invalidFunctionCallInAssignment:
			strcpy(error_flags.error_message, "Line No. %d :: Invalid call to function %s, functions in assignments cannot have void return type.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case botOrPointMisused:
			strcpy(error_flags.error_message, "Line No %d :: Can't use bot, point type variable %s in expression. Use := to assign bot and point.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case invalidArrayReference:
			strcpy(error_flags.error_message, "Line No %d :: Can't use array %s in expressions \n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case invalidOperandsUnary:
			strcpy(error_flags.error_message, "Line No %d :: Invalid operand to unary operator. Expected INT, found %s.\n");
			printf(error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case invalidSplice:
			printf(error_flags.error_message, error_flags.line_number);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number);
			break;
		case invalidAssignment:
			strcpy(error_flags.error_message, "Line No. %d :: Invalid assignment. Unequal number of entities on both sides of assignment. Left: %d, right: %d\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.type_of_mismatched_variable, error_flags.correct_type);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.type_of_mismatched_variable, error_flags.correct_type);
			break;
		case invalidAssignmentType:
			strcpy(error_flags.error_message, "Line No. %d :: Invalid assignment. Can't assign variable ( %s ) of type %s to type %s.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name,tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case referencedBeforeDecl:
			strcpy(error_flags.error_message, "Line No. %d :: Variable %s referenced before declaration.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case missingReturnStmt:
			strcpy(error_flags.error_message, "Line No. %d :: Missing return statement. Function should return %d values.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.correct_no_of_return_values);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.correct_no_of_return_values);
			break;
		case invalidValueDecl:
			strcpy(error_flags.error_message, "Line No. %d :: Invalid value assigned to %s. Expected type %s, instead found %s.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name, tnt_arr[error_flags.correct_type], tnt_arr[error_flags.type_of_mismatched_variable]);
			break;
		case arrayIncr:
			//strcpy(error_flags.error_message, "Line No. %d :: Arrays cannot be incremented.\n");
			printf(error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number, error_flags.variable_name);
			break;
		case spliceError:
			printf("Line No. %d :: Splice error. First index cannot be less than second.\n", error_flags.line_number);
			fprintf(fp_se, "Line No. %d :: Splice error. First index cannot be less than second.\n", error_flags.line_number);
			break;
		case invalidArrayUse:
			printf(error_flags.error_message, error_flags.line_number);
			fprintf(fp_se, error_flags.error_message, error_flags.line_number);
			break;
		case variableRedeclaration:
			printf("Line No. %d :: Redeclaration of symbol: %s, which is already declared in current scope.\n", error_flags.line_number, error_flags.variable_name);
			fprintf(fp_se, "Line No. %d :: Redeclaration of symbol: %s, which is already declared in current scope.\n", error_flags.line_number, error_flags.variable_name);
			break;

	}
	ERROR_FLAG = 1;
	char fun[20];
	gets(fun);
	if(errno == 1)
	{
		char fun[20]; gets(fun);
	}
	errno++;
}

void sem_chk_functionDecl(AST_NODE root) {
	
	//printf("checking function decl\n");
	//char fun[20]; gets(fun);
	int ret_num = root->children[2]->num_children;
	AST_NODE return_stmt_node = root->children[2]->children[ret_num-1];

	int i, count = root->children[0]->num_children;
	AST_NODE ast_ret_node = root->children[0];

	if (count > 0 && return_stmt_node->type != ret) {
		// ERROR - INCORRECT NUMBER OF RETURN VALUES
		error_flags.line_number = return_stmt_node->token.lineNo;
		error_flags.correct_no_of_return_values = count;
		error_flags.error_code = missingReturnStmt;

		semantic_error();
	}
	else if (count != return_stmt_node->num_children) {
		// ERROR - INCORRECT NUMBER OF RETURN VALUES
		error_flags.line_number = return_stmt_node->token.lineNo;
		error_flags.correct_no_of_return_values = count;
		error_flags.passed_no_of_return_values = return_stmt_node->num_children;
		error_flags.error_code = funcReturnNumberMismatch;

		semantic_error();
	}
	else {
		for (i=0; i<count; i++) {
			if (ast_ret_node->children[i]->token.type != return_stmt_node->children[i]->type_extract->type) {
				// ERROR - TYPE MISMATCH - RETURN TYPE NOT AS EXPECTED
				// Which 
				error_flags.line_number = return_stmt_node->token.lineNo;
				error_flags.type_mismatch_position = i;
				error_flags.type_of_mismatched_variable = return_stmt_node->children[i]->type_extract->type;
				error_flags.correct_type = ast_ret_node->children[0]->token.type;
				error_flags.error_code = funcReturnTypeMismatch;

				semantic_error();
			}
		}
	}

	//isArray == 0 || isArray == num_children
}

void sem_chk_array(AST_NODE root) {
	// dimensions check
	//printf("checking array\n");

	// splice check
	if (root->num_children == 1 && root->children[0]->type == op_splice) {
		sem_chk_splice_bounds(root);
		//printf("back\n");
		return;
	}

	if (root->num_children > root->type_extract->isArray) {
		if (root->type_extract->isArray == 0) {
			// ERROR - VARIABLE IS REFERENCED AS AN ARRAY BUT ISN'T ACTUALLY AN ARRAY
			error_flags.line_number = root->token.lineNo;
			
			strcpy(error_flags.variable_name, root->token.str);
			error_flags.error_code = notAnArray;

			semantic_error();
			return;
		}
		else {
			// ERROR - MORE NUMBER OF ARRAY SUBSCRIPTS THAN POSSIBLE
			error_flags.line_number = root->token.lineNo;

			strcpy(error_flags.variable_name, root->token.str);
			error_flags.error_code = incorrectArrayDimensions;
			error_flags.max_no_of_array_dimensions = root->type_extract->isArray;
			error_flags.error_in_dimension_num = root->num_children;

			semantic_error();
			return;
		}
	}

	
	/*if (root->num_children == 1 && root->children[0]->type == op_splice) {
		printf("splice error\n");
		if (root->children[0]->children[0]->token.num > root->children[0]->children[1]->token.num) {
			
			error_flags.line_number = root->token.lineNo;
			error_flags.error_code = spliceError;

			semantic_error();
		}

		if (root->children[0]->children[0]->token.num > root->type_extract->bound1) {
				// ERROR - OUT OF BOUNDS EXCEPTION AT INDEX 1
				error_flags.line_number = root->token.lineNo;
				
				strcpy(error_flags.variable_name, root->token.str);
				error_flags.error_code = arrayOutOfBounds;
				error_flags.maximum_array_bound = root->type_extract->bound1;
				error_flags.recvd_array_bound = root->children[0]->children[0]->token.num;
				error_flags.error_in_dimension_num = 1;

				semantic_error();
		}
		if (root->children[0]->children[1]->token.num > root->type_extract->bound1) {
				// ERROR - OUT OF BOUNDS EXCEPTION AT INDEX 1
				error_flags.line_number = root->token.lineNo;
				
				strcpy(error_flags.variable_name, root->token.str);
				error_flags.error_code = arrayOutOfBounds;
				error_flags.maximum_array_bound = root->type_extract->bound1;
				error_flags.recvd_array_bound = root->children[0]->children[1]->token.num;
				error_flags.error_in_dimension_num = 1;

				semantic_error();
		}
		return;
	}*/

	// bounds checking
	//printf("haha %d\n", root->children[0]->type);
	if (root->num_children > 0) {
		if (root->children[0]->type == constant_value) {
			if (root->children[0]->token.num >= root->type_extract->bound1 || root->children[0]->token.num < 0) {
				// ERROR - OUT OF BOUNDS EXCEPTION AT INDEX 1
				error_flags.line_number = root->token.lineNo;
				
				strcpy(error_flags.variable_name, root->token.str);
				error_flags.error_code = arrayOutOfBounds;
				error_flags.maximum_array_bound = root->type_extract->bound1;
				error_flags.recvd_array_bound = root->children[0]->token.num;
				error_flags.error_in_dimension_num = 1;

				semantic_error();
			}
		}
	}
	if (root->num_children > 1) {
		
		if (root->children[1]->type == constant_value) {
			if (root->children[1]->token.num >= root->type_extract->bound2 || root->children[0]->token.num < 0) {
				// ERROR - OUT OF BOUNDS EXCEPTION AT INDEX 2
				error_flags.line_number = root->token.lineNo;

				strcpy(error_flags.variable_name, root->token.str);
				error_flags.error_code = arrayOutOfBounds;
				error_flags.maximum_array_bound = root->type_extract->bound2;
				error_flags.recvd_array_bound = root->children[1]->token.num;
				error_flags.error_in_dimension_num = 2;

				semantic_error();
			}
		}
	}
}


void sem_chk_functionCall(AST_NODE root) {
	SYM_NODE function_scope;
	//printf("chk1\n");
	function_scope = getScopeNode(root->token.str);
	//printf("chk2\n");

	int scopeResTableIndex = getFunctionId(root->token.str);
	int param_count = scopeResTable[scopeResTableIndex].no_of_params;
	//printf("chk3 should be %d, %d\n", param_count, root->num_children/2);

	char *var_names_checked = malloc(sizeof(char)*35*param_count);
	int i;

	if (param_count != root->num_children/2) {
		// ERROR - INCORRECT NUMBER OF PARAMETERS
		error_flags.line_number = root->children[0]->token.lineNo;

		//strcpy(error_flags.variable_name, root->token.str);
		error_flags.error_code = funcParameterNumMismatch;
		error_flags.correct_no_of_parameters = param_count;
		error_flags.passed_no_of_parameters = root->num_children/2;

		semantic_error();
	}
	else {
		//printf("No. of children %d\n", root->num_children);
		for (i=0; i<root->num_children/2; i+=2) {
			//printf("stage 1\n");
			if (function_scope == NULL) {
				printf("scope is null\n");
			}

			//printf("%s\n", root->children[i]->token.str);
			if (lookup_SymTable(function_scope, root->children[i]->token.str, hashSymTable(root->children[i]->token.str, BUCKET_PRIME), 1) == -1) {
				// ERROR - INCORRECT PARAMETER
				strcpy(error_flags.variable_name, root->children[i]->token.str);
				error_flags.line_number = root->children[i]->token.lineNo;
				error_flags.error_code = invalidParamName;
				// add is_recursive flag
				semantic_error();
			}
			else if (lookup_SymTable(function_scope, root->children[i]->token.str, hashSymTable(root->children[i]->token.str, BUCKET_PRIME), 0) != root->children[i+1]->type_extract->type) {
				// ERROR - INCORRECT TYPE VALUE PASSED TO PARAMETER
				error_flags.line_number = root->children[0]->token.lineNo;
				error_flags.type_mismatch_position = i;
				error_flags.type_of_mismatched_variable = root->children[i+1]->type_extract->type;
				error_flags.correct_type = lookup_SymTable(function_scope, root->children[i]->token.str, hashSymTable(root->children[i]->token.str, BUCKET_PRIME), 0);
				error_flags.error_code = funcParamTypeMismatch;

				semantic_error();
			}
		}
	}
}


void sem_chk_conditional(AST_NODE root) {
	if (root->children[0]->type_extract->type != type_boolean) {
		// ERROR - CONDITIONAL EXPRESSION MUST BE OF TYPE BOOLEAN, INSTEAD FOUND <TYPE>
		error_flags.line_number = root->token.lineNo;
		error_flags.error_code = conditionalTypeMismatch;
		error_flags.type_of_mismatched_variable = root->children[0]->type_extract->type;
		semantic_error();
	}
	
	/*if (root->num_children>2 && root->children[2]->type == conditional) {
		sem_chk_conditional(root->children[2]);
	}*/
}

void sem_chk_loop(AST_NODE root) {
	if (root->children[1]->type_extract->type != type_boolean) {
		//printf("Expression for FOR loop must be boolean expression.\n");
		// ERROR - EXPRESSION IN FOR LOOP MUST BE OF TYPE BOOLEAN, INSTEAD FOUND EXPRESSION OF <TYPE>
		error_flags.line_number = root->token.lineNo;
		error_flags.error_code = loopTypeMismatch;
		error_flags.type_of_mismatched_variable = root->children[1]->type_extract->type;
		semantic_error();

	}
}

void sem_chk_botNode(AST_NODE root) {

		if (root->token.type == find_hash(tnt_hash, "ADDV")) {
			if (root->children[0]->type_extract->type != type_point) {
				// ERROR - OPERAND 1 MUST BE OF TYPE POINT, INSTEAD FOUND <TYPE>
				error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.error_message, "Line No. %d :: Type mismatch in parameter 1, expected type POINT, instead found type %s.\n");
				error_flags.error_code = operandTypeMismatch;
				error_flags.type_of_mismatched_variable = root->children[0]->type_extract->type;

				semantic_error();
			}
			if (root->children[1]->type_extract->type != type_point) {			
				// ERROR - OPERAND 2 MUST BE OF TYPE POINT, INSTEAD FOUND <TYPE>
				error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.error_message, "Line No. %d :: Type mismatch in parameter 2, expected type POINT, instead found type %s.\n");
				error_flags.error_code = operandTypeMismatch;
				error_flags.type_of_mismatched_variable = root->children[1]->type_extract->type;

				semantic_error();
			}
		}
		else if (root->token.type == find_hash(tnt_hash, "FW")) {
		//case find_hash(tnt_hash, "FW"):
			if (root->children[0]->type_extract->type != type_bot) {
				// ERROR - OPERAND 1 MUST BE OF TYPE BOT, INSTEAD FOUND <TYPE> 
				error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.error_message, "Line No. %d :: Type mismatch in parameter 1, expected type BOT, instead found type %s.\n");
				error_flags.error_code = operandTypeMismatch;
				error_flags.type_of_mismatched_variable = root->children[0]->type_extract->type;

				semantic_error();
			}
			if (root->children[1]->type_extract->type != type_int) {
				// ERROR - OPERAND 2 MUST BE OF TYPE INT, INSTEAD FOUND <TYPE>
				error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.error_message, "Line No. %d :: Type mismatch in parameter 2, expected type POINT, instead found type %s.\n");
				error_flags.error_code = operandTypeMismatch;
				error_flags.type_of_mismatched_variable = root->children[0]->type_extract->type;

				semantic_error();
			}
			//break;
		}
		else if (root->token.type == find_hash(tnt_hash, "RT")) {
		//case find_hash(tnt_hash, "RT"):
			if (root->children[0]->type_extract->type != type_bot) {
				// ERROR - OPERAND 1 MUST BE OF TYPE BOT, INSTEAD FOUND <TYPE> 
				error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.error_message, "Line No. %d :: Type mismatch in parameter 1, expected type BOT, instead found type %s.\n");
				error_flags.error_code = operandTypeMismatch;
				error_flags.type_of_mismatched_variable = root->children[0]->type_extract->type;

				semantic_error();
			}
			//break;
		}
	}

void sem_chk_declaration(AST_NODE root) {
	// ALREADY CHECKED IN AST.C
	// ALSO DONE IN AST ITSELF FOR BOT AND POINT
}

void sem_chk_io(AST_NODE root) {
	//printf("check io\n");
	if (root->children[0]->type_extract->type != type_int) {
		//printf("ReadI must have an integer type variable catching the input.\n");
		// ERROR - WRONG TYPE OF VARIABLE CATCHING INPUT, MUST BE INT
		error_flags.line_number = root->token.lineNo;
		strcpy(error_flags.error_message, "Line No. %d :: WRONG TYPE OF VARIABLE CATCHING INPUT OF READI, MUST BE INT, instead found type %s\n");
		error_flags.error_code = operandTypeMismatch;
		error_flags.type_of_mismatched_variable = root->children[0]->type_extract->type;

		semantic_error();
	}
}

void sem_chk_unary(AST_NODE root) {
	if ((root->children[0]->type_extract->type != type_int)) {
		// ERROR - TYPE OF IDENTIFIER INCREMENTED MUST BE INTEGER, FOUND <TYPE>
		error_flags.line_number = root->children[0]->token.lineNo;
		strcpy(error_flags.error_message, "Line No. %d :: INCREMENT STATEMENT SUPPORTS ONLY INTEGER TYPE, INSTEAD FOUND TYPE %s\n");
		error_flags.error_code = operandTypeMismatch;
		error_flags.type_of_mismatched_variable = root->children[0]->type_extract->type;

		semantic_error();
	}
	else if ((root->children[0]->type_extract->isArray != root->children[0]->num_children)) {
		//printf("t:%d, l: %d, n:%s 3\n", root->children[0]->token.type, root->children[0]->token.lineNo, root->children[0]->token.str);

		error_flags.line_number = root->children[0]->token.lineNo;
		strcpy(error_flags.error_message, "Line No. %d :: ARRAY VARIABLE %s CAN'T BE INCREMENTED.\n");
		error_flags.error_code = arrayIncr;
		strcpy(error_flags.variable_name, root->children[0]->token.str);

		semantic_error();	
	}
}

void sem_chk_splice_bounds(AST_NODE var);
void sem_chk_assignment(AST_NODE root);


void sem_chk_splice_bounds(AST_NODE var)
{
	//check for possible errors!
	//printf("splice check...\n");
	int lb = var->children[0]->children[0]->token.num; 
	//printf("checking cases 1\n");
	int rb = var->children[0]->children[1]->token.num; 
	//printf("checking cases 2\n");
	
	astNodeType ltype = var->children[0]->children[0]->type;
	astNodeType rtype = var->children[0]->children[1]->type;

	//printf("checking cases 3: %d\n", var->type_extract->type);
	if(var->type_extract->isArray != 1)
	{
		//printf("case 1\n");
		error_flags.error_code = invalidSplice;
	 	error_flags.line_number = var->token.lineNo;
		strcpy(error_flags.error_message, "Line No. %d :: Splicing allowed for 1-D arrays only");
		semantic_error();
	}
	if( ltype == constant_value && rtype == constant_value && lb >= rb)
	{
		//printf("case 2\n");
		//Error: invalid splice indices , left bound greater than right bound
		error_flags.error_code = invalidSplice;
	 	error_flags.line_number = var->token.lineNo;
		strcpy(error_flags.error_message, "Line No. %d :: Invalid splice indices , left bound greater than right bound");
		semantic_error();
	}
	if (ltype == constant_value && lb<0)
	{
		//printf("case 3\n");
		//Error: invalid splice indices - NEgative indices not allowed
		error_flags.error_code = invalidSplice;
	 	error_flags.line_number = var->token.lineNo;
		strcpy(error_flags.error_message, "Line No. %d :: Invalid splice indices, Left Index Negative");
		semantic_error();
	}
	if (rtype == constant_value && rb<0)
	{
		//printf("case 4\n");
		//Error: invalid splice indices - NEgative indices not allowed
		error_flags.error_code = invalidSplice;
	 	error_flags.line_number = var->token.lineNo;
		strcpy(error_flags.error_message, "Line No. %d :: Invalid splice indices, Right Index Negative");
		semantic_error();
	}
	//printf("out\n");
}

void sem_chk_assignment(AST_NODE root)
{
	//ASsume type_int, type_float, type_boolean, type_bot, type_point exist

	AST_NODE left = root->children[0];
	AST_NODE right = root->children[1];
	AST_NODE temp;
	int leftEntities=0, rightEntities=0, bug, prev = -1;
	int lhscur = left->num_children;
	int rhscur = right->num_children;
	//array for holding num entities covered yet.. left and right
	//array for holding types 
	int left_num_ele[10], right_num_ele[10], left_types[20], right_types[20];
	int lhspos = 0, rhspos = 0, i;
	typeExtract t;

	for(i=0; i< lhscur; i++) {
		//printf("lhsCur $$$$$$$$$$$$$$$ %d, %d\n", i, lhscur);
		temp = left->children[i];
		bug = getNumberOfEntities(temp);
		leftEntities+=bug;
		left_num_ele[i] = bug+prev;
		t = temp->type_extract;
		if(t->type == type_bot || t->type == type_point)
		{
			//cant assign bot and points using this statement
			error_flags.error_code = botOrPointMisused;
			error_flags.line_number = temp->token.lineNo;
			strcpy(error_flags.variable_name,temp->token.str);
			semantic_error();
		}
		//on the left, it could be an array element/id => 1 element or a splice. 
		//need to check if theres sthg like..
		
		//type always same as root, except when its a function which appears on right
		else if(bug > 1 || (temp->num_children==1 && temp->children[0]->type == op_splice))
		{
			//If its a splice, insert types
			while(bug>0)
			{
				//if bug>1, all types same on left. 
				left_types[lhspos++] = t->type;
				--bug;
			}
			sem_chk_splice_bounds(temp);
			//printf("back2.\n");
		}
		else
		{
			int addFactor = 0;
			if(t->isArray != temp->num_children)
			{
				addFactor = t->isArray*300;  //==> could be 0 or 300 or 600 incase of arrays!!!
			}
			left_types[lhspos++] = t->type + addFactor;
		}
		
		prev = left_num_ele[i];
	}
	prev = -1;
	//printf("hello\n");
	for(i=0; i< rhscur; i++) {
		temp = right->children[i];
		//printf("temp children: %d\n", temp->num_children);
		//printf("temp gc: %d\n", temp->children[0]->num_children);
		//printf("before ent\n");
		
		bug = getNumberOfEntities(temp);
		//printf("after ent: %d\n", bug);
		rightEntities+=bug;
		right_num_ele[i] = bug+prev;
		//printf("its the type\n");
		t = temp->type_extract;
		//printf("checkpoint 0\n");
		if(t->type == type_bot || t->type == type_point)
		{
			//cant assign bot and points using this statement
			error_flags.error_code = botOrPointMisused;
			error_flags.line_number = temp->token.lineNo;
			strcpy(error_flags.variable_name,temp->token.str);
			semantic_error();
		}
		else if(temp->type == function)
		{
			int index = getFunctionId(temp->token.str);
			int ret = scopeResTable[index].no_of_return_vals;
			int k = 0;
			if(ret == 0)
			{
				error_flags.error_code = invalidFunctionCallInAssignment;
			 	error_flags.line_number = temp->token.lineNo;
				strcpy(error_flags.variable_name,temp->token.str);
				semantic_error();
			}
			while(k<ret)
			{
				right_types[rhspos++] = scopeResTable[index].returnValTypes[k];
				k++;
			}
		}
		else if(bug > 1 || (temp->num_children==1 && temp->children[0]->type == op_splice))
		{
			//If its a splice, insert types
			while(bug>0)
			{
				//if bug>1, all types same on left. 
				right_types[rhspos++] = t->type;
				--bug;
			}
			sem_chk_splice_bounds(temp);
			//printf("back3.\n");
		}
		else 
		{
			int addFactor = 0;
			if(t->isArray != temp->num_children)
			{
				addFactor = t->isArray*300;  //==> could be 0 or 300 or 600 incase of arrays!!!
			}
			right_types[rhspos++] = t->type + addFactor;
		}
	}
	//printf("checkpoint 1\n");
	if(leftEntities != rightEntities){
		//"Error at line %d, Can't complete assignment, Unequal number of entities on both sides\n", cur_ast_node->token.lineNo
		//printf("\n\nDEbug, entities dont match inside assignment! %d != %d\n\n", leftEntities, rightEntities);
		error_flags.error_code = invalidAssignment;
		error_flags.line_number = root->token.lineNo;
		error_flags.type_of_mismatched_variable = leftEntities;
	 	error_flags.correct_type = rightEntities;
		semantic_error();
	}
	else 
	{
		//check left_types and right_types
		//printf("\n\nDEbug, entities match. inside assignment. checking types. %d == %d\n\n", leftEntities, rightEntities);
		for(i=0; i<leftEntities; i++)
		{
			int l=0, r=0;
			while(left_num_ele[l]<i){
				l++;
			}
			while(right_num_ele[r]<i){
				r++;
			}
			if((left_types[i] == type_float && right_types[i] == type_int))
			{
				printf("\nTYPE COERCION IN LINE NUMBER %d, LEFT SIDE IS FLOAT, RIGHT SIDE IS INT \n", left->children[left_num_ele[l]]->token.lineNo);	
			}
			else if(left_types[i] != right_types[i]) 
			{
				//type mismatch!
				//set l and r!!
				//can't assign left->children[l] to right->children[r]

				/*printf("\n\nDebug: type mismatch i: %d, l: %d, r:%d\n", i, l, r);
				printf("left type: %d\n", left_types[i]);
				printf("right type: %d\n", right_types[i]);
				printf("\n\nDebug: type mismatch %d != %d", left_types[i],right_types[i]);
				*/
				error_flags.error_code = invalidAssignmentType;
				error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.variable_name,left->children[l]->token.str);
				int t1 = left_types[i]-600;
				if(t1<0) t1 = left_types[i] -300;
				if(t1<0) t1 = left_types[i];
				error_flags.correct_type = t1;
				t1 = right_types[i]-600;
				if(t1<0) t1 = right_types[i] -300;
				if(t1<0) t1 = right_types[i];
			 	error_flags.type_of_mismatched_variable = t1;
				//printf("calling sem error:-\n ");
				semantic_error();
			}
			else if(left_types[i]-600 > 0)
			{
				//its a 2d array, check bounds
				if (left->children[l]->type_extract->bound2 != right->children[r]->type_extract->bound2
				    || left->children[l]->type_extract->bound1 != right->children[r]->type_extract->bound1) 
				{
					//Unequal arrays
					//can't assign left->children[l] to right->children[r]
					//semantic_error(911);
				}
			}
			else if(left_types[i]-300 > 0)
			{
				//its a 1d array, check bounds
				if (left->children[l]->type_extract->bound1 != right->children[r]->type_extract->bound1) 
				{
					//Unequal arrays
					//can't assign left->children[l] to right->children[r]
					//semantic_error(911);
				}
			}
		}
	}

}

void assignTypes(AST_NODE root, int *expr_error_flag)
{
	
	if(root->type == dataType)
	{
		//might be typecast!
		int childType;
		childType = root->children[0]->type_extract->type;
		
		root->type_extract = makeNewTypeExtract(0,0,0,0);
		root->type_extract->type = root->token.type;
		root->type_extract->isArray = root->children[0]->type_extract->isArray;				
		root->type_extract->bound1 = root->children[0]->type_extract->bound1;				
		root->type_extract->bound2 = root->children[0]->type_extract->bound2;	
		//possible sem error			
	}
	else if(root->type == op)
	{
		int c1type = root->children[0]->type_extract->type;
		int c2type = root->children[1]->type_extract->type;
		//switch(root->token.type)
		//{
		if ((root->token.type == find_hash(tnt_hash, "LT")) || (root->token.type == find_hash(tnt_hash, "GT")) || (root->token.type == find_hash(tnt_hash, "LTE")) || (root->token.type == find_hash(tnt_hash, "GTE"))) {
			//case (int)find_hash(tnt_hash, "LT"):
			//case find_hash(tnt_hash, "GT"):
			//case find_hash(tnt_hash, "LTE"):
			//case find_hash(tnt_hash, "GTE"):
			 if ( (c1type != type_int && c1type != type_float) 
			 		|| (c2type != type_int && c2type != type_float))
			 {
			 	//invalid operands to op
			 	//expected type_boolean and type_boolean, got c1type & c2type
				error_flags.error_code = invalidOperands;
			 	error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.variable_name,tnt_arr[root->token.type]);
				error_flags.type_of_mismatched_variable = c1type;
			 	error_flags.correct_type = c2type;
			 	semantic_error();
			 	root->type_extract = makeNewTypeExtract(-1,0,0,0);
				*expr_error_flag = 1;				 
			}
			 else 
			 {
			 	root->type_extract = makeNewTypeExtract(type_boolean,0,0,0);
			 }
			 //break;
		}	
		else if ((root->token.type == find_hash(tnt_hash, "LOG_AND")) || (root->token.type == find_hash(tnt_hash, "LOG_OR"))) {
			
			 if (c1type != type_boolean && c2type != type_boolean) 
			 {
			 	//invalid operands to op
			 	//expected type_boolean and type_boolean, got c1type & c2type
				error_flags.error_code = invalidOperands;
			 	error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.variable_name,tnt_arr[root->token.type]);
				error_flags.type_of_mismatched_variable = c1type;
			 	error_flags.correct_type = c2type;
			 	semantic_error();
			 	root->type_extract = makeNewTypeExtract(-1,0,0,0);
				*expr_error_flag = 1;				 
			}
			 else 
			 {
			 	root->type_extract = makeNewTypeExtract(type_boolean,0,0,0);
			 }
			 //break;
		}
		else if ((root->token.type == find_hash(tnt_hash, "PLUS")) || (root->token.type == find_hash(tnt_hash, "MINUS")) || (root->token.type == find_hash(tnt_hash, "MULTIPLY")) || (root->token.type == find_hash(tnt_hash, "DIVIDE"))) {
			 if ( (c1type != type_int && c1type != type_float) 
			 		|| (c2type != type_int && c2type != type_float))
			 {
			 	//invalid operands to op
			 	//expected type_boolean and type_boolean, got c1type & c2type
				error_flags.error_code = invalidOperands;
			 	error_flags.line_number = root->token.lineNo;
				strcpy(error_flags.variable_name,tnt_arr[root->token.type]);
				error_flags.type_of_mismatched_variable = c1type;
			 	error_flags.correct_type = c2type;
			 	semantic_error();
			 	root->type_extract = makeNewTypeExtract(-1,0,0,0);
				*expr_error_flag = 1;
			 }
			 else 
			 {
			 	if ((c1type == type_float) || (c2type == type_float)) {
			 		//printf("\nMaking new node of type float\n");
			 		root->type_extract = makeNewTypeExtract(type_float,0,0,0);
			 	}
			 	else{
			 		root->type_extract = makeNewTypeExtract(type_int,0,0,0);
			 	}
			 }
			 //break;				
		}
		else if (root->token.type == find_hash(tnt_hash, "LOG_EQ")) {
		//case find_hash(tnt_hash, "LOG_EQ"):
			if(c1type != c2type)
			{
				error_flags.error_code = invalidOperandsLogEq;
			 	error_flags.line_number = root->token.lineNo;
				error_flags.type_of_mismatched_variable = c1type;
			 	error_flags.correct_type = c2type;
			 	semantic_error();
			 	root->type_extract = makeNewTypeExtract(-1,0,0,0);
				*expr_error_flag = 1;
			}
			else
			{
				root->type_extract = makeNewTypeExtract(c1type,0,0,0); 
			}
			//break;
		}
		else if ((root->token.type == find_hash(tnt_hash, "UNARY_INCR")) || (root->token.type == find_hash(tnt_hash, "UNARY_INCR"))) {

 		//case find_hash(tnt_hash, "UNARY_INCR"):	
		//case find_hash(tnt_hash, "UNARY_DECR"):
			if(c1type!=type_int)
			{
				error_flags.error_code = invalidOperandsUnary;
			 	error_flags.line_number = root->token.lineNo;
				error_flags.type_of_mismatched_variable = c1type;
			 	semantic_error();
			 	root->type_extract = makeNewTypeExtract(-1,0,0,0);
				*expr_error_flag = 1;	
			}
			else
			{
				root->type_extract = makeNewTypeExtract(c1type,0,0,0);
			}
			//break;
		}
	}		
	else
	{
		printf("\nDont know what type in epression!\n");
		char fun[10]; gets(fun);
	}
}

void sem_chk_expression(AST_NODE root, int *expr_error_flag)
{
	if (root->type == variable || root->type == constant_value)
	{
		//base case, its a variable, we've reached bottom of the tree.
		int t = root->type_extract->type;
		if(t == type_bot || t == type_point)
		{
			error_flags.error_code = botOrPointMisused;
			error_flags.line_number = root->token.lineNo;
			strcpy(error_flags.variable_name,root->token.str);
			semantic_error();
			*expr_error_flag = 1;
		}
		else if(root->type_extract->isArray != root->num_children)
		{
			error_flags.error_code = invalidArrayReference;
			error_flags.line_number = root->token.lineNo;
			strcpy(error_flags.variable_name,root->token.str);
			//cant use array in expressions!
			semantic_error();
			*expr_error_flag = 1;

		}
		return;
	}
	else if(root->type == functionCall)
	{
		//printf("I came here\n");
		int ret = getNoOfReturnValues(root->token.str);
		if(ret!=1)
		{
			//cant use function that doesnt return exactly one value in an expression
			error_flags.error_code = invalidFunctionCallInExpression;
		 	error_flags.line_number = root->token.lineNo;
			strcpy(error_flags.variable_name,root->token.str);
			semantic_error();
		 	root->type_extract = makeNewTypeExtract(-1,0,0,0);
			*expr_error_flag = 1;
		}
		else{
			
			int index = getFunctionId(root->token.str);
			root->type_extract = makeNewTypeExtract(scopeResTable[index].returnValTypes[0],0,0,0);
			//printf("I came here too\n");
		
		}
		return;
	}

	int i=0;
	while (i < root->num_children) 
	{
		sem_chk_expression(root->children[i], expr_error_flag);
		i++;
	}
	if(*expr_error_flag == 1)
	{
		root->type_extract = makeNewTypeExtract(-1,0,0,0);
	}
	else
	{
		assignTypes(root, expr_error_flag);
	}
}

