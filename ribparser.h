#ifndef CRT_RIBPARSER_H
#define CRT_RIBPARSER_H

#include <string>
#include <boost/spirit.hpp>

#include <iostream>

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
			comment = (boost::spirit::ch_p('#') >> *(boost::spirit::alnum_p | boost::spirit::blank_p))[&CommentReadAction];
			multilineComment = *(comment >> boost::spirit::eol_p);
			comments = (multilineComment | comment);

			// Grammar root.
			base_expression = comments >> boost::spirit::end_p;
		}

		const boost::spirit::rule<ScannerT>& start() const	{ return base_expression; }

		boost::spirit::rule<ScannerT> comment, multilineComment, comments;
		boost::spirit::rule<ScannerT> base_expression;
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
		boost::spirit::parse_info<> info;
		info = boost::spirit::parse(input.c_str(), syntax);

		return info.full;
	}

private:
	RibSyntax syntax;
};

#endif
