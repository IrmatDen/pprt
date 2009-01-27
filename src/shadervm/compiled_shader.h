#ifndef CRT_COMPILED_SHADER_H
#define CRT_COMPILED_SHADER_H

#include <queue>
#include <string>
#include <stack>

#include "symtab.h"

class CompiledShader
{
public:
	CompiledShader();

	void fromMnemonics(const std::string &mnemonics);

private:
	// Standard vars & method feeding.
	void feedStandardVars();

	// Parsing helpers
	void parseVarDecl(const std::string &varDecl);
	void parseInstr(const std::string &instr);
	bool findVarIdx(const std::string &str, int &varIdx);
	
	// Functions
		// Type constructors
		void color4Ctor();

private:
	enum OpCode
	{
		Push,	//! Push a value
		Pushv,	//! Push a var id
		Call,	//! Call a function
		Pop		//! Pop into a var id
	};

private:
	typedef std::pair<OpCode, boost::any>	ByteCode;
	typedef std::queue<ByteCode>			Instructions;
	typedef std::vector<Variable>			VariableTable;
	typedef std::stack<boost::any>			ProgramStack;

private:
	std::string			shaderName;
	VariableTable		varTable;
	Instructions		code;
	
	ProgramStack		execStack;

private:
	typedef std::map<std::string, OpCode>	OpCodeMapping;

private:
	friend void initOpCodeMappings();

	static OpCodeMapping	opCodeMappings;
	static bool				opCodeMappingsInitialized;
};

#endif
