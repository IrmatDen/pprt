#ifndef CRT_COMPILED_SHADER_H
#define CRT_COMPILED_SHADER_H

#include <map>
#include <queue>
#include <string>
#include <stack>

#include "symtab.h"

#include "../scene/color.h"

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
		// input vars
		Cs	= 0,
		P	= 1,
		N	= 2,
		Ng	= 3,
		I	= 4,

		// output vars
		Ci	= 5
	};

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

	typedef void (CompiledShader::*ShaderFunction)();

	typedef boost::variant<Real, Color, Vec3, int, ShaderFunction>	OpCodeArg;
	typedef std::pair<OpCode, OpCodeArg>							ByteCode;
	typedef std::vector<ByteCode>									Instructions;

public:
	CompiledShader(ShaderType shaderType = ST_Invalid);
	CompiledShader(const CompiledShader &other, bool runtime = false);

	CompiledShader& operator=(const CompiledShader &other);

	CompiledShader		cloneWithCodePtr(ByteCode *bcode, size_t codeLen) const;

	void				setScene(Scene *scn)				{ scene = scn; }
	void				setCurrentDepth(int depth)			{ currentDepth = depth; }

	void				setName(const std::string &n)		{ shaderName = n; }
	const std::string&	name() const						{ return shaderName; }

	void addVar(const Variable &v)							{ varTable.push_back(v); }
	void addVar(VariableStorageType varST, VariableType varT, const std::string &name, VarValue value);

	void setVarValue(const std::string &name, VarValue value);
	void setVarValueByIndex(size_t index, VarValue value);

	void parseInstr(const std::string &instr);
	Instructions getCode() const { return code; }

	void exec(Color &out);

private:
	typedef std::vector<Variable>					VariableTable;

	typedef std::pair<VariableType, VarValue>		ProgramStackElement;
	typedef std::vector<ProgramStackElement>		ProgramStack;

private:
	// Parsing helpers
	bool findVarIdx(const std::string &str, int &varIdx);
	bool findFunRef(const std::string &str, ShaderFunction &fnRef);
	
	// Functions
		// Type constructors
		void colorCtor();

		// Geometric functions
		void	faceForward();
		void	reflect();

		// Shading and lighting functions
		void	diffuse();
		void	specular();
		void	trace();

private:
	ShaderType					type;
	std::string					shaderName;
	VariableTable				varTable;

	Instructions				code;

	ByteCode				*	codePtr;
	ByteCode				*	codePtrEnd;
	size_t						codeSize;
	ByteCode				*	eip;
	
	ProgramStackElement			execStack[15];
	ProgramStackElement		*	esp;

	Scene					*	scene;
	int							currentDepth;

private:
	typedef std::map<std::string, OpCode>			OpCodeMapping;
	typedef std::map<std::string, ShaderFunction>	FunctionMapping;

private:
	static OpCodeMapping	opCodeMappings;
	static FunctionMapping	fnMappings;
	static bool				opCodeMappingsInitialized;
};

#endif
