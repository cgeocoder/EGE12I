#ifndef __GENBYTECODE_H__
#define __GENBYTECODE_H__

#include <vector>
#include "ast.h"

enum ByteCode {
	bc_find,
	bc_replace,
	bc_jz,
	bc_clear_stack,
	bc_jmp,
	bc_or,

	bc_halt
};

int get_mem_index(std::string val, std::vector<std::string>& mem);
void gen_index_bc(AST ast, std::vector<ByteCode>& bc, std::vector<std::string>& mem);
std::vector<ByteCode> gen_bytecode(AST ast, std::vector<std::string>& mem);


#endif // __GENBYTECODE_H__