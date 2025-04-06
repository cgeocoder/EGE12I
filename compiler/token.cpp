#include "token.h"
// #include "../ege12clib.h"

inline bool is_separator(char ch) {
	return (ch == ' ') || (ch == ',') || (ch == '\t') || (ch == '\n') || (ch == '(') || (ch == ')');
}

inline bool is_number(char ch) {
	return (ch >= '0') && (ch <= '9');
}

std::vector<Token> parse_tokens(std::string prog_src) {
	std::vector<Token> tokens;
	std::string target;

	int line_count = 1;
	int pos_count = 0;
	size_t prog_len = prog_src.length();
	for (int i = 0; i < prog_len; ++i) {
		char ch = prog_src.at(i);

		if (is_separator(ch)) {
			if (target != "") {
				if (target == "ÍÀ×ÀËÎ") {
					tokens.push_back({ tok_begin , pos_count - 6, pos_count, line_count });
				}
				else if (target == "ÏÎÊÀ") {
					tokens.push_back({ tok_while , pos_count - 4, pos_count, line_count });
				}
				else if (target == "ÅÑËÈ") {
					tokens.push_back({ tok_if , pos_count - 4, pos_count, line_count });
				}
				else if (target == "ÊÎÍÅÖ") {
					tokens.push_back({ tok_end , pos_count - 5, pos_count, line_count });
				}
				else if (target == "ÒÎ") {
					tokens.push_back({ tok_then , pos_count - 2, pos_count, line_count });
				}
				else if (target == "ÈÍÀ×Å") {
					tokens.push_back({ tok_else , pos_count - 5, pos_count, line_count });
				}
				else if (target == "íàøëîñü") {
					tokens.push_back({ tok_find , pos_count - 7, pos_count, line_count });
				}
				else if (target == "çàìåíèòü") {
					tokens.push_back({ tok_replace , pos_count - 8, pos_count, line_count });
				}
				else if (target == "ÈËÈ" || target == "èëè") {
					tokens.push_back({ tok_or , pos_count - 3, pos_count, line_count });
				}
				else {
					tokens.push_back({ tok_literal, pos_count - (int)target.length(), pos_count, line_count, target });
				}
			}

			if (ch == '(') {
				tokens.push_back({ tok_lp , pos_count, pos_count + 1, line_count });
			}
			else if (ch == ')') {
				tokens.push_back({ tok_rp , pos_count, pos_count + 1, line_count });
			}
			else if (ch == ',') {
				tokens.push_back({ tok_comma , pos_count, pos_count + 1, line_count });
			}
			else if (ch == '\n') {
				pos_count = -1;
				line_count += 1;
			}

			// __debugbreak();

			target.clear();
		}
		else {
			target += ch;
		}

		pos_count += 1;
	}

	return tokens;
}

bool check_syntax(std::vector<Token> tokens, std::vector<std::string>& errors, const std::string raw_text) {
	size_t len = tokens.size() - 2;
	bool ret = true;

	auto print_err = [&](Token t_left, Token t_current, Token t_right) {
		ret = false;

		size_t cntr = 0;
		size_t line_counter = 1;
		while (line_counter != t_current.line) {
			if (raw_text.at(cntr) == '\n')
				line_counter += 1;

			cntr += 1;
		}

		int spaces = 0;
		char ch = raw_text.at(cntr);
		while (ch == ' ' || ch == '\t') {
			cntr += 1;
			ch = raw_text.at(cntr);
			spaces += 1;
		}

		std::string text_info = "line " + std::to_string(t_current.line) + " | ";
		while (raw_text.at(cntr) != '\n') {
			text_info += raw_text.at(cntr++);
		}

		text_info += "\n    ";

		for (size_t t = 0; t < std::to_string(t_current.line).length(); ++t)
			text_info += " ";
		
		text_info += "+ | ";

		for (size_t i = 0; i < text_info.length(); ++i) {
			if (i >= (t_left.start - spaces) && i <= (t_right.start - spaces)) {
				text_info += '^';
			}
			else if (i > (t_right.start - spaces)) {
				break;
			}
			else {
				text_info += ' ';
			}
		}

		errors.push_back("EGE12C: invalid syntax:\n" + text_info + "\n");
	};

	for (size_t i = 0; i < len; ++i) {
		TokenID tid0 = tokens.at(i).id;
		TokenID tid1 = tokens.at(i + 1).id;

		if (tid0 == tok_begin) {
			if (tid1 != tok_end &&
				tid1 != tok_while &&
				tid1 != tok_replace &&
				tid1 != tok_if &&
				tid1 != tok_find) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
		else if (tid0 == tok_while) {
			if (tid1 != tok_find &&
				tid1 != tok_while &&
				tid1 != tok_if &&
				tid1 != tok_else &&
				tid1 != tok_end &&
				tid1 != tok_replace) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
		else if (tid0 == tok_find && tid1 != tok_lp) {
			print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
		}
		else if (tid0 == tok_lp && tid1 != tok_literal) {
			print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
		}
		else if (tid0 == tok_literal) {
			if (tid1 != tok_comma && 
				tid1 != tok_rp) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
		else if (tid0 == tok_rp) {
			if (tid1 != tok_or &&
				tid1 != tok_end &&
				tid1 != tok_if &&
				tid1 != tok_while &&
				tid1 != tok_then &&
				tid1 != tok_else &&
				tid1 != tok_replace &&
				tid1 != tok_find) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
		else if (tid0 == tok_if) {
			if (tid1 != tok_find &&
				tid1 != tok_while &&
				tid1 != tok_else &&
				tid1 != tok_if &&
				tid1 != tok_end &&
				tid1 != tok_replace) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
		else if (tid0 == tok_then) {
			if (tid1 != tok_replace &&
				tid1 != tok_if &&
				tid1 != tok_while &&
				tid1 != tok_find) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
		else if (tid0 == tok_else) {
			if (tid1 != tok_replace &&
				tid1 != tok_if &&
				tid1 != tok_while &&
				tid1 != tok_find) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
		else if (tid0 == tok_end) {
			if (tid1 != tok_if &&
				tid1 != tok_while) {

				print_err(tokens.at(i), tokens.at(i + 1), tokens.at(i + 2));
			}
		}
	}

	return ret;
}