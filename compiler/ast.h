#ifndef __AST_H__
#define __AST_H__

#include <vector>
#include "token.h"

class AST {
public:
	AST() = default;
	~AST() = default;

	TokenID id;
	Token token;
	std::vector<AST> nodes;
};

AST make_ast(std::vector<Token> tokens);


#endif // __AST_H__