#ifndef H_LEXER_HPP
#define H_LEXER_HPP

#include <stdio.h>

#include <string>
#include <stdexcept>

#include "parser.hpp"
#include "position.hh"
#include "location.hh"


namespace mn {

namespace solar {


struct Lexer {
	struct error : public std::runtime_error {
		error(const std::string &msg) : runtime_error(msg) { }
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
	explicit Lexer(const char *theFilename, FILE *theStream = 0)
		: filename(theFilename),
		  stream(theStream ? theStream : openFile(theFilename)),
		  closeStream(stream) {
		current.filename = &filename;
		current.line = current.column = 1;
		previous = current;
	}

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
	explicit Lexer(const std::string &theFilename, FILE *theStream = 0)
		: filename(theFilename),
		  stream(theStream ? theStream : openFile(theFilename.c_str())),
		  closeStream(stream) {
		current.filename = &filename;
		current.line = current.column = 1;
		previous = current;
	}

	/** Creates lexer reading from standard input.  */
	Lexer() : filename(stdin_filename), stream(stdin), closeStream(false) {
		current.filename = &filename;
		current.line = current.column = 1;
		previous = current;
	}

	/** Closes stream if needed. */
	~Lexer() {
		if (closeStream) {
			fclose(stream);
		}
	}


	/**
	 * Returns next token from stream.
	 * \param value structure to save value to.
	 * \param location structure to update location in stream of given
	 *        token.
	 * \return read token.
	 */
	int nextToken(yy::Parser::semantic_type &value,
	              yy::location &location);

	/** Returns current position in stream. */
	const yy::position &getPosition() const { return current; }


private:
	/**
	 * Opens given file in read only mode.
	 * \param filename name of the file to open.
	 * \return opened stream.
	 * \throw IOException on error.
	 */
	static FILE *openFile(const char *filename);

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
	yy::position current;
	/** A position in stream before last character was read. */
	yy::position previous;


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
