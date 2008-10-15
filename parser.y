   /*  Make EMACS happy: -*- Mode: C++; tab-width: 8; c-basic-offset: 8 -*- */
%skeleton "lalr1.cc"
%require "2.1a"
%defines
%define "parser_class_name" "Parser"

%{
#include <string>
#include <vector>

#include "color.hpp"

namespace mn { namespace solar {

struct Lexer;
struct Sphere;

} }

%}

%union {
	float             real;
	mn::gl::Color     color;
	std::string      *string;
	struct Attributes {
		float distance;
		float size;
		float period;
		mn::gl::Color color;
	}             attributes;
	struct Attribute {
		union {
			float real;
			mn::gl::Color color;
		} value;
		enum AttributeType {
			ATTR_DIST, ATTR_SIZE, ATTR_PERIOD, ATTR_COLOR
		} type;
	}             attribute;
	mn::solar::Sphere   *sphere;
};

%parse-param { mn::solar::Lexer   &lexer }
%lex-param   { mn::solar::Lexer   &lexer }
%parse-param { mn::solar::Sphere **rootSphere }
%parse-param { std::vector<mn::solar::Sphere*> sphereStack }

%debug
%error-verbose
%locations
%output = "parser.cpp"

%{
#include <stdio.h>

#include "planet.hpp"
#include "lexer.hpp"

int yylex(yy::Parser::semantic_type *yylval,
          yy::Parser::location_type *yylloc,
          mn::solar::Lexer &lexer);

%}


%token	<real>	REAL
%token	<color>	COLOR
%token	<string>	STRING
%token		SIZE		"size"
%token		DISTANCE	"distance"
%token		COLOR_KW	"color"

%type	<sphere>	sphere;
%type	<attributes>	attributes;
%type	<attribute>	attribute;

%destructor	{ delete $$; } STRING sphere

%%
start	: sphere		{ *rootSphere = $1; }
	;

sphere	: STRING attributes ';' {
		$$ = new mn::solar::Sphere($2.distance, $2.size, 1/$2.period, mn::gl::Color($2.color), *$1);
	}
	| STRING attributes '{' {
		sphereStack.push_back(new mn::solar::Sphere($2.distance, $2.size, 1/$2.period, mn::gl::Color($2.color), *$1));
	} spheres '}' {
		$$ = sphereStack.back();
		sphereStack.pop_back();
	}
	;

spheres	: sphere		{ sphereStack.back()->pushSatelite(*$1); }
	| spheres sphere	{ sphereStack.back()->pushSatelite(*$2); }
	;

attributes
	: /* espilon */		{
		$$.distance = $$.size = $$.period = 1.0;
		$$.color = mn::gl::color();
	}
	| attributes attribute	{
		$$ = $1;
		switch ($2.type) {
		case Parser::semantic_type::Attribute::ATTR_DIST:   $$.distance = $2.value.real ; break;
		case Parser::semantic_type::Attribute::ATTR_SIZE:   $$.size     = $2.value.real ; break;
		case Parser::semantic_type::Attribute::ATTR_PERIOD: $$.period   = $2.value.real ; break;
		case Parser::semantic_type::Attribute::ATTR_COLOR:  $$.color    = $2.value.color; break;
		}
	}
	;

attribute
	: "size"     REAL	{ $$.type = Parser::semantic_type::Attribute::ATTR_SIZE  ; $$.value.real  = $2 }
	| "distance" REAL	{ $$.type = Parser::semantic_type::Attribute::ATTR_DIST  ; $$.value.real  = $2 }
	| "period"   REAL	{ $$.type = Parser::semantic_type::Attribute::ATTR_PERIOD; $$.value.real  = $2 }
	| "color"    COLOR	{ $$.type = Parser::semantic_type::Attribute::ATTR_COLOR ; $$.value.color = $2 }
	| "color" '(' REAL ',' REAL ',' REAL ')' { $$.type = Parser::semantic_type::Attribute::ATTR_COLOR ; $$.value.color = mn::gl::color($3, $5, $7); }
	;

%%
void yy::Parser::error(const location_type &location,
                       const std::string &m) {
	fprintf(stderr, "%s:%d:%d: %s\n",
	        location.begin.filename->c_str(),
	        location.begin.line, location.begin.column,
	        m.c_str());
}

int yylex(yy::Parser::semantic_type *yylval,
          yy::Parser::location_type *yylloc,
          mn::solar::Lexer &lexer) {
	return lexer.nextToken(*yylval, *yylloc);
}
