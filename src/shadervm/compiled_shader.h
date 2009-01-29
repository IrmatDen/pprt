#ifndef CRT_COMPILED_SHADER_H
#define CRT_COMPILED_SHADER_H

#include <queue>
#include <string>
#include <stack>

#include "symtab.h"

#include "../scene/scene.h"

class CompiledShader
{
	friend void initOpCodeMappings();

public:
	CompiledShader();

	void fromMnemonics(const std::string &mnemonics);

	void exec(Color4 &out);

private:
	enum OpCode
	{
		Pushd,	//! Push a value
		Pushv,	//! Push a var id
		Call,	//! Call a function
		Pop		//! Pop into a var id
	};

	enum TypeFlag
	{
		TF_Double,
		TF_Color
	};

private:
	typedef std::vector<Variable>					VariableTable;

	typedef std::pair<OpCode, boost::any>			ByteCode;
	typedef std::vector<ByteCode>					Instructions;

	typedef std::pair<TypeFlag, boost::any>			ProgramStackElement;
	typedef std::stack<ProgramStackElement>			ProgramStack;
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
	std::string				shaderName;
	VariableTable			varTable;

	Instructions			code;
	Instructions::iterator	eip;
	
	ProgramStack			execStack;

private:
	typedef std::map<std::string, OpCode>			OpCodeMapping;
	typedef std::map<std::string, ShaderFunction>	FunctionMapping;

private:
	static OpCodeMapping	opCodeMappings;
	static FunctionMapping	fnMappings;
	static bool				opCodeMappingsInitialized;
};

#endif
