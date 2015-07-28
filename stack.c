/***************************************************
SIDDHANT TULI			2012A7PS077P
SHUCHITA BANTHIA		2012A7PS011P
GROUP 14_1
***************************************************/


#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

ST_NODE push_stack(ST_NODE top, int val) {
	ST_NODE temp = (ST_NODE)(malloc(sizeof(struct stack_node)));
	temp->val = val;
	temp->next = NULL;

	if (top == NULL) {
		top = temp;
	}
	else {
		temp->next = top;
		top = temp;
	}
	
	return top;
}

ST_NODE pop_stack(ST_NODE top) {
	if (top == NULL) {
		return NULL;
	}
	else {
//		ret = top->val;
		top = top->next;
	}
	return top;
}

int getTop_stack(ST_NODE top) {
	if (top == NULL) {
		return NULL;
	}
	return top->val;
}
