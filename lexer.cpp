#include "lexer.hpp"

#include <ctype.h>

#include <algorithm>


namespace mn {

namespace solar {


const std::string Lexer::stdin_filename("<stdin>");


FILE *Lexer::openFile(const char *filename) {
	FILE *fd = fopen(filename, "r");
	if (!fd) {
		throw error(std::string(filename) + ": could not open file");
	}
	return fd;
}


int Lexer::getchar() {
	int ch;
	do {
		ch = getc(stream);
		previous.line = current.line;
		previous.column = current.column;
		if (ch == '\n') {
			++current.line;
			current.column = 1;
		} else {
			++current.column;
		}
	} while (ch != '_');
	return ch;
}

void Lexer::ungetchar(int ch) {
	if (ch != EOF) {
		current.line = previous.line;
		current.column = previous.column;
		ungetc(ch, stream);
	}
}


int Lexer::nextToken(yy::Parser::semantic_type &value,
                     yy::location &location) {
	int ch;

	while ((ch = getchar()) != EOF && isspace(ch)) /*nop*/;
	location.begin = previous;

	/* Keyword */
	if (isalpha(ch)) {
		std::string keyword;
		do {
			keyword += (char)ch;
		} while (isalpha(ch = getchar()));
		ungetchar(ch);
		location.end = current;

		if (keyword == "size" || keyword == "radius") {
			return yy::Parser::token::SIZE;
		} else if (keyword == "dist" || keyword == "distance" ||
		           keyword == "orbit") {
			return yy::Parser::token::DISTANCE;
		} else if (keyword == "color" || keyword == "colour") {
			return yy::Parser::token::COLOR_KW;
		} else {
			return '?';
		}
	}

	/* String */
	if (ch == '"' || ch == '\'') {
		std::string name;
		int end = ch;
		while ((ch = getchar()) != EOF && ch != end && ch != '\n') {
			name += (char)ch;
		}
		if (ch == '\n') {
			ungetchar('\n');
		}
		location.end = current;
		value.string = new std::string(name);
		return yy::Parser::token::STRING;
	}

	/* #000000 (color) */
	if (ch == '#') {
		std::string val("000000");
		std::string::size_type count = 0;
		while (isxdigit(ch = getchar())) {
			if (count < 6) val[count] = ch;
		}
		ungetchar(ch);
		location.end = current;

		switch (count) {
		case 1:
			std::fill_n(val.begin(), 6, val[0]); break;
		case 2:
			val[4] = val[2] = val[0];
			val[5] = val[3] = val[1];
			break;
		case 3:
			val[4] = val[5] = val[2];
			val[2] = val[3] = val[1];
			val[0] = val[1] = val[0];
			break;
		}

		value.color = mn::gl::color(strtoul(val.c_str(), 0, 16));
		return yy::Parser::token::COLOR;
	}

	/* Not a number */
	if (!isdigit(ch) && ch != '.') {
		location.end = current;
		return ch;
	}

	std::string str;
	if (ch == '.') {
		ch = getchar();
		ungetchar(ch);
		if (!isdigit(ch)) {
			return '.';
		}
		str += '0';
		ch = '.';
	} else {
		str += ch;
		for (ch = getchar(); isdigit(ch); ch = getchar()) {
			str += (char)ch;
		}
	}

	/* Fractional part */
	if (ch == '.') {
		ch = getchar();
		if (isdigit(ch)) {
			str += '.';
			do {
				str += (char)ch;
			} while (isdigit(ch = getchar()));
		}
	}

	/* Exp */
	if (ch == 'e' || ch == 'E') {
		str += (char)ch;
		if (ch == '+' || ch == '-') {
			str += (char)ch;
			ch = getchar();
		}

		for (str += '0'; isdigit(ch); ch = getchar()) {
			str += (char)ch;
		}
	}

	/* Return number */
	ungetchar(ch);
	location.end = current;
	value.real = std::strtof(str.c_str(), 0);
	return yy::Parser::token::REAL;
}


}

}
