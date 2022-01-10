
#ifndef __AST_CALC_H__
#define __AST_CALC_H__

typedef enum {
	VALUE,
	OP_PLUS,
	OP_MINUS,
	OP_MUL,
	OP_DIV,
} TYPE;

typedef struct _AST_ {
	TYPE type;
	int val;
	struct _AST_* pleft;
	struct _AST_* pright;
} AST;

AST* alloc_ast();
void comsume_spaces(char* p, int* pi);
AST* comsume_digit(char* p, int* pi);
void print_line(int indent, char* s);
void print_ast(int indent, AST* ast);
int eval_ast(AST* ast);
void free_ast(AST* ast);
AST* make_ast_expr1(char* p, int* pi);
AST* make_ast_expr2(char* p, int* pi);
AST* make_ast_expr3(char* p, int* pi);

#endif
