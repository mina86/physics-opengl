#include "lexer.hpp"

#include <string.h>
#include <ctype.h>

#include <algorithm>


namespace mn {

namespace physics {


const std::string Lexer::stdin_filename("<stdin>");


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
	} while (ch == '_');
	return ch;
}

void Lexer::ungetchar(int ch) {
	if (ch != EOF) {
		current.line = previous.line;
		current.column = previous.column;
		ungetc(ch, stream);
	}
}


struct Keyword {
	char name[12];
	unsigned short num;
};
static const Keyword keywords[] = {
	/* !!! KEEP THAT SORTED !!! */
	{ "auto",     Lexer::T_AUTO     },
	{ "color",    Lexer::T_COLOR    },
	{ "light",    Lexer::T_LIGHT    },
	{ "mass",     Lexer::T_MASS     },
	{ "size",     Lexer::T_SIZE     },
	{ "texture",  Lexer::T_TEXTURE  },
	{ "vel",      Lexer::T_VELOCITY },
	{ "velocity", Lexer::T_VELOCITY },
	{ "z",        Lexer::T_ERROR    } /* !!! KEEP IT HERE !!! */
};
static inline bool operator<(const Keyword &a, const Keyword &b) {
	return strcmp(a.name, b.name) < 0;
}
static inline bool operator<(const char *a, const Keyword &b) {
	return strcmp(a, b.name) < 0;
}
static inline bool operator<(const Keyword &a, const char *b) {
	return strcmp(a.name, b) < 0;
}


int Lexer::nextToken(Value &value, Location &location) {
	int ch;

	while ((ch = getchar()) != EOF && isspace(ch)) /*nop*/;
	location.begin = previous;

	/* Keyword */
	if (isalpha(ch)) {
		std::string keyword;
		do {
			keyword += (char)tolower(ch);
		} while (isalpha(ch = getchar()));
		ungetchar(ch);
		location.end = current;

		if (keyword.size() == 1) {
			return keyword[0];
		}

		const Keyword *kw = std::lower_bound(keywords, keywords + sizeof keywords / sizeof *keywords, keyword.c_str());
		return strcmp(kw->name, keyword.c_str()) ? (int)T_ERROR : kw->num;
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
		value.string = new char[name.size() + 1];
		std::copy(name.begin(), name.end(), value.string);
		value.string[name.size()] = 0;
		return T_STRING;
	}

	/* Not a number */
	if (!isdigit(ch) && ch != '.' && ch != '-') {
		location.end = current;
		return ch;
	}

	float mul = 1.0;
	std::string str;
	if (ch == '-') {
		ch = getchar();
		if (!isdigit(ch) && ch != '.') {
			ungetchar(ch);
			return '-';
		}
		mul = -1.0;
	}

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
		ch = getchar();
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
	value.real = mul * std::strtof(str.c_str(), 0);
	return T_REAL;
}



const char *Lexer::tokenName(int token) {
	static char buf[10];

	switch (token) {
	case T_ERROR:    return "ERROR";
	case T_EOF:      return "EOF";

	case '\'':       return "'\\''";
	case '\\':       return "'\\\\'";

	case T_VELOCITY: return "\"velocity\"";
	case T_SIZE:     return "\"size\"";
	case T_MASS:     return "\"mass\"";
	case T_LIGHT:    return "\"light\"";
	case T_TEXTURE:  return "\"texture\"";
	case T_COLOR:    return "\"color\"";

	case T_REAL:     return "number";
	case T_STRING:   return "string";

	default:
		if ((unsigned)token < 32 ||
		    ((unsigned)token >= 128 && (unsigned)token < 256)) {
			sprintf(buf, "'\\%o'", token);
		} else if ((unsigned)token < 128) {
			sprintf(buf, "'%c'", token);
		} else {
			return "UNKNOWN"; /* dead code */
		}
		return buf;
	}
}



}

}
