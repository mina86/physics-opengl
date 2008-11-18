#ifndef H_LEXER_HPP
#define H_LEXER_HPP

#include <stdio.h>

#include <string>
#include <stdexcept>


namespace mn {

namespace physics {


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
		T_VELOCITY = 256, T_SIZE, T_MASS, T_LIGHT, T_TEXTURE,
		T_COLOR, /* keywords */
		T_REAL, T_STRING /* tokens with parameters */
	};

	/** Returns a user readable name of a token. */
	static const char *tokenName(int token);


	union Value {
		float real;
		char *string;
	};


	/**
	 * Creates lexer reading from a file.  If \a theStream is \c NULL
	 * constructor will try to open file specified by \a theFilename,
	 * otherwise it will use \a theStream as input stream and assume
	 * theFilename is corresponding file name.
	 *
	 * \param theFilename file name.
	 * \param theStream input stream to read from or \c NULL.
	 * \throw error if file could not be opened.
	 */
	explicit Lexer(const char *theFilename)
		: filename(theFilename ? theFilename : stdin_filename),
		  stream(theFilename ? fopen(theFilename, "r") : stdin),
		  closeStream(theFilename) { }

	/**
	 * Creates lexer reading from a file.  If \a theStream is \c NULL
	 * constructor will try to open file specified by \a theFilename,
	 * otherwise it will use \a theStream as input stream and assume
	 * theFilename is corresponding file name.
	 *
	 * \param theFilename file name.
	 * \param theStream input stream to read from or \c NULL.
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
