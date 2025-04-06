#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "token.h"
#include "ast.h"

static void __debug_print_vec_ast(const std::vector<AST>& ast) {
	for (auto& node : ast) {
		TokenID tid = node.id;

		switch (tid)
		{
		case zero_tok: 
			printf("zero_tok ");
			break;
		case tok_begin:
			printf("BEGIN ");
			break;
		case tok_end:
			printf("END ");
			break;
		case tok_while:
			printf("WHILE ");
			break;
		case tok_if:
			printf("IF ");
			break;
		case tok_then:
			printf("THEN ");
			break;
		case tok_else:
			printf("ELSE ");
			break;
		case tok_find:
			printf("find ");
			break;
		case tok_replace:
			printf("replace ");
			break;
		case tok_lp:
			printf("( ");
			break;
		case tok_rp:
			printf(") ");
			break;
		case tok_comma:
			printf(", ");
			break;
		case tok_or:
			printf("OR ");
			break;
		case tok_literal:
			printf("[%s] ", node.token.val.c_str());
			break;
		case tok_invalid:
			printf("tok_invalid ");
			break;
		case expr_find:
			printf("expr_find ");
			break;
		case expr_bin_op:
			printf("expr_bin_op ");
			break;
		case expr_replace:
			printf("expr_replace ");
			break;
		case if_cond:
			printf("if_cond ");
			break;
		case then_block:
			printf("then_block ");
			break;
		case else_block:
			printf("else_block ");
			break;
		case end_if:
			printf("end_if ");
			break;
		case full_block_cond:
			printf("full_block_cond ");
			break;
		case part_block_cond:
			printf("part_block_cond ");
			break;
		case while_cond:
			printf("while_cond ");
			break;
		case end_while:
			printf("end_while ");
			break;
		case block_while:
			printf("block_while ");
			break;
		case program:
			printf("program ");
			break;
		case blocks:
			printf("blocks ");
			break;
		case invalid_ast:
			printf("invalid_ast ");
			break;
		}
	}

	printf("\n");
}

#endif // __DEBUG_H__