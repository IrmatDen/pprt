#ifndef CRT_COMPILED_SHADER_H
#define CRT_COMPILED_SHADER_H

#include <queue>
#include <string>
#include <stack>

#include "symtab.h"

#include "../scene/color4.h"

class CompiledShader
{
	friend void initOpCodeMappings();

public:
	enum ShaderType
	{
		ST_Invalid,
		ST_Surface
	};

	enum STDIndices
	{
		Out = 0
	};

public:
	CompiledShader(ShaderType type = ST_Invalid);
	CompiledShader(const CompiledShader &other);

	CompiledShader& operator=(const CompiledShader &other);

	void				setName(const std::string &n)		{ shaderName = n; }
	const std::string&	name() const						{ return shaderName; }

	void addVar(const Variable &v)							{ varTable.push_back(v); }
	void addVar(VariableStorageType varST, VariableType varT, const std::string &name, boost::any value);
	void setVarValue(const std::string &name, boost::any value);
	void setVarValueByIndex(size_t index, boost::any value);

	void parseInstr(const std::string &instr);

	void exec(Color4 &out);

private:
	enum OpCode
	{
		Pushd,	//! Push a value
		Pushv,	//! Push a var id
		Call,	//! Call a function
		Pop,	//! Pop into a var id
		Ret		//! End execution (only used in functions)
	};

private:
	typedef std::vector<Variable>					VariableTable;

	typedef std::pair<OpCode, boost::any>			ByteCode;
	typedef std::vector<ByteCode>					Instructions;

	typedef std::pair<VariableType, boost::any>		ProgramStackElement;
	typedef std::stack<ProgramStackElement>			ProgramStack;
	typedef void (CompiledShader::*ShaderFunction)();

private:

	// Parsing helpers
	//void parseVarDecl(const std::string &varDecl);
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