#ifndef CRT_COMPILED_SHADER_H
#define CRT_COMPILED_SHADER_H

#include <map>
#include <queue>
#include <string>
#include <stack>

#include "symtab.h"

#include "../scene/color.h"

class Scene;

class _MM_ALIGN16 CompiledShader
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
		Cs,
		Os,
		P,
		N,
		Ng,
		s,
		t,
		I,

		// output vars
		Ci,
		Oi
	};

	enum OpCode
	{
		Pushd,	//! Push a value
		Pushv,	//! Push a var id
		Mult,	//! Multiply the 2 first values in stack
		Add,	//! Add the 2 first values in stack
		Sub,	//! Substract the 2 first values in stack
		Dot,	//! Dot product between the 2 top values of the stack (must be vector)
		Negate,	//! Negate the passed argument
		Call,	//! Call a function
		Pop,	//! Pop into a var id
		Ret		//! End execution (only used in functions)
	};

	typedef void (CompiledShader::*ShaderFunction)();

	typedef boost::variant<float, Color, Vec3, int, ShaderFunction>	OpCodeArg;
	typedef std::pair<OpCode, OpCodeArg>							ByteCode;
	typedef std::vector<ByteCode, memory::AllocAlign16<ByteCode> >	Instructions;

public:
	CompiledShader(ShaderType shaderType = ST_Invalid);
	CompiledShader(const CompiledShader &other, bool runtime = false);

	CompiledShader& operator=(const CompiledShader &other);

	CompiledShader		cloneWithCodePtr(ByteCode *bcode, size_t codeLen) const;

	void				setScene(Scene *scn)				{ scene = scn; }
	void				setCurrentDepth(int depth)			{ currentDepth = depth; }

	void				setName(const std::string &n)		{ shaderName = n; }
	const std::string&	name() const						{ return shaderName; }

	ShaderType			shaderType() const					{ return type; }

	void addVar(const Variable &v)							{ varTable.push_back(v); }
	void addVar(VariableStorageType varST, VariableType varT, const std::string &name, const VarValue &value);

	void setVarValue(const std::string &name, const VarValue &value);
	void setVarValueByIndex(size_t index, const VarValue &value);
	
	void getOutput(Color &color, Color &opacity);

	void parseInstr(const std::string &instr);
	Instructions getCode() const { return code; }

	void exec();

private:
	typedef std::vector<Variable, memory::AllocAlign16<ByteCode> >	VariableTable;
	typedef std::pair<VariableType, VarValue>						ProgramStackElement;

private:
	// Parsing helpers
	bool findVarIdx(const std::string &str, int &varIdx);
	bool findFunRef(const std::string &str, ShaderFunction &fnRef);
	
	// Functions
		// Type constructors
		void colorCtor();
		void vec3Ctor();

		// Mathematical functions
		void	smoothstep();

		// Geometric functions
		void	faceForward();
		void	normalize();
		void	reflect();

		// Color functions
		void	mix();

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
