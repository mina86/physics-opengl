/*
 * src/lib/lexer.cpp
 * Copyright 2010 by Michal Nazarewicz (mina86/AT/mina86/DOT/com)
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "lexer.hpp"

#include <ctype.h>

#include <fstream>

namespace mn {

namespace lib {

unsigned Lexer::getchar() {
	unsigned ch;
	do {
		ch = (unsigned char)stream.get();

		if (stream.bad()) {
			throw error(current, "error while reading file");
		} else if (stream.eof()) {
			return T_EOF;
		}

		previous = current;
		if (ch == '\n') {
			++current.line;
			current.column = 1;
		} else {
			++current.column;
		}
	} while (ch == '\0');
	return ch;
}

void Lexer::ungetchar(unsigned ch) {
	if (ch != T_EOF) {
		current.line = previous.line;
		current.column = previous.column;
		stream.putback(ch);
	}
}

unsigned Lexer::getdigits(std::string &str, unsigned ch) {
	do {
		if (ch != '_') {
			str += (char)ch;
		}
	} while (isdigit(ch = getchar()) || ch == '_');
	return ch;
}

unsigned Lexer::nextToken(Value &value, Location &location) {
	std::string str;
	unsigned ch;

	while ((ch = getchar()) != T_EOF && isspace(ch))
		/*nop*/;
	location.begin = previous;

	/* Keyword */
	if (isalpha(ch)) {
		do {
			str += (char)tolower(ch);
		} while (isalpha(ch = getchar()));
		ungetchar(ch);
		location.end = current;

		if (str.size() == 1) {
			return str[0];
		}

		if (!keywords) {
			throw error(location, error::msg(str, "unknown keywoard"));
		}

		Keywords::const_iterator it = keywords->find(str);
		if (it == keywords->end()) {
			throw error(location, error::msg(str, "unknown keywoard"));
		}
		value.string = str;
		return it->second;
	}

	/* String */
	if (ch == '"' || ch == '\'') {
		unsigned end = ch;
		while ((ch = getchar()) != end) {
			if (ch == '\n' || ch == T_EOF) {
				throw error(previous, current, "unexpected end of line or file");
			}
			str += (char)ch;
		}
		location.end = current;
		value.string = str;
		return T_STRING;
	}

	/* Not a number */
	if (!isdigit(ch) && ch != '.' && ch != '-' && ch != '+') {
		location.end = current;
		return ch;
	}

	/* Sign */
	if (ch == '-' || ch == '+') {
		unsigned sgn = ch;

		ch = getchar();

		bool dot = ch == '.';
		if (dot) {
			ch = getchar();
		}

		if (!isdigit(ch)) {
			ungetchar(ch);
			if (dot) {
				ungetchar('.');
			}
			return sgn;
		}

		str += (char)sgn;
		if (dot) {
			ungetchar(ch);
			ch = '.';
		}
	}

	/* Read digits before '.' if any */
	bool got = isdigit(ch);
	if (got) {
		ch = getdigits(str, ch);
	} else {
		str += '0';
	}

	/* Part after '.' */
	if (ch == '.') {
		ch = getchar();
		if (!isdigit(ch)) {
			ungetchar(ch);
			if (got) {
				goto got_number;
			}
			return '.';
		}

		str += '.';
		ch = getdigits(str, ch);
	}

	/* Exp */
	if (ch == 'e' || ch == 'E') {
		unsigned sgn = '+';
		str += (char)ch;
		ch = getchar();
		if (ch == '+' || ch == '-') {
			sgn = ch;
			ch = getchar();
		}

		if (isdigit(ch)) {
			str += 'e';
			str += (char)sgn;
			ch = getdigits(str, ch);
		}
	}

	/* Return number */
got_number:
	ungetchar(ch);
	location.end = current;
	value.real = locale.toFloat(str.c_str());
	return T_REAL;
}

const char *Lexer::tokenName(unsigned token, const Value &value) {
	static char buf[10];

	if (token == T_EOF) {
		return "EOF";
	} else if (token == T_REAL) {
		return "<number>";
	} else if (token == T_STRING) {
		return "<string>";
	} else if (token == '\'') {
		return "'\\''";
	} else if (token == '\\') {
		return "'\\\\'";
	} else if (token < 32 || (token >= 128 && token < 256)) {
		sprintf(buf, "'\\%o'", token);
		return buf;
	} else if ((unsigned)token < 128) {
		sprintf(buf, "'%c'", token);
		return buf;
	} else {
		/* Token */
		return value.string.c_str();
	}
}

static std::string message(const char *str1, unsigned len1,
                           const char *str2, unsigned len2) {
	std::string out;
	out.reserve(len1 + 2 + len2);
	out.append(str1, len1);
	out.append(": ", 2);
	out.append(str2, len2);
	return out;
}

std::string Lexer::error::msg(const std::string &msg1,
                              const std::string &msg2) {
	return message(msg1.c_str(), msg1.size(), msg2.c_str(), msg2.size());
}

std::string Lexer::error::msg(const std::string &msg1, const char *msg2) {
	return message(msg1.c_str(), msg1.size(), msg2, strlen(msg2));
}

std::string Lexer::error::msg(const char *msg1, const std::string &msg2) {
	return message(msg1, strlen(msg1), msg2.c_str(), msg2.size());
}

std::string Lexer::error::msg(const char *msg1, const char *msg2) {
	return message(msg1, strlen(msg1), msg2, strlen(msg2));
}

}

}
