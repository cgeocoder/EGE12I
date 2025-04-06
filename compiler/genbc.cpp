#include "genbc.h"

int get_mem_index(std::string val, std::vector<std::string>& mem) {
	for (int i = 0; i < mem.size(); ++i) {
		if (val == mem.at(i)) {
			return i;
		}
	}

	mem.push_back(val);
	return (int)(mem.size() - 1);
}

void gen_index_bc(AST ast, std::vector<ByteCode>& bc, std::vector<std::string>& mem) {
	if (ast.id == program) {
		for (AST node : ast.nodes)
			gen_index_bc(node, bc, mem);

		bc.push_back(bc_halt);
	}
	else if (ast.id == expr_find) {
		bc.push_back(bc_find);
		bc.push_back((ByteCode)get_mem_index(ast.nodes[0].token.val, mem));
		return;
	}
	else if (ast.id == expr_replace) {		
		bc.push_back(bc_replace);
		bc.push_back((ByteCode)get_mem_index(ast.nodes[0].token.val, mem));
		bc.push_back((ByteCode)get_mem_index(ast.nodes[1].token.val, mem));
		return;
	}
	else if (ast.id == block_while) {
		int start_loop = (int)bc.size();

		gen_index_bc(ast.nodes[0], bc, mem);
		bc.push_back(bc_jz);
		bc.push_back((ByteCode)0);
		int jz = (int)(bc.size() - 1);

		gen_index_bc(ast.nodes[1], bc, mem);

		bc.push_back(bc_clear_stack);
		bc.push_back(bc_jmp);
		bc.push_back((ByteCode)start_loop);
		bc.at(jz) = (ByteCode)bc.size();
	}
	else if (ast.id == expr_bin_op) {
		gen_index_bc(ast.nodes[0], bc, mem);
		gen_index_bc(ast.nodes[1], bc, mem);

		if (ast.token.id == tok_or) {
			bc.push_back(bc_or);
		}
		else {
			__debugbreak();
		}
	}
	else if (ast.id == blocks || ast.id == else_block || ast.id == then_block) {
		for (AST node : ast.nodes) {
			gen_index_bc(node, bc, mem);
		}
	}
	else if (ast.id == full_block_cond) {

		// conditions 
		gen_index_bc(ast.nodes[0], bc, mem);

		bc.push_back(bc_jz);
		bc.push_back((ByteCode)0);
		int jz = (int)(bc.size() - 1);

		// if conditions in true
		gen_index_bc(ast.nodes[1], bc, mem);

		bc.push_back(bc_jmp);
		bc.push_back((ByteCode)0);
		int jmp = (int)(bc.size() - 1);
		bc.at(jz) = (ByteCode)(bc.size());

		gen_index_bc(ast.nodes[2], bc, mem);

		bc.at(jmp) = (ByteCode)(bc.size());
	}
	else if (ast.id == part_block_cond) {
		// conditions
		gen_index_bc(ast.nodes[0], bc, mem);

		bc.push_back(bc_jz);
		bc.push_back((ByteCode)0);
		int jz = (int)(bc.size() - 1);

		gen_index_bc(ast.nodes[1], bc, mem);

		bc.at(jz) = (ByteCode)(bc.size());
	}
}


std::vector<ByteCode> gen_bytecode(AST ast, std::vector<std::string>& mem) {
	std::vector<ByteCode> bc;
	gen_index_bc(ast, bc, mem);
	return bc;
}
