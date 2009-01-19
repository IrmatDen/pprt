#ifndef CRT_RIBPARSER_H
#define CRT_RIBPARSER_H

#include <string>
#include <boost/spirit.hpp>

#include <iostream>

using namespace boost::spirit;

void CommentReadAction(char const *first, char const *last)
{
	std::string str(first, last);
	std::cout << "Comment read:" << std::endl << str << std::endl << "---end" << std::endl;
}

struct RibSyntax : public boost::spirit::grammar<RibSyntax>
{
public:
	RibSyntax()				{}
	virtual ~RibSyntax()	{}

	template <typename ScannerT>
	struct definition
	{
	public:
		definition(RibSyntax const &self)
		{
			// Comments may be any chars, numbers, space or tabs until end of line, and on 0..n lines.
			comment = (ch_p('#') >> *print_p)[&CommentReadAction];

			// Grammar root.
			base_expression = *(comment >> *eol_p);
		}

		const rule<ScannerT>& start() const	{ return base_expression; }

		rule<ScannerT> comment;
		rule<ScannerT> base_expression;
	};
};

class RibParser
{
public:
	RibParser()
	{
	}

	bool parseString(const std::string &input)
	{
		parse_info<> info;
		info = parse(input.c_str(), syntax);

		return info.full;
	}

private:
	RibSyntax syntax;
};

#endif
