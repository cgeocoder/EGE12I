#ifndef __TOKEN_H__
#define __TOKEN_H__

#include <cstdlib>
#include <string>
#include <vector>
// #include "../ege12clib.h"

enum TokenID {
	zero_tok = -1,
	tok_begin,
	tok_end,
	tok_while,
	tok_if,
	tok_then,
	tok_else,
	tok_find,
	tok_replace,
	tok_lp,
	tok_rp,
	tok_comma,
	tok_or,
	tok_literal,
	tok_invalid,

	expr_find,
	expr_bin_op,
	expr_replace,
	if_cond,
	then_block,
	else_block,
	end_if,
	full_block_cond,
	part_block_cond,
	while_cond,
	end_while,
	block_while,
	program,
	blocks,

	invalid_ast
};

// Token class
class Token {
public:
	TokenID id;
	int start;
	int end;
	int line;
	std::string val;
};

std::vector<Token> parse_tokens(std::string prog_src);
bool check_syntax(
	std::vector<Token> tokens,
	std::vector<std::string>& errors,
	const std::string raw_text
);

#endif // __TOKEN_H__