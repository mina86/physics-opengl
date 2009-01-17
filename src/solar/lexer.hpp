/*
 * src/solar/lexer.hpp
 * Copyright 2009 by Michal Nazarewicz (mina86/AT/mina86/DOT/com)
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

#include <stdio.h>

#include <string>
#include <stdexcept>


namespace mn {

namespace solar {


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



	/**
	 * Named tokens.  Other tokens correspond directly to character
	 * codes.
	 */
	enum NamedTokens {
		T_ERROR = -2, T_EOF = -1, /* errors and such */
		T_FACTORS = 256, T_LIGHT, T_TEXTURE, /* keywords */
		T_REAL, T_STRING /* tokens with parameters */
	};

	/** Returns a user readable name of a token. */
	static const char *tokenName(int token);


	union Value {
		float real;
		char *string;
	};


	/**
	 * Creates lexer reading from a file.  \a theFilename specifies
	 * file name of a file to open.
	 *
	 * \param theFilename file name.
	 * \throw error if file could not be opened.
	 */
	explicit Lexer(const char *theFilename)
		: filename(theFilename), stream(fopen(theFilename, "r")),
		  closeStream(true) { }

	/**
	 * Creates lexer reading from a file.  \a theFilename specifies
	 * file name of a file to open.
	 *
	 * \param theFilename file name.
	 * \throw error if file could not be opened.
	 */
	explicit Lexer(const std::string &theFilename)
		: filename(theFilename), stream(fopen(theFilename.c_str(), "r")),
		  closeStream(true) { }

	/** Creates lexer reading from standard input.  */
	Lexer() : filename(stdin_filename), stream(stdin), closeStream(false) {
		current.line = current.column = 1;
		previous = current;
	}

	/** Closes stream if needed. */
	~Lexer() {
		if (closeStream && stream) {
			fclose(stream);
		}
	}


	bool operator!() const { return !stream; }


	/**
	 * Returns next token from stream.
	 * \param value structure to save value to.
	 * \param location structure to update location in stream of given
	 *        token.
	 * \return read token.
	 */
	int nextToken(Value &value, Location &location);

	/** Returns file name. */
	const std::string &getFileName() const { return filename; }

	/** Returns current position in stream. */
	const Position &getPosition() const { return current; }


private:
	/** Name of a standard input stream. */
	static const std::string stdin_filename;


	/**
	 * Copying not allowed.
	 * \param lexer object to copy.
	 */
	Lexer(const Lexer &lexer) { (void)lexer; }


	/** Name of the file we are reading from. */
	std::string filename;
	/** Stream to read data from. */
	FILE *stream;
	/** Whether stream should be closed by destructor. */
	bool closeStream;
	/** Current position in stream. */
	Position current;
	/** A position in stream before last character was read. */
	Position previous;


	/** Returns next character from file and updates location. */
	int getchar();
	/**
	 * "Ungets" character from file and updates location.
	 * \param ch character to unget.
	 */
	void ungetchar(int ch);
};


}

}

#endif
