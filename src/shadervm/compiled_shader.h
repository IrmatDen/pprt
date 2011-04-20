#ifndef PPRT_COMPILED_SHADER_H
#define PPRT_COMPILED_SHADER_H
#include "symtab.h"
#include "vmstack.h"

#include "../scene/color.h"
#include "../common.h"
#include "../aligned_vector.h"

#include <map>
#include <queue>
#include <string>
#include <stack>

#include <tbb/enumerable_thread_specific.h>

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
		Pushd,		//! Push a value
		PushVec,	//! Push the var id of a vector3
		PushCol,	//! Push the var id of a color
		PushReal,	//! Push the var id of a real value
		Mult,		//! Generic Multiply the 2 first values in stack (only used in high lvl representation)
		MultRealReal,
		MultRealVec,
		MultRealCol,
		MultVecReal,
		MultVecVec,
		MultVecCol,
		MultColReal,
		MultColVec,
		MultColCol,
		Add,		//! Add the 2 first values in stack, HL opcode
		AddRealReal,
		AddRealVec,
		AddRealCol,
		AddVecReal,
		AddVecVec,
		AddVecCol,
		AddColReal,
		AddColVec,
		AddColCol,
		Sub,		//! Substract the 2 first values in stack, HL representation only again
		SubRealReal,
		SubRealVec,
		SubRealCol,
		SubVecReal,
		SubVecVec,
		SubVecCol,
		SubColReal,
		SubColVec,
		SubColCol,
		Dot,		//! Dot product between the 2 top values of the stack (must be vector)
		Negate,		//! Negate the passed argument, HL representation
		NegateReal,
		NegateVec,
		Call,		//! Call a function
		Pop,		//! Generic pop, only used in high lvl representation
		PopVecVec,	//! Pop a vector in a vector
		PopVecReal,	//! Pop a scalar in a vector
		PopVecCol,	//! Pop a color in a vector
		PopColCol,	//! Pop a color in a color
		PopColReal,	//! Pop a scalar in a color
		PopColVec,	//! Pop a vector in a color
		PopRealReal,//! Pop into a var id
		Ret			//! End execution (only used in functions)
	};

	typedef void (CompiledShader::*ShaderFunction)();

	typedef boost::variant<float, Color, Vector3, int, ShaderFunction>	OpCodeArg;
	typedef std::pair<OpCode, OpCodeArg>							ByteCode;
	typedef std::vector<ByteCode, memory::AllocAlign16<ByteCode> >	Instructions;

public:
	CompiledShader(ShaderType shaderType = ST_Invalid);
	CompiledShader(const CompiledShader &other, bool runtime = false);
	~CompiledShader();

	CompiledShader& operator=(const CompiledShader &other);

	// Move ctor/assignment
	CompiledShader(CompiledShader &&other);
	CompiledShader& operator=(CompiledShader &&other);

	CompiledShader		cloneWithCodePtr(ByteCode *bcode, size_t codeLen) const;

	void				setScene(Scene *scn)				{ scene = scn; }
	void				setCurrentDepth(int depth)			{ currentDepth = depth; }

	void				setName(const std::string &n)		{ shaderName = n; }
	const std::string&	name() const						{ return shaderName; }

	ShaderType			shaderType() const					{ return type; }

	void addVar(const Variable &v);
	void addVar(VariableStorageType varST, VariableType varT, const std::string &name, const VarValue &value);

	void setVarValue(const std::string &name, const VarValue &value);
	void setVarValueByIndex(size_t index, const VarValue &value);

	void parseInstr(const std::string &instr);
	Instructions getCode() const { return code; }

	void finalize();

	void setRTVarValueByIndex(size_t index, const VarValue &value);
	
	void getOutput(Color &color, Color &opacity);

	void exec();

private:
	// Compilation related datatypes
	typedef AlignedVector<Variable>		VariableTable;
	typedef std::stack<VariableType>	TypeStack;

	struct FunctionInfo
	{
		FunctionInfo() : func(nullptr), args(0), retValue(VariableType(0))		{}
		FunctionInfo(ShaderFunction f, int a, VariableType vt)
			: func(f), args(a), retValue(vt)
		{
		}

		ShaderFunction	func;
		int				args;
		VariableType	retValue;
	};

private:
	// Initialization helper
	void initRTVars(const VarValue * const copyFrom);

	// Parsing helpers
	bool findVarIdx(const std::string &str, int &varIdx);
	bool findFunRef(const std::string &str, FunctionInfo **fnRef);
	
	// Functions
		// Type constructors
		void colorCtor();
		void vec3Ctor();

		// Mathematical functions
		void smoothstep();

		// Geometric functions
		void faceForward();
		void normalize();
		void reflect();

		// Color functions
		void mix();

		// Shading and lighting functions
		void ambient();
		void diffuse();
		void specular();
		void trace();

private:
	ShaderType			type;
	std::string			shaderName;
	VariableTable		varTable;
	size_t				rtVarTableSize;
	VarValue		*	rtVarTable;

	Instructions		code;

	ByteCode		*	codePtr;
	ByteCode		*	codePtrEnd;
	size_t				codeSize;
	ByteCode		*	eip;

	TypeStack			typeStack;	//<! Used at compilation only. Will be moved to a shader compiler module soon
	VMStack				execStack;

	Scene			*	scene;
	int					currentDepth;

private:
	typedef std::map<std::string, OpCode>			OpCodeMapping;
	typedef std::map<std::string, FunctionInfo>		FunctionMapping;
	
private:
	static OpCodeMapping		opCodeMappings;
	static FunctionMapping		fnMappings;
	static bool					opCodeMappingsInitialized;
	static memory::TLPool		shaderRTVarPool;
};

#endif
