#ifndef CRT_RIBPARSER_H
#define CRT_RIBPARSER_H

#include <string>
#include <iostream>
#include <vector>

#include <boost/spirit.hpp>

using namespace boost::spirit;

namespace RibParser
{
	typedef char						char_t;
	typedef file_iterator<char_t>		iterator_t;
	typedef scanner<iterator_t>			scanner_t;
	typedef rule<scanner_t>				rule_t;

	enum RiToken
	{
		DisplayName,
		DisplayType,
		DisplayMode
	};

	void CommentReadAction(const iterator_t &first, const iterator_t &last)
	{
		std::string str(first, last);
		std::cout << "Comment read: " << str << std::endl;
	}

	struct display_a
	{
		void operator()(const iterator_t &/*first*/, const iterator_t &/*last*/) const
		{
			std::cout << "Display token found, params:" << std::endl;
			for (std::vector<std::string>::iterator it = params.begin() ; it != params.end(); ++it)
				std::cout << *it << std::endl;
		}

		static std::vector<std::string> params;
	};
	std::vector<std::string> display_a::params;

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
				// Generic rules
				ending = *blank_p >> !comment >> eol_p; 
				string = ch_p('"') >> (+((alnum_p | punct_p) - '"')) >> ch_p('"');

				// Comment definition
				comment = (ch_p('#') >> *(anychar_p - eol_p))[&CommentReadAction];

				// Display definition
				displayType = (ch_p('"') >> (str_p("framebuffer") | str_p("file")) >> ch_p('"'));
				displayMode = (ch_p('"') >> (str_p("rgb") || ch_p('a') || ch_p('z')) >> ch_p('"'));
				display = ( str_p("Display")								>> blank_p >>
							string		[push_back_a(display_a::params)]	>> blank_p >>	// name
							displayType	[push_back_a(display_a::params)]	>> blank_p >>	// type
							displayMode	[push_back_a(display_a::params)]	>>				// mode
							*(blank_p >> string[push_back_a(display_a::params)])			// options
						  )[display_a()];

				// Grammar line & root.
				element = display;
				line = *blank_p >> !element >> ending >> *blank_p;
				base_expression = *line;
			}

			const rule<ScannerT>& start() const	{ return base_expression; }

			// Generic types
			rule<ScannerT> ending, string;

			// Specific elements
			rule<ScannerT> comment;
			rule<ScannerT> display, displayType, displayMode;

			// General description
			rule<ScannerT> element, line, base_expression;
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
