/*
 * src/lib/lexer.hpp
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
#ifndef H_LEXER_HPP
#define H_LEXER_HPP

#include <string>
#include <stdexcept>
#include <iostream>
#include <map>

#include <QLocale>

namespace mn {

namespace lib {

struct Lexer {
	struct Position {
		unsigned line, column;

		Position(unsigned theLine = 1, unsigned theColumn = 1)
			: line(theLine), column(theColumn) { }
	};

	struct Location {
		Position begin, end;

		Location(const Position &theBegin, const Position &theEnd)
			: begin(theBegin), end(theEnd) { }
		Location(const Position &position = Position())
			: begin(position), end(position) { }
	};

	struct error : public std::runtime_error {
		error(const struct Location &theLocation, const std::string &message)
			: std::runtime_error(message), location(theLocation) { }
		error(const struct Position &begin, const struct Position &end,
		      const std::string &message)
			: std::runtime_error(message), location(begin, end) { }
		error(const struct Position &position, const std::string &message)
			: std::runtime_error(message), location(position) { }

		const struct Location location;

		static std::string msg(const std::string &msg1,
		                       const std::string &msg2);
		static std::string msg(const char *msg1, const std::string &msg2);
		static std::string msg(const char *msg1, const char *msg2);
		static std::string msg(const std::string &msg1, const char *msg2);
	};

	enum {
		T_EOF,
		/* each character is returned as is */
		T_REAL = 256, T_STRING,
		T_FIRST_KEYWOARD,
	};

	typedef std::map<std::string, int> Keywords;

	struct Value {
		float real;
		std::string string;
	};

	/** Returns a user readable name of a token. */
	static const char *tokenName(unsigned token, const Value &value);

	/**
	 * Creates lexer reading from a file.  \a theFilename specifies
	 * file name of a file to open.
	 *
	 * \param theKeywoards list of keywoards.  Argument is cleared.
	 * \param in           the in stream, will be destroyed when
	 */
	Lexer(std::istream &in, const Keywords *theKeywords = NULL)
		: stream(in), locale("C"), keywords(theKeywords) {
	}

	/**
	 * Returns next token from stream.
	 * \param value structure to save value to.
	 * \param location structure to update location in stream of given
	 *        token.
	 * \return read token.
	 */
	unsigned nextToken(Value &value, Location &location);

	/** Returns current position in stream. */
	const Position &getPosition() const { return current; }

	const Keywords *getKeywords() const { return keywords; }
	void setKeywords(const Keywords *theKeywords) {
		keywords = theKeywords;
	}

private:
	/**
	 * Copying not allowed.
	 * \param lexer object to copy.
	 */
	Lexer();
	Lexer(const Lexer &);
	void operator=(const Lexer &);

	/** Stream to read data from. */
	std::istream &stream;
	/** Current position in stream. */
	Position current;
	/** A position in stream before last character was read. */
	Position previous;
	/** Locale class used to parse input. */
	QLocale locale;
	/** List of keywords. */
	const Keywords *keywords;

	/** Returns next character from file and updates location. */
	unsigned getchar();
	/**
	 * "Ungets" character from file and updates location.
	 * \param ch character to unget.
	 */
	void ungetchar(unsigned ch);
	unsigned getdigits(std::string &str, unsigned ch);
};

}

}

#endif
