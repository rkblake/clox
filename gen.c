#include "ast.h"
#include "parser.h"
#include <stdio.h>

#define NUM_REG 4

static int free_reg[NUM_REG];
static char *reg_list[NUM_REG] = {"%r8", "%r9", "%r10", "%r11"};
FILE *out;

void free_all_registers() {
	free_reg[0] = free_reg[1] = free_reg[2] = free_reg[3] = 1;
}

static int alloc_register() {
	for (int i = 0; i < NUM_REG; i++) {
		if (free_reg[i]) {
			free_reg[i] = 0;
			return i;
		}
	}
	fprintf(stderr, "out of registers\n");
	exit(1);
}

static void free_register(int reg) {
	if (free_reg[reg] != 0) {
		fprintf(stderr, "Error trying to free register %d\n", reg);
		exit(1);
	}
	free_reg[reg] = 1;
}

void cg_preamble() {
	free_all_registers();
	fputs("\t.text\n"
		  ".LC0:\n"
		  "\t.string\t\"%d\\n\"\n"
		  "printint:\n"
		  "\tpushq\t%rbp\n"
		  "\tmovq\t%rsp, %rbp\n"
		  "\tsubq\t$16, %rsp\n"
		  "\tmovl\t%edi, -4(%rbp)\n"
		  "\tmovl\t-4(%rbp), %eax\n"
		  "\tmovl\t%eax, %esi\n"
		  "\tleaq .LC0(%rip), %rdi\n"
		  "\tmovl $0, %eax\n"
		  "\tcall printf@PLT\n"
		  "\tnop\n"
		  "\tleave\n"
		  "\tret\n"
		  "\n"
		  "\t.globl\tmain\n"
		  "\t.type\tmain, @function\n"
		  "main:\n"
		  "\tpushq\t%rbp\n"
		  "\tmovq %rsp, %rbp\n",
		  out);
}

void cg_postamble() {
	fputs("\tmovl $0, %eax\n"
		  "\tpopq %rbp\n"
		  "\tret\n",
		  out);
}

int cg_load(int value) {
	int r = alloc_register();
	fprintf(out, "\tmovq\t$%d, %s\n", value, reg_list[r]);
	return r;
}

int cg_add(int r1, int r2) {
	fprintf(out, "\taddq\t%s, %s\n", reg_list[r1], reg_list[2]);
	free_register(r1);
	return r2;
}

int cg_mult(int r1, int r2) {
	fprintf(out, "\timulq\t%s, %s\n", reg_list[r1], reg_list[r2]);
	free_register(r1);
	return r2;
}

static int gen_ast(AstNode *node) {
	int left_reg, right_reg;

	if (node->left) left_reg = gen_ast(node->left);
	if (node->right) right_reg = gen_ast(node->right);

	switch (node->op) {
		case AST_ADD: return cg_add(left_reg, right_reg);
		case AST_MULTIPLY: return cg_mult(left_reg, right_reg);
		case AST_INTLIT: return cg_load(node->int_value);

		default: fprintf(stderr, "unknow ast op\n"); exit(1);
	}
}

void generate_x86(AstNode *node) {
	out = fopen("./out.s", "w");
	int reg;

	cg_preamble();
	reg = gen_ast(node);
	cg_postamble();
}