#ifndef CRT_CRTCL_PARSER_H
#define CRT_CRTCL_PARSER_H

#include <string>
#include <stack>
#include <queue>

namespace SLParser
{
	struct ShaderGrammar;

	class Parser
	{
	public:
		Parser();

		bool parseFile(const std::string &filename);

		void storeStatementToken(const std::string &t)
		{
			stmtsTokenStack.push(t);
		}

		void endInstruction()
		{
			while (stmtsTokenStack.size() > 0)
			{
				instructions.push(stmtsTokenStack.top());
				stmtsTokenStack.pop();
			}
		}

	private:
		typedef std::stack<std::string> FunctionsStack;
		typedef std::stack<std::string> VariablesStack;
		typedef std::stack<std::string> StatementsStack;
		typedef std::queue<std::string> Instructions;

	private:
		ShaderGrammar		*	syntax;

		FunctionsStack			funsTokenStack;
		VariablesStack			varsTokenStack;
		StatementsStack			stmtsTokenStack;
		Instructions			instructions;
	};
}

#endif
