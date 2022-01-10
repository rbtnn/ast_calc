
#include <stdio.h>
#include <stdlib.h>
#include "ast_calc.h"

int indent_array[100];

int main(int argc, char* argv[])
{
	if (argc == 2)
	{
		int i = 0;
		AST* ast = make_ast_expr1(argv[1], &i);
		print_ast(0, ast);
		printf("= %d\n", eval_ast(ast));
		free_ast(ast);
	}

	return 0;
}

AST* alloc_ast()
{
	return (AST*) malloc(sizeof(AST));
}

void comsume_spaces(char* p, int* pi)
{
	for (; p[*pi] == ' '; (*pi)++);
}

AST* comsume_digit(char* p, int* pi)
{
	AST* ast = alloc_ast();
	int ok = 0;
	int i = *pi;
	int n = 0;
	while (('0' <= p[*pi]) && (p[*pi] <= '9'))
	{
		n = n * 10 + (p[*pi] - '0');
		ok = 1;
		(*pi)++;
	}
	if (ok)
	{
		ast->type = VALUE;
		ast->val = n;
		return ast;
	}
	else
	{
		*pi = i;
		free(ast);
		return NULL;
	}
}

void print_line(int indent, char* s)
{
	char* format = " %-5s";
	for (int i = 0; i < indent; i++)
	{
		if ((s[0] != '\0') && (i == indent - 1))
		{
			printf(format, "+---");
			indent_array[i] = !indent_array[i];
		}
		else if (indent_array[i])
			printf(format, "|");
		else
			printf(format, "");
	}
	printf("%s\n", s);
}

void print_ast(int indent, AST* ast)
{
	char buffer[10];

	if (indent == 0)
		for (int i = 0; i < sizeof(indent_array) / sizeof(indent_array[0]); i++)
			indent_array[i] = 0;

	if (ast != NULL)
	{
		switch (ast->type)
		{
			case VALUE:
				sprintf(buffer, "%d", ast->val);
				print_line(indent, buffer);
				print_line(indent + 1, "");
				break;

			case OP_PLUS:
				print_ast(indent + 1, ast->pleft);
				print_line(indent, "(+)");
				print_line(indent + 1, "");
				print_ast(indent + 1, ast->pright);
				break;

			case OP_MINUS:
				print_ast(indent + 1, ast->pleft);
				print_line(indent, "(-)");
				print_line(indent + 1, "");
				print_ast(indent + 1, ast->pright);
				break;

			case OP_MUL:
				print_ast(indent + 1, ast->pleft);
				print_line(indent, "(*)");
				print_line(indent + 1, "");
				print_ast(indent + 1, ast->pright);
				break;

			case OP_DIV:
				print_ast(indent + 1, ast->pleft);
				print_line(indent, "(/)");
				print_line(indent + 1, "");
				print_ast(indent + 1, ast->pright);
				break;

			default:
				printf("It seems there is an ast not printed!\n");
				break;
		}
	}
}

int eval_ast(AST* ast)
{
	int n = 0;
	if (ast != NULL)
	{
		switch (ast->type)
		{
			case VALUE:
				n = ast->val;
				break;

			case OP_PLUS:
				n = eval_ast(ast->pleft) + eval_ast(ast->pright);
				break;

			case OP_MINUS:
				n = eval_ast(ast->pleft) - eval_ast(ast->pright);
				break;

			case OP_MUL:
				n = eval_ast(ast->pleft) * eval_ast(ast->pright);
				break;

			case OP_DIV:
				n = eval_ast(ast->pleft) / eval_ast(ast->pright);
				break;

			default:
				printf("It seems there is an ast not evaluated!\n");
				break;
		}
	}
	return n;
}

void free_ast(AST* ast)
{
	switch (ast->type)
	{
		case VALUE:
			break;

		case OP_PLUS:
		case OP_MINUS:
		case OP_MUL:
		case OP_DIV:
			if (ast->pleft)
			{
				free_ast(ast->pleft);
				ast->pleft = NULL;
			}
			if (ast->pright)
			{
				free_ast(ast->pright);
				ast->pright = NULL;
			}
			break;

		default:
			printf("It seems there is a memory not freed!\n");
			break;
	}
	free(ast);
}

AST* make_ast_expr3(char* p, int* pi)
{
	comsume_spaces(p, pi);
	if ('(' == p[*pi])
	{
		(*pi)++;
		AST* e1 = make_ast_expr1(p, pi);
		if (')' == p[*pi])
		{
			(*pi)++;
			return e1;
		}
		else
		{
			free_ast(e1);
			return NULL;
		}
	}
	else
	{
		return comsume_digit(p, pi);
	}
}

AST* make_ast_expr2(char* p, int* pi)
{
	AST* e1 = make_ast_expr3(p, pi);
	while (e1 != NULL)
	{
		comsume_spaces(p, pi);
		int op_i = *pi;
		if (('*' == p[op_i]) || ('/' == p[op_i]))
		{
			(*pi)++;
			AST* e2 = make_ast_expr3(p, pi);
			if (e2 != NULL)
			{
				AST* op = alloc_ast();
				if ('*' == p[op_i])
					op->type = OP_MUL;
				else if ('/' == p[op_i])
					op->type = OP_DIV;
				op->pleft = e1;
				op->pright = e2;
				e1 = op;
			}
			else
				break;
		}
		else
			break;
	}
	return e1;
}

AST* make_ast_expr1(char* p, int* pi)
{
	AST* e1 = make_ast_expr2(p, pi);
	while (e1 != NULL)
	{
		comsume_spaces(p, pi);
		int op_i = *pi;
		if (('+' == p[op_i]) || ('-' == p[op_i]))
		{
			(*pi)++;
			AST* e2 = make_ast_expr2(p, pi);
			if (e2 != NULL)
			{
				AST* op = alloc_ast();
				if ('+' == p[op_i])
					op->type = OP_PLUS;
				else if ('-' == p[op_i])
					op->type = OP_MINUS;
				op->pleft = e1;
				op->pright = e2;
				e1 = op;
			}
			else
				break;
		}
		else
			break;
	}
	return e1;
}

