h: parseTable.c hashtable.c stack.c lexer.c ast.c symbol_table.c semantic_check.c code_gen.c
	gcc -w -o h -g parseTable.c hashtable.c stack.c lexer.c ast.c symbol_table.c semantic_check.c code_gen.c -I.
