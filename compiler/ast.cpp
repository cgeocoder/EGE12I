#include "ast.h"

#ifdef _DEBUG

#include "debug.h"

#else

#define __debug_print_vec_ast(_arg1)

#endif // _DEBUG

inline static bool is_expr(TokenID id) {
	return (id >= expr_find) && (id <= expr_replace);
}

inline bool is_blocks(TokenID id) {
	return (id == full_block_cond) || (id == part_block_cond)  || (id == then_block)
		|| (id == else_block) || (id == block_while) || (id == blocks) || is_expr(id);
}

inline static bool is_expr_cond(TokenID t0) {
	return (t0 == expr_find) || (t0 == expr_bin_op);
}

inline static bool is_bin_op(TokenID t0) {
	return t0 == tok_or;
}

inline static bool is_expr_find(TokenID t0, TokenID t1, TokenID t2, TokenID t3) {
	return (t0 == tok_find) && (t1 == tok_lp) && (t2 == tok_literal) && (t3 == tok_rp);
}

inline static bool is_expr_bin_op(TokenID t0, TokenID t1, TokenID t2) {
	return is_expr_cond(t0) && is_bin_op(t1) && is_expr_cond(t2);
}

inline static bool is_expr_replace(TokenID t0, TokenID t1, TokenID t2, TokenID t3, TokenID t4, TokenID t5) {
	return (t0 == tok_replace) && (t1 == tok_lp) && (t2 == tok_literal) && (t3 == tok_comma) && (t4 == tok_literal) && (t5 == tok_rp);
}

inline static bool is_if_cond(TokenID t0, TokenID t1) {
	return (t0 == tok_if) && is_expr_cond(t1);
}

inline static bool is_then_block(TokenID t0, TokenID t1) {
	return (t0 == tok_then) && is_blocks(t1);
}

inline static bool is_else_block(TokenID t0, TokenID t1) {
	return (t0 == tok_else) && is_blocks(t1);
}

inline static bool is_end_if(TokenID t0, TokenID t1) {
	return (t0 == tok_end) && (t1 == tok_if);
}

inline static bool is_full_block_cond(TokenID t0, TokenID t1, TokenID t2, TokenID t3) {
	return (t0 == if_cond) && (t1 == then_block) && (t2 == else_block) && (t3 == end_if);
}

inline static bool is_part_block_cond(TokenID t0, TokenID t1, TokenID t2) {
	return (t0 == if_cond) && ((t1 == then_block) || (t1 == blocks)) && (t2 == end_if);
}

inline static bool is_while_cond(TokenID t0, TokenID t1) {
	return (t0 == tok_while) && is_expr_cond(t1);
}

inline static bool is_end_while(TokenID t0, TokenID t1) {
	return (t0 == tok_end) && (t1 == tok_while);
}

inline static bool is_block_while(TokenID t0, TokenID t1, TokenID t2) {
	return (t0 == while_cond) && (t1 == blocks) && (t2 == end_while);
}

inline static bool is_block_cond(TokenID t0, TokenID t1, TokenID t2) {
	return (t0 == while_cond) && (t1 == blocks) && (t2 == end_while);
}

inline static bool is_program(TokenID t0, TokenID t1, TokenID t2) {
	return (t0 == tok_begin) && (t1 == blocks) && (t2 == tok_end);
}

AST make_ast(std::vector<Token> tokens) {
	size_t tok_len = tokens.size();

	std::vector<AST> ast;

	for (int i = 0; i < tok_len; ++i) {
		AST tmp_ast;
		tmp_ast.token = tokens.at(i);
		tmp_ast.id = tokens.at(i).id;

		ast.push_back(tmp_ast);
	}

	auto remove_complite_token = [&](int _Off, int _Len) {
		for (int i = _Off + 1; i < tok_len - _Len; ++i) {
			ast[i] = ast[i + _Len];
		}

		for (int i = 0; i < _Len; ++i)
			ast.pop_back();

		tok_len -= _Len;
	};

	// Simple compound expressions

	for (int i = 0; i < tok_len; ++i) {
		TokenID tid0 = ((i < tok_len) ? ast[i].id : zero_tok);
		TokenID tid1 = ((i + 1 < tok_len) ? ast[i + 1].id : zero_tok);
		TokenID tid2 = ((i + 2 < tok_len) ? ast[i + 2].id : zero_tok);
		TokenID tid3 = ((i + 3 < tok_len) ? ast[i + 3].id : zero_tok);
		TokenID tid4 = ((i + 4 < tok_len) ? ast[i + 4].id : zero_tok);
		TokenID tid5 = ((i + 5 < tok_len) ? ast[i + 5].id : zero_tok);

		if (is_expr_find(tid0, tid1, tid2, tid3)) {
			ast[i].id = expr_find;
			ast[i].nodes.push_back(ast[i + 2]);
			remove_complite_token(i, 3);
			i = -1;
		}
		else if (is_expr_bin_op(tid0, tid1, tid2)) {
			ast[i + 1].nodes.push_back(ast[i]);
			ast[i + 1].nodes.push_back(ast[i + 2]);
			ast[i] = ast[i + 1];
			ast[i].id = expr_bin_op;
			remove_complite_token(i, 2);
			i = -1;
		}
		else if (is_expr_replace(tid0, tid1, tid2, tid3, tid4, tid5)) {
			ast[i].id = expr_replace;
			ast[i].nodes.push_back(ast[i + 2]);
			ast[i].nodes.push_back(ast[i + 4]);
			remove_complite_token(i, 5);
			i = -1;
		}
		else if (is_end_if(tid0, tid1)) {
			ast[i].id = end_if;
			remove_complite_token(i, 1);
			i = -1;
		}
		else if (is_end_while(tid0, tid1)) {
			ast[i].id = end_while;
			remove_complite_token(i, 1);
			i = -1;
		}
	}

	// Block part expressions

	for (int i = 0; i < tok_len; ++i) {

		TokenID tid0 = ((i < tok_len) ? ast[i].id : zero_tok);
		TokenID tid1 = ((i + 1 < tok_len) ? ast[i + 1].id : zero_tok);
		TokenID tid2 = ((i + 2 < tok_len) ? ast[i + 2].id : zero_tok);
		TokenID tid3 = ((i + 3 < tok_len) ? ast[i + 3].id : zero_tok);
		TokenID tid4 = ((i + 4 < tok_len) ? ast[i + 4].id : zero_tok);
		TokenID tid5 = ((i + 5 < tok_len) ? ast[i + 5].id : zero_tok);

		if (is_full_block_cond(tid0, tid1, tid2, tid3)) {
			ast[i].id = full_block_cond;
			ast[i].nodes.push_back(ast[i + 1]);
			ast[i].nodes.push_back(ast[i + 2]);
			remove_complite_token(i, 3);
			i = -1;
		}
		else if (is_part_block_cond(tid0, tid1, tid2)) {
			ast[i].id = part_block_cond;
			ast[i].nodes.push_back(ast[i + 1]);
			remove_complite_token(i, 2);
			i = -1;
		}
		else if (is_block_while(tid0, tid1, tid2)) {
			ast[i].id = block_while;
			ast[i].nodes.push_back(ast[i + 1]);
			remove_complite_token(i, 2);
			i = -1;
		}
		else if (is_program(tid0, tid1, tid2)) {
			ast[i].id = program;
			ast[i].nodes.push_back(ast[i + 1]);
			remove_complite_token(i, 2);
			i = -1;
		}
		else if (is_if_cond(tid0, tid1)) {
			ast[i].id = if_cond;
			ast[i].nodes.push_back(ast[i + 1]);
			remove_complite_token(i, 1);
			i = -1;
		}
		else if (is_then_block(tid0, tid1)) {
			ast[i].id = then_block;
			ast[i].nodes.push_back(ast[i + 1]);
			remove_complite_token(i, 1);
			i = -1;
		}
		else if (is_else_block(tid0, tid1)) {
			ast[i].id = else_block;
			ast[i].nodes.push_back(ast[i + 1]);
			remove_complite_token(i, 1);
			i = -1;
		}
		else if (is_while_cond(tid0, tid1)) {
			ast[i].id = while_cond;
			ast[i].nodes.push_back(ast[i + 1]);
			remove_complite_token(i, 1);
			i = -1;
		}
		else if (is_blocks(tid0) && (tid0 != blocks) && (tid0 != then_block) && (tid0 != else_block)) {
			AST block;
			block.id = blocks;

			block.nodes.push_back(ast[i]);

			ast[i] = block;
			i = -1;
		}
		else if ((is_blocks(tid0) && is_blocks(tid1)) && ((tid0 != then_block) && (tid1 != else_block))) {
			ast[i].nodes.push_back(ast[i + 1]);

			remove_complite_token(i, 1);
			i = -1;
		}
		else if (((tid0 == then_block) || (tid0 == else_block)) && (tid1 == blocks)) {
			ast[i].nodes.push_back(ast[i + 1]);

			remove_complite_token(i, 1);
			i = -1;
		}
	}

	if (ast.size() != 1) {
		__debug_print_vec_ast(ast);
		throw(std::exception("EGE12C: make_ast() failed: ast.size() != 1: incorrect sequence of tokens"));
	}

	return ast[0];
}
