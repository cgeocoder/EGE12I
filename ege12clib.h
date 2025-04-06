#ifndef __EGE_12_COMPILER_LIB_H__
#define __EGE_12_COMPILER_LIB_H__

#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include "compiler/genbc.h"

// The performer is the editor from task 12 of the "EGE" in Computer Science
class Editor {
private:
	bool compile(const std::string& text);

public:
	Editor() = default;
	~Editor() = default;
	Editor(const std::string& _File);

	// Attach and compile program from file
	bool compile_file(const std::string& _File);

	// Attach and compile program
	inline bool compile_program(const std::string& _Prog) { return compile(_Prog); }

	// Execute program
	// @param _Arg - the program's input line
	std::string perform(const std::string& _Arg);

	// Prints program compiled byte code
	// @param show_data - whether to show the data values
	void print_bytecode(bool show_data=true);

	// Returns last error
	std::string get_error(void);

	// Returns if there are any errors
	inline bool is_errors(void) { 
		return !this->m_Errors.empty(); 
	}

	// Returns the execution time from the last run in seconds
	inline double get_last_run_time(void) {
		return (double)(m_End - m_Start) / (double)(CLOCKS_PER_SEC);
	}

private:
	// std::string m_Raw_file;
	std::vector<std::string> m_Mem;
	std::vector<ByteCode> m_Bc;
	clock_t m_Start = 0, m_End = 0;

	std::vector<std::string> m_Errors;
};

// Literal operator to convert const char* to std::string
static inline std::string operator "" _s(const char* _String, size_t _Size) {
	return (std::string)(_String);
}

// Operator to multipli std::tring to int
static inline std::string operator*(std::string _Left, int _Right) {
	std::string res = _Left;
	for (int i = 0; i < _Right - 1; ++i) {
		res += _Left;
	}

	return res;
}


#endif // __EGE_12_COMPILER_LIB_H__