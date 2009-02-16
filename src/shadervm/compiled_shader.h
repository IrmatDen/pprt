#ifndef CRT_COMPILED_SHADER_H
#define CRT_COMPILED_SHADER_H

#include <map>
#include <queue>
#include <string>
#include <stack>

#include "symtab.h"

#include "../scene/color4.h"

class Scene;

class CompiledShader
{
	friend void initOpCodeMappings();

public:
	enum ShaderType
	{
		ST_Invalid,
		ST_Surface
	};

	enum STDVarIndices
	{
		Out = 0,
		P	= 1,
		N	= 2,
		I	= 3
	};

public:
	CompiledShader(ShaderType type = ST_Invalid);
	CompiledShader(const CompiledShader &other);

	CompiledShader& operator=(const CompiledShader &other);

	void				setScene(Scene *scn)				{ scene = scn; }

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
		Mult,	//! Multiply the 2 first values in stack
		Add,	//! Add the 2 first values in stack
		Call,	//! Call a function
		Pop,	//! Pop into a var id
		Ret		//! End execution (only used in functions)
	};

private:
	typedef std::vector<Variable>					VariableTable;

	typedef std::pair<OpCode, boost::any>			ByteCode;
	typedef std::vector<ByteCode>					Instructions;

	typedef std::pair<VariableType, boost::any>		ProgramStackElement;
	typedef std::vector<ProgramStackElement>		ProgramStack;

	typedef void (CompiledShader::*ShaderFunction)();

private:
	// Parsing helpers
	bool findVarIdx(const std::string &str, int &varIdx);
	bool findFunRef(const std::string &str, ShaderFunction &fnRef);
	
	// Functions
		// Type constructors
		void color4Ctor();

		// Shading and lighting functions
		void	diffuse();
		void	specular();

private:
	std::string					shaderName;
	VariableTable				varTable;

	Instructions				code;
	Instructions::iterator		eip;
	
	//ProgramStack				execStack;
	ProgramStackElement			execStack[15];
	ProgramStackElement		*	esp;

	Scene					*	scene;

private:
	typedef std::map<std::string, OpCode>			OpCodeMapping;
	typedef std::map<std::string, ShaderFunction>	FunctionMapping;

private:
	static OpCodeMapping	opCodeMappings;
	static FunctionMapping	fnMappings;
	static bool				opCodeMappingsInitialized;
};

#endif
