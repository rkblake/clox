#include "ast.h"
#include "parser.h"
#include <stdio.h>

#define NUM_REG 4
#define NOREG -1

static int free_reg[NUM_REG];
static char *reg_list[NUM_REG] = {"%r8", "%r9", "%r10", "%r11"};
static char *breg_list[NUM_REG] = {"%r8b", "%r9b", "%r10b", "%r11b"};
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

static int label() {
	static int id = 1;
	return id++;
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
	fprintf(out, "\taddq\t%s, %s\n", reg_list[r1], reg_list[r2]);
	free_register(r1);
	return r2;
}

int cg_subtract(int r1, int r2) {
	fprintf(out, "\tsubq\t%s, %s\n", reg_list[r2], reg_list[r1]);
	free_register(r1);
	return r2;
}

int cg_mult(int r1, int r2) {
	fprintf(out, "\timulq\t%s, %s\n", reg_list[r1], reg_list[r2]);
	free_register(r1);
	return r2;
}

int cg_divide(int r1, int r2) {
	fprintf(out, "\tmovq\t%s,%%rax\n", reg_list[r1]);
	fprintf(out, "\tcqo\n");
	fprintf(out, "\tidivq\t%s\n", reg_list[r2]);
	fprintf(out, "movq\t%%rax,%s\n", reg_list[r1]);
	free_register(r2);
	return r1;
}

static int cg_compare(int r1, int r2, char *how) {
	fprintf(out, "\tcmpq\t%s, %s\n", reg_list[r2], reg_list[r1]);
	fprintf(out, "\t%s\t%s\n", how, breg_list[r2]);
	fprintf(out, "\tandq\t$255,%s\n", reg_list[r2]);
	free_register(r1);
	return r2;
}

int cg_equal(int r1, int r2) { return cg_compare(r1, r2, "sete"); }
int cg_notequal(int r1, int r2) { return cg_compare(r1, r2, "setne"); }
int cg_lessthan(int r1, int r2) { return cg_compare(r1, r2, "setl"); }
int cg_greaterthan(int r1, int r2) { return cg_compare(r1, r2, "setg"); }
int cg_lessequal(int r1, int r2) { return cg_compare(r1, r2, "setle"); }
int cg_greaterequal(int r1, int r2) { return cg_compare(r1, r2, "setge"); }

void cg_printint(int r) {
	fprintf(out, "\tmovq\t%s, %%rdi\n", reg_list[r]);
	fprintf(out, "\tcall\tprintint\n");
	free_register(r);
}

int cg_loadglob(char *identifier) {
	int r = alloc_register();

	fprintf(out, "\tmovq\t%s(\%%rip), %s\n", identifier, reg_list[r]);
	return r;
}

int cg_storeglob(int r, char *identifier) {
	fprintf(out, "\tmovq\t%s, %s(\%%rip)\n", reg_list[r], identifier);
	return r;
}

void cg_label(int label) { fprintf(out, "L%d:\n", label); }
void cg_jump(int label) { fprintf(out, "\tjmp\tL%d\n", label); }
/* void cg_if(AstNode *n) { int Lfalse, Lend; } */

static char *cmp_list[] = {"sete", "setne", "setl", "setg", "setle", "setge"};
int cg_cmp_and_set(int ASTop, int r1, int r2) {
	if (ASTop < AST_EQ || ASTop > AST_GTE) {
		fprintf(stderr, "bad op in cg_cmp_and_set\n");
		exit(-1);
	}

	fprintf(out, "\tcmpq\t%s, %s\n", reg_list[r2], reg_list[r1]);
	fprintf(out, "\t%s\t%s\n", cmp_list[ASTop - AST_EQ], breg_list[r2]);
	fprintf(out, "\tmovzbq\t%s, %s\n", breg_list[r2], reg_list[r2]);
	free_all_registers();
	return r2;
}

static char *inv_cmp_list[] = {"jne", "je", "jge", "jle", "jg", "jl"};
int cg_cmp_and_jmp(int ASTop, int r1, int r2, int label) {
	if (ASTop < AST_EQ || ASTop > AST_GTE) {
		fprintf(stderr, "bad op in cg_cmp_and_jmp\n");
		exit(-1);
	}

	fprintf(out, "\tcmpq\t%s, %s\n", reg_list[r2], reg_list[r1]);
	fprintf(out, "\t%s\tL%d\n", inv_cmp_list[ASTop - AST_EQ], label);
	free_all_registers();
	return NOREG;
}

int gen_ast(AstNode *, int, int);

static int gen_if(AstNode *n) {
	int Lfalse, Lend;

	Lfalse = label();
	if (n->right) Lend = label();

	gen_ast(n->left, Lfalse, n->op);
	free_all_registers();

	gen_ast(n->mid, NOREG, n->op);
	free_all_registers();

	if (n->right) cg_jump(Lend);

	cg_label(Lfalse);

	if (n->right) {
		gen_ast(n->right, NOREG, n->op);
		free_all_registers();
		cg_label(Lend);
	}

	return NOREG;
}

void cg_globsym(char *sym) { fprintf(out, "\t.comm\t%s,8,8\n", sym); }

extern struct symbol_table sym[1024];

int gen_ast(AstNode *node, int reg, int parentASTop) {
	int left_reg, right_reg;

	switch (node->op) {
		case AST_IF: return gen_if(node);
		case AST_GLUE:
			gen_ast(node->left, NOREG, node->op);
			free_all_registers();
			gen_ast(node->right, NOREG, node->op);
			free_all_registers();
			return NOREG;
	}

	if (node->left) left_reg = gen_ast(node->left, NOREG, node->op);
	if (node->right) right_reg = gen_ast(node->right, left_reg, node->op);

	switch (node->op) {
		case AST_ADD: return cg_add(left_reg, right_reg);
		case AST_SUBTRACT: return cg_subtract(left_reg, right_reg);
		case AST_MULTIPLY: return cg_mult(left_reg, right_reg);
		case AST_DIVIDE: return cg_divide(left_reg, right_reg);
		case AST_INTLIT: return cg_load(node->int_value);
		case AST_IDENT: return cg_loadglob(sym[node->id].name);
		case AST_LVIDENT: return cg_storeglob(reg, sym[node->id].name);
		case AST_ASSIGNMENT: return right_reg;
		/* case AST_EQ: return cg_equal(left_reg, right_reg); */
		/* case AST_NE: return cg_notequal(left_reg, right_reg); */
		/* case AST_LT: return cg_lessthan(left_reg, right_reg); */
		/* case AST_GT: return cg_greaterthan(left_reg, right_reg); */
		/* case AST_LTE: return cg_lessequal(left_reg, right_reg); */
		/* case AST_GTE: return cg_greaterequal(left_reg, right_reg); */
		case AST_EQ:
		case AST_NE:
		case AST_LT:
		case AST_GT:
		case AST_LTE:
		case AST_GTE:
			if (parentASTop == AST_IF)
				return cg_cmp_and_jmp(node->op, left_reg, right_reg, reg);
			else
				return cg_cmp_and_set(node->op, left_reg, right_reg);

		default: fprintf(stderr, "unknow ast op: %d\n", node->op); exit(1);
	}
}

void generate_x86(AstNode *node) {
	out = fopen("./out.s", "w");
	int reg;

	cg_preamble();
	reg = gen_ast(node, NOREG, node->op);
	cg_printint(reg);
	cg_postamble();

	fclose(out);
}
