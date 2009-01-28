#ifndef CRT_COMPILED_SHADER_H
#define CRT_COMPILED_SHADER_H

#include <queue>
#include <string>
#include <stack>

#include "symtab.h"

class CompiledShader
{
	friend void initOpCodeMappings();

public:
	CompiledShader();

	void fromMnemonics(const std::string &mnemonics);

private:
	enum OpCode
	{
		Push,	//! Push a value
		Pushv,	//! Push a var id
		Call,	//! Call a function
		Pop		//! Pop into a var id
	};

private:
	typedef std::pair<OpCode, boost::any>			ByteCode;
	typedef std::queue<ByteCode>					Instructions;
	typedef std::vector<Variable>					VariableTable;

	typedef std::stack<boost::any>					ProgramStack;
	//typedef std::mem_fun_t<void, CompiledShader>	ShaderFunction;
	typedef void (CompiledShader::*ShaderFunction)();

private:
	// Standard vars & method feeding.
	void feedStandardVars();

	// Parsing helpers
	void parseVarDecl(const std::string &varDecl);
	void parseInstr(const std::string &instr);
	bool findVarIdx(const std::string &str, int &varIdx);
	bool findFunRef(const std::string &str, ShaderFunction &fnRef);
	
	// Functions
		// Type constructors
		void color4Ctor();

private:
	std::string			shaderName;
	VariableTable		varTable;
	Instructions		code;
	
	ProgramStack		execStack;

private:
	typedef std::map<std::string, OpCode>			OpCodeMapping;
	typedef std::map<std::string, ShaderFunction>	FunctionMapping;

private:
	static OpCodeMapping	opCodeMappings;
	static FunctionMapping	fnMappings;
	static bool				opCodeMappingsInitialized;
};

#endif