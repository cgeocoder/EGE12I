#include "ege12clib.h"
#include "compiler/token.h"
#include "compiler/ast.h"
#include "compiler/genbc.h"

#define STACK_SIZE 127

std::string Editor::get_error(void) {
	std::string err = this->m_Errors.back();
	this->m_Errors.pop_back();
	return err;
}

bool Editor::compile_file(const std::string& _File) {
	std::ifstream file{ _File };

	if (!file.is_open())
		throw(std::exception("EGE12C: Editor::compile_file() failed: could't open the file"));

	std::string raw_file;
	std::string line;
	while (std::getline(file, line))
		raw_file += (line + '\n');

	file.close();

	return compile(raw_file);
}

bool Editor::compile(const std::string& text) {
	std::vector<Token> tokens = parse_tokens(text);

	if (!check_syntax(tokens, this->m_Errors, text)) {
		return false;
	}

	AST ast = make_ast(tokens);
	tokens.clear();

	this->m_Bc = gen_bytecode(ast, this->m_Mem);

	return true;
}

Editor::Editor(const std::string& _File) {
	this->m_Start = this->m_End = 0;
	compile_file(_File);
}

__declspec(noreturn) static void runtime_error(const char* fnc, const std::exception& ex) {
	printf("EGE12C: Runtime error %s: %s\n", fnc, ex.what());
	exit(-1);
}

std::string Editor::perform(const std::string& _Arg) {
	std::string reg_str = _Arg;
	size_t i = 0;
	bool stack[STACK_SIZE] = { 0 };
	size_t stack_counter = 0;

	std::string* memory = m_Mem.data();
	ByteCode* bc = m_Bc.data();

	if (bc == nullptr) {
		return "EGE12C: Editor::perform() failed: m_Bc.data() == nullptr: there is no current program";
	}

	this->m_Start = clock();
	for (;;) {
		switch (bc[i]) {
		case bc_clear_stack:
			stack_counter = 0;
			i += 1;
			break;
		case bc_find:
			try {
				stack[stack_counter++] = 
					reg_str.find(memory[bc[i + 1]]) != std::string::npos;
			}
			catch (std::exception ex) {
				runtime_error("find", ex);
			}

			i += 2;

			break;
		case bc_replace: {
			std::string arg1 = memory[bc[i + 1]];
			std::string arg2 = memory[bc[i + 2]];

			try {
				size_t first_seq_index = reg_str.find(arg1);
				reg_str = reg_str.substr(0, first_seq_index)
					+ arg2 + reg_str.substr(first_seq_index + arg1.length());
			}
			catch (std::exception ex) {
				runtime_error("replace", ex);
			}

			i += 3;

			break;
		}
		case bc_jz:

			if (!stack[--stack_counter])
				i = bc[i + 1];
			else
				i += 2;

			break;
		case bc_jmp:
			i = bc[i + 1];
			break;
		case bc_or: {
			bool arg1 = stack[--stack_counter];
			bool arg2 = stack[--stack_counter];

			stack[stack_counter++] = (arg1 || arg2);
			i += 1;
			break;
		}
		case bc_halt:
			goto _end;
		}
	}

_end:
	this->m_End = clock();
	return reg_str;
}

void Editor::print_bytecode(bool show_data) {
	for (size_t i = 0; i < this->m_Bc.size(); ++i) {
		if (this->m_Bc.at(i) == bc_find) {
			if (show_data)
				printf("%llu:\tfind %d\t\t# (%s)\n", i, this->m_Bc.at(i + 1), 
					this->m_Mem.at(this->m_Bc.at(i + 1)).c_str());
			else
				printf("%llu:\tfind %d\n", i, this->m_Bc.at(i + 1));

			i += 1;
		}
		else if (this->m_Bc.at(i) == bc_replace) {
			if (show_data)
				printf("%llu:\treplace %d %d\t# (%s) (%s)\n", i, this->m_Bc.at(i + 1), this->m_Bc.at(i + 2),
					this->m_Mem.at(this->m_Bc.at(i + 1)).c_str(), this->m_Mem.at(this->m_Bc.at(i + 2)).c_str());
			else
				printf("%llu:\treplace %d %d\n", i, this->m_Bc.at(i + 1), this->m_Bc.at(i + 2));

			i += 2;
		}
		else if (this->m_Bc.at(i) == bc_jz) {
			printf("%llu:\tjz [%d]\n", i, this->m_Bc.at(i + 1));
			i += 1;
		}
		else if (this->m_Bc.at(i) == bc_jmp) {
			printf("%llu:\tjmp [%d]\n", i, this->m_Bc.at(i + 1));
			i += 1;
		}
		else if (this->m_Bc.at(i) == bc_or) {
			printf("%llu:\tor\n", i);
		}
		else if (this->m_Bc.at(i) == bc_halt) {
			printf("%llu:\thalt\n", i);
		}
		else if (this->m_Bc.at(i) == bc_clear_stack) {
			printf("%llu:\tclear_stack\n", i);
		}
	}

	printf("\n");
}