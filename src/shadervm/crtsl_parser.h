#ifndef CRT_CRTCL_PARSER_H
#define CRT_CRTCL_PARSER_H

#include <string>
#include <stack>
#include <queue>

class SymbolTable;

namespace SLParser
{
	struct ShaderGrammar;

	class Parser
	{
	public:
		Parser();
		~Parser();

		bool parseFile(const std::string &filename);

		void storeVariableToken(const std::string &t)
		{
			varsTokens.push(t);
		}

		void discardLastStatement()
		{
			stmtsTokens.pop();
		}

		void storeStatementToken(const std::string &t)
		{
			stmtsTokens.push(t);
		}

		void endVariable();
		void endInstruction();

	private:
		typedef std::queue<std::string> VariablesStack;
		typedef std::stack<std::string> StatementsStack;
		typedef std::queue<std::string> Instructions;

	private:
		ShaderGrammar		*	syntax;

		SymbolTable			*	symTable;

		VariablesStack			varsTokens;
		StatementsStack			stmtsTokens;
		Instructions			instructions;
	};
}

#endif
