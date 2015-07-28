/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/

enum er_types {
	funcReturnNumberMismatch, funcReturnTypeMismatch, notAnArray, 
	incorrectArrayDimensions, arrayOutOfBounds, funcParameterNumMismatch, 
	funcParamTypeMismatch, conditionalTypeMismatch, 
	loopTypeMismatch, operandTypeMismatch, botOrPointMisused, 
	invalidParamName, invalidOperands, invalidArrayReference, 
	invalidOperandsLogEq, invalidFunctionCallInExpression, 
	invalidFunctionCallInAssignment, invalidOperandsUnary,
	invalidSplice, invalidAssignment, invalidAssignmentType, 
	referencedBeforeDecl, missingReturnStmt, invalidValueDecl, arrayIncr, 
	spliceError, invalidArrayUse, invalidOperandsBot, variableRedeclaration
};
typedef enum er_types sem_er_types;

struct _error_vars {
	sem_er_types error_code;
	char variable_name[35];
	int line_number;
	int type_of_mismatched_variable;
	int correct_type;
	int type_mismatch_position;
	int correct_no_of_return_values;
	int passed_no_of_return_values;
	int correct_no_of_parameters;
	int passed_no_of_parameters;
	int maximum_array_bound;
	int recvd_array_bound;
	int max_no_of_array_dimensions;
	int error_in_dimension_num;
	char error_message[150];
	int error_parameter_num;
};
typedef struct _error_vars Error_vars;

extern Error_vars error_flags;