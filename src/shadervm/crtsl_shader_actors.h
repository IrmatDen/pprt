#ifndef CRT_CRTSL_SHADER_ACTORS_H
#define CRT_CRTSL_SHADER_ACTORS_H

// IMPORTANT: this file can't be used alone, it's only to be included in crtslparser.h which
// contains the necessary includes and typedefs!

#include <sstream>
#include <boost/lexical_cast.hpp>

struct debug_a
{
	void operator()(const iterator_t &begin, const iterator_t &end) const
	{
		std::cout << "debug: " << std::string(begin, end) << std::endl;
	}

	template<typename ValueT>
	void operator()(const ValueT &v) const
	{
		std::cout << "debug: " << v << std::endl;
	}
};

struct varToken_a
{
	SLParser::Parser &parser;

	varToken_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		parser.storeVariableToken(std::string(first, last));
	}
};

struct varAssignOp_a
{
	SLParser::Parser &parser;

	varAssignOp_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		parser.storeVariableToken(std::string(first, last));

		std::string s("pop " + std::string(first, last));
		parser.storeStatementToken(s);
	}
};

struct varCheckDiscard_a
{
	SLParser::Parser &parser;

	varCheckDiscard_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		if (first == last)
			parser.discardLastStatement();
	}
};

struct endVariable_a
{
	SLParser::Parser &parser;

	endVariable_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t&, const iterator_t&) const
	{
		parser.endVariable();
	}

	template<typename ValueT>
	void operator()(const ValueT &) const
	{
		parser.endVariable();
	}
};

struct endInstruction_a
{
	SLParser::Parser &parser;

	endInstruction_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t&, const iterator_t&) const
	{
		parser.endInstruction();
	}

	template<typename ValueT>
	void operator()(const ValueT &) const
	{
		parser.endInstruction();
	}
};

struct assignOp_a
{
	SLParser::Parser &parser;

	assignOp_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::string s("pop " + std::string(first, last));
		parser.storeStatementToken(s);
	}
};

struct procCall_a
{
	SLParser::Parser &parser;

	procCall_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::string s(first, last);
		parser.storeStatementToken(s);
	}
};

struct push_a
{
	SLParser::Parser &parser;

	push_a(SLParser::Parser &p) : parser(p) {}

	void operator()(const iterator_t &first, const iterator_t &last) const
	{
		std::string s("push " + std::string(first, last));
		parser.storeStatementToken(s);
	}

	template<typename ValueT>
	void operator()(const ValueT &v) const
	{
		std::ostringstream oss;
		oss << "push " << v;
		parser.storeStatementToken(oss.str());
	}
};


#endif
