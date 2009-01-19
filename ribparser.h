#ifndef CRT_RIBPARSER_H
#define CRT_RIBPARSER_H

#include <string>
#include <boost/spirit.hpp>

#include <iostream>

using namespace boost::spirit;

namespace RibParser
{
	typedef char						char_t;
	typedef file_iterator<char_t>		iterator_t;
	typedef scanner<iterator_t>			scanner_t;
	typedef rule<scanner_t>				rule_t;

	void CommentReadAction(const iterator_t &first, const iterator_t &last)
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

	class Parser
	{
	public:
		Parser()
		{
		}

		bool parseFile(const std::string &filename)
		{
			iterator_t ribFileBegin(filename);
			if (!ribFileBegin)
			{
				std::cerr << "Can't open " << filename << std::endl;
				return false;
			}
			iterator_t ribFileEnd = ribFileBegin.make_end();

			parse_info<iterator_t> info = parse(ribFileBegin, ribFileEnd, syntax);

			return info.full;
		}

	private:
		RibSyntax syntax;
	};
}

#endif
