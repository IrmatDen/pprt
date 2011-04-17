#include <cassert>
#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace std;
using namespace boost;

struct RTVarPoolCreator
{
	static memory::AlignedPool* create()
	{
		const size_t base_size = sizeof(VarValue) * 128;
		return new memory::AlignedPool(base_size);
	}
};


CompiledShader::OpCodeMapping		CompiledShader::opCodeMappings;
CompiledShader::FunctionMapping		CompiledShader::fnMappings;
bool								CompiledShader::opCodeMappingsInitialized = false;
CompiledShader::ShaderRTVarPoolImpl	CompiledShader::shaderRTVarPool(&RTVarPoolCreator::create);

#define MK_SHADERFUNC(METHODNAME)	CompiledShader::ShaderFunction(&CompiledShader::METHODNAME)

void initOpCodeMappings()
{
	// mnemonic - Opcodes mapping
	CompiledShader::opCodeMappings["push"]	= CompiledShader::Pushd;
	CompiledShader::opCodeMappings["mult"]	= CompiledShader::Mult;
	CompiledShader::opCodeMappings["add"]	= CompiledShader::Add;
	CompiledShader::opCodeMappings["sub"]	= CompiledShader::Sub;
	CompiledShader::opCodeMappings["dot"]	= CompiledShader::Dot;
	CompiledShader::opCodeMappings["negate"]= CompiledShader::Negate;
	CompiledShader::opCodeMappings["call"]	= CompiledShader::Call;
	CompiledShader::opCodeMappings["pop"]	= CompiledShader::Pop;
	CompiledShader::opCodeMappings["ret"]	= CompiledShader::Ret;

	// Function name - fn pointers mapping
		// Type ctors
	CompiledShader::fnMappings["color"]		= CompiledShader::FunctionInfo(MK_SHADERFUNC(colorCtor), 3, VT_Color);
	CompiledShader::fnMappings["vec3"]		= CompiledShader::FunctionInfo(MK_SHADERFUNC(vec3Ctor), 3, VT_Vector);
	
		// Mathematical
	CompiledShader::fnMappings["smoothstep"]	= CompiledShader::FunctionInfo(MK_SHADERFUNC(smoothstep), 3, VT_Float);

		// Geometric
	CompiledShader::fnMappings["faceforward"]	= CompiledShader::FunctionInfo(MK_SHADERFUNC(faceForward), 2, VT_Vector);
	CompiledShader::fnMappings["normalize"]		= CompiledShader::FunctionInfo(MK_SHADERFUNC(normalize), 1, VT_Vector);
	CompiledShader::fnMappings["reflect"]		= CompiledShader::FunctionInfo(MK_SHADERFUNC(reflect), 2, VT_Vector);
	
		// Color
	CompiledShader::fnMappings["mix"]	= CompiledShader::FunctionInfo(MK_SHADERFUNC(mix), 3, VT_Color);
		
		// Shading & lighting
	CompiledShader::fnMappings["ambient"]	= CompiledShader::FunctionInfo(MK_SHADERFUNC(ambient), 0, VT_Color);
	CompiledShader::fnMappings["diffuse"]	= CompiledShader::FunctionInfo(MK_SHADERFUNC(diffuse), 1, VT_Color);
	CompiledShader::fnMappings["specular"]	= CompiledShader::FunctionInfo(MK_SHADERFUNC(specular), 2, VT_Color);
	CompiledShader::fnMappings["trace"]		= CompiledShader::FunctionInfo(MK_SHADERFUNC(trace), 2, VT_Color);
}

CompiledShader::CompiledShader(ShaderType shaderType)
:type(shaderType), scene(nullptr), rtVarTable(nullptr), rtVarTableSize(0)
{
	if (!opCodeMappingsInitialized)
	{
		initOpCodeMappings();
		opCodeMappingsInitialized = true;
	}
	
	// input
	addVar(VST_Varying, VT_Color,	"Cs",	Color(0.f));
	addVar(VST_Varying, VT_Color,	"Os",	Color(0.f));
	addVar(VST_Varying, VT_Vector,	"P",	Vector3(0.f));
	addVar(VST_Varying, VT_Vector,	"N",	Vector3(0.f));
	addVar(VST_Varying, VT_Vector,	"Ng",	Vector3(0.f));
	addVar(VST_Varying, VT_Float,	"s",	0.f);
	addVar(VST_Varying, VT_Float,	"t",	0.f);
	addVar(VST_Varying, VT_Vector,	"I",	Vector3(0.f));

	// output
	addVar(VST_Varying, VT_Color,	"Ci",	Color(0.f));
	addVar(VST_Varying, VT_Color,	"Oi",	Color(0.f));

	switch(type)
	{
	case ST_Surface:
		break;
	}
}

CompiledShader::CompiledShader(const CompiledShader &other, bool runtime)
	:	codePtr(other.codePtr), codeSize(other.codeSize), codePtrEnd(other.codePtrEnd),
		scene(other.scene), rtVarTable(nullptr), rtVarTableSize(0)
{
	if (!runtime)
	{
		type		= other.type;
		shaderName	= other.shaderName;
		code		= other.code;
		varTable	= other.varTable;
	}
	else
	{
		assert(other.rtVarTable != nullptr && other.rtVarTableSize > 0);
		rtVarTableSize = other.rtVarTableSize;
		initRTVars(other.rtVarTable);
	}
}

CompiledShader::~CompiledShader()
{
	if (rtVarTable != nullptr)
		shaderRTVarPool.local()->ordered_free(rtVarTable, rtVarTableSize);
}

CompiledShader& CompiledShader::operator=(const CompiledShader &other)
{
	type		= other.type;
	shaderName	= other.shaderName;
	varTable	= other.varTable;
	code		= other.code;
	scene		= other.scene;

	if (rtVarTable != nullptr)
		shaderRTVarPool.local()->ordered_free(rtVarTable, rtVarTableSize);

	rtVarTableSize = 0;
	rtVarTable = nullptr;

	if (other.rtVarTable != nullptr)
	{
		rtVarTableSize	= other.rtVarTableSize;
		initRTVars(other.rtVarTable);
	}

	return *this;
}

void CompiledShader::initRTVars(const VarValue * const copyFrom)
{
	rtVarTable = static_cast<VarValue*>(shaderRTVarPool.local()->ordered_malloc(rtVarTableSize));
	memcpy(rtVarTable, copyFrom, sizeof(VarValue) * rtVarTableSize);
}

CompiledShader::CompiledShader(CompiledShader &&other)
	:	shaderName(other.shaderName), codePtr(other.codePtr), codeSize(other.codeSize), codePtrEnd(other.codePtrEnd),
		scene(other.scene), execStack(other.execStack), varTable(other.varTable),
		rtVarTableSize(other.rtVarTableSize), rtVarTable(other.rtVarTable)
{
	other.rtVarTable = nullptr;
}

CompiledShader& CompiledShader::operator=(CompiledShader &&other)
{
	type		= other.type;
	shaderName	= other.shaderName;
	varTable	= other.varTable;
	code		= other.code;
	scene		= other.scene;
	execStack	= other.execStack;

	if (rtVarTable != nullptr)
		shaderRTVarPool.local()->ordered_free(rtVarTable, rtVarTableSize);

	rtVarTableSize	= other.rtVarTableSize;
	rtVarTable		= other.rtVarTable;
	other.rtVarTable = nullptr;

	return *this;
}

CompiledShader CompiledShader::cloneWithCodePtr(ByteCode *bcode, size_t codeLen) const
{
	CompiledShader ret(*this);
	ret.codePtr		= bcode;
	ret.codeSize	= codeLen;
	ret.codePtrEnd	= ret.codePtr + ret.codeSize;

	return ret;
}

void CompiledShader::addVar(const Variable &v)
{
	varTable.push_back(v);
}

void CompiledShader::addVar(VariableStorageType varST, VariableType varT, const std::string &name, const VarValue &value)
{
	Variable v(varST, varT, name, value);
	addVar(v);
}

void CompiledShader::setVarValue(const std::string &name, const VarValue &value)
{
	for(VariableTable::iterator it = varTable.begin(); it != varTable.end(); ++it)
	{
		if ((*it).name == name)
		{
			(*it).content = value;
		}
	}
}

void CompiledShader::setVarValueByIndex(size_t index, const VarValue &value)
{
	varTable.at(index).content = value;
}

void CompiledShader::setRTVarValueByIndex(size_t index, const VarValue &value)
{
	assert(rtVarTable != nullptr);
	assert(index < rtVarTableSize);
	rtVarTable[index] = value;
}

void CompiledShader::getOutput(Color &color, Color &opacity)
{
	color	= boost::get<Color>(rtVarTable[Ci]);
	opacity	= boost::get<Color>(rtVarTable[Oi]);
}

void CompiledShader::parseInstr(const std::string &instr)
{
	typedef vector<string> Tokens;
	Tokens tokens;
	split(tokens, instr, is_any_of(" "));

	//! \todo throw exception if tokens doesn't contain 2 strings

	ByteCode bc;

	// Found instruction opcode
	OpCodeMapping::const_iterator opcode = opCodeMappings.find(tokens[0]);

	//! \todo throw exception if opCodeMappings doesn't contain the current opcode mnemonic.
	if (opcode == opCodeMappings.end())
		return;

	bc.first = opcode->second;

	switch (bc.first)
	{
	// Push <something>
	case Pushd:
#pragma region Pushing
		// Check if it's a number, in which case, it is pushed.
		try
		{
			bc.second = lexical_cast<float>(tokens[1]);
			typeStack.push(VT_Float);
		}
		catch (bad_lexical_cast &)
		{
			// Otherwise, it's a var name, so search for it
			int varIdx;
			if (findVarIdx(tokens[1], varIdx))
			{
				// Change the opcode to match the fact we're pushing a variable index and not a number.
				switch (varTable.at(varIdx).type)
				{
				case VT_Float:
					bc.first = PushReal;
					typeStack.push(VT_Float);
					break;
						
				case VT_Color:
					bc.first = PushCol;
					typeStack.push(VT_Color);
					break;
						
				case VT_Vector:
					bc.first = PushVec;
					typeStack.push(VT_Vector);
					break;
				};

				bc.second = varIdx;
			}
			else
			{
				//! \todo Throw an exception as variable is unknown
			}
		}
#pragma endregion
		break;

		// Mult (2 operands on stack)
	case Mult:
		{
#pragma region Multiplying
			// Second operand of bytecode is useless (ATM that is)
			const VariableType vt1 = typeStack.top();		typeStack.pop();
			const VariableType vt2 = typeStack.top();		typeStack.pop();

			switch (vt1)
			{
			case VT_Float:
				switch (vt2)
				{
				case VT_Float:
					bc.first = MultRealReal;
					typeStack.push(VT_Float);
					break;

				case VT_Vector:
					bc.first = MultRealVec;
					typeStack.push(VT_Vector);
					break;

				case VT_Color:
					bc.first = MultRealCol;
					typeStack.push(VT_Color);
					break;
				}
				break;

			case VT_Vector:
				switch (vt2)
				{
				case VT_Float:
					bc.first = MultVecReal;
					typeStack.push(VT_Vector);
					break;

				case VT_Vector:
					bc.first = MultVecVec;
					typeStack.push(VT_Vector);
					break;

				case VT_Color:
					bc.first = MultVecCol;
					typeStack.push(VT_Color);
					break;
				}
				break;

			case VT_Color:
				switch (vt2)
				{
				case VT_Float:
					bc.first = MultColReal;
					typeStack.push(VT_Color);
					break;

				case VT_Vector:
					bc.first = MultColVec;
					typeStack.push(VT_Color);
					break;

				case VT_Color:
					bc.first = MultColCol;
					typeStack.push(VT_Color);
					break;
				}
				break;
			}
#pragma endregion
		}
		break;

		// Add (2 operands on stack)
	case Add:
		{
#pragma region Addition
			// Second operand of bytecode is useless (ATM that is)
			const VariableType vt1 = typeStack.top();		typeStack.pop();
			const VariableType vt2 = typeStack.top();		typeStack.pop();

			switch (vt1)
			{
			case VT_Float:
				switch (vt2)
				{
				case VT_Float:
					bc.first = AddRealReal;
					typeStack.push(VT_Float);
					break;

				case VT_Vector:
					bc.first = AddRealVec;
					typeStack.push(VT_Vector);
					break;

				case VT_Color:
					bc.first = AddRealCol;
					typeStack.push(VT_Color);
					break;
				}
				break;

			case VT_Vector:
				switch (vt2)
				{
				case VT_Float:
					bc.first = AddVecReal;
					typeStack.push(VT_Vector);
					break;

				case VT_Vector:
					bc.first = AddVecVec;
					typeStack.push(VT_Vector);
					break;

				case VT_Color:
					bc.first = AddVecCol;
					typeStack.push(VT_Color);
					break;
				}
				break;

			case VT_Color:
				switch (vt2)
				{
				case VT_Float:
					bc.first = AddColReal;
					typeStack.push(VT_Color);
					break;

				case VT_Vector:
					bc.first = AddColVec;
					typeStack.push(VT_Color);
					break;

				case VT_Color:
					bc.first = AddColCol;
					typeStack.push(VT_Color);
					break;
				}
				break;
			}
#pragma endregion
		}
		break;

		// Add (2 operands on stack)
	case Sub:
		{
#pragma region Substraction
			// Second operand of bytecode is useless (ATM that is)
			const VariableType vt1 = typeStack.top();		typeStack.pop();
			const VariableType vt2 = typeStack.top();		typeStack.pop();

			switch (vt1)
			{
			case VT_Float:
				switch (vt2)
				{
				case VT_Float:
					bc.first = SubRealReal;
					typeStack.push(VT_Float);
					break;

				case VT_Vector:
					bc.first = SubRealVec;
					typeStack.push(VT_Vector);
					break;

				case VT_Color:
					bc.first = SubRealCol;
					typeStack.push(VT_Color);
					break;
				}
				break;

			case VT_Vector:
				switch (vt2)
				{
				case VT_Float:
					bc.first = SubVecReal;
					typeStack.push(VT_Vector);
					break;

				case VT_Vector:
					bc.first = SubVecVec;
					typeStack.push(VT_Vector);
					break;

				case VT_Color:
					bc.first = SubVecCol;
					typeStack.push(VT_Color);
					break;
				}
				break;

			case VT_Color:
				switch (vt2)
				{
				case VT_Float:
					bc.first = SubColReal;
					typeStack.push(VT_Color);
					break;

				case VT_Vector:
					bc.first = SubColVec;
					typeStack.push(VT_Color);
					break;

				case VT_Color:
					bc.first = SubColCol;
					typeStack.push(VT_Color);
					break;
				}
				break;
			}
#pragma endregion
		}
		break;

		// Pop <something>
	case Pop:
		{
#pragma region Popping methods
			// Popping is only possible in a variable, so no need to search something else.
			int varIdx;
			if (findVarIdx(tokens[1], varIdx))
			{
				bc.second	= varIdx;

				switch (varTable.at(varIdx).type)
				{
				case VT_Float:
					switch (typeStack.top())
					{
					case VT_Float:
						bc.first = PopRealReal;
						break;

					default:
						break; //! \todo throw exception
					};
					typeStack.pop();
					break;
					
				case VT_Color:
					switch (typeStack.top())
					{
					case VT_Color:
						bc.first = PopColCol;
						break;

					case VT_Float:
						bc.first = PopColReal;
						break;

					case VT_Vector:
						bc.first = PopColVec;
						break;
					};
					typeStack.pop();
					break;
					
				case VT_Vector:
					switch (typeStack.top())
					{
					case VT_Vector:
						bc.first = PopVecVec;
						break;

					case VT_Float:
						bc.first = PopVecReal;
						break;

					case VT_Color:
						bc.first = PopVecCol;
						break;
					};
					typeStack.pop();
					break;
				};
			}
			else
			{
				//! \todo Throw an exception as variable is unknown
			}
#pragma endregion
		}
		break;

	case Negate:
		switch (typeStack.top())
		{
		case VT_Float:
			bc.first = NegateReal;
			break;

		case VT_Vector:
			bc.first = NegateVec;
			break;
		}
		break;

	case Dot:
		typeStack.push(VT_Float);
		break;

		// Call <proc>
	case Call:
		{
			FunctionInfo *fi(nullptr);
			if (findFunRef(tokens[1], &fi))
			{
				assert(fi != nullptr);
				assert(fi->func != nullptr);
				bc.second = fi->func;

				// Update type stack accordingly
				for (int i = 0; i != fi->args; i++)
					typeStack.pop();
				typeStack.push(fi->retValue);
			}
			else
			{
				//! \todo Throw an exception as function is unknown
			}
		}
		break;

	default:
		break;
	}

	code.push_back(bc);
}

bool CompiledShader::findVarIdx(const std::string &str, int &varIdx)
{
	for (size_t i = 0; i < varTable.size(); i++)
	{
		if (varTable.at(i).name == str)
		{
			varIdx	= (int)i;
			return true;
		}
	}

	return false;
}

bool CompiledShader::findFunRef(const std::string &str, FunctionInfo **fnRef)
{
	// Found instruction opcode
	FunctionMapping::iterator mappedFnRef = fnMappings.find(str);

	//! \todo throw exception if fnMappings doesn't contain the current function name.
	if (mappedFnRef == fnMappings.end())
	{
		fnRef = nullptr;
		return false;
	}

	*fnRef = &(mappedFnRef->second);

	return true;
}

void CompiledShader::finalize()
{
	// Generate runtime vartable
	rtVarTableSize	= varTable.size();
	rtVarTable		= static_cast<VarValue*>(shaderRTVarPool.local()->ordered_malloc(rtVarTableSize));
	size_t i		= 0;
	for_each(varTable.begin(), varTable.end(),
		[&] (const Variable &v)
		{
			new (&rtVarTable[i]) VarValue(v.content);
			i++;
		} );

	varTable.swap(VariableTable());
}

void CompiledShader::exec()
{
	eip = codePtr;
	execStack.allocate();
	execStack.reset();

	while (eip != codePtrEnd)
	{
		switch(eip->first)
		{
#pragma region Pushing
		case Pushd:
			execStack.push<float>(get<float>(eip->second));
			break;

		case PushVec:
			{
				const VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				execStack.push(get<Vector3>(var));
			}
			break;
			
		case PushCol:
			{
				const VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				execStack.push(get<Color>(var));
			}
			break;

		case PushReal:
			{
				const VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				execStack.push(get<float>(var));
			}
			break;
#pragma endregion

		case Call:
			{
				ShaderFunction f = boost::get<ShaderFunction>(eip->second);
				(this->*f)();
				break;
			}

		case Dot:
			{
				Vector3	&op1v = execStack.pop<Vector3>();
				Vector3	&op2v = execStack.pop<Vector3>();
				execStack.push((float)dot(op1v, op2v));
				break;
			}

		case NegateReal:
			{
				float &op = execStack.pop<float>();
				execStack.push(-op);
			}
			break;

		case NegateVec:
			{
				Vector3 &op = execStack.pop<Vector3>();
				execStack.push(-op);
			}
			break;
			
#pragma region Multiplying
		case MultRealReal:
			{
				float &op1 = execStack.pop<float>();
				float &op2 = execStack.pop<float>();
				execStack.push(op1 * op2);
			}
			break;

		case MultRealVec:
			{
				float	&op1 = execStack.pop<float>();
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(op1 * op2);
			}
			break;

		case MultRealCol:
			{
				float	&op1 = execStack.pop<float>();
				Color	&op2 = execStack.pop<Color>();
				execStack.push(op1 * op2);
			}
			break;

		case MultVecReal:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				float	&op2 = execStack.pop<float>();
				execStack.push(op1 * op2);
			}
			break;

		case MultVecVec:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(mulPerElem(op1, op2));
			}
			break;

		case MultVecCol:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Color	&op2 = execStack.pop<Color>();
				execStack.push(mulPerElem(op1, op2));
			}
			break;

		case MultColReal:
			{
				Color	&op1 = execStack.pop<Color>();
				float	&op2 = execStack.pop<float>();
				execStack.push(op1 * op2);
			}
			break;

		case MultColVec:
			{
				Color	&op1 = execStack.pop<Color>();
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(mulPerElem(op1, op2));
			}
			break;

		case MultColCol:
			{
				Color	&op1 = execStack.pop<Color>();
				Color	&op2 = execStack.pop<Color>();
				execStack.push(mulPerElem(op1, op2));
			}
			break;
#pragma endregion
			
#pragma region Addition
		case AddRealReal:
			{
				float &op1 = execStack.pop<float>();
				float &op2 = execStack.pop<float>();
				execStack.push(op1 + op2);
			}
			break;

		case AddRealVec:
			{
				Vector3	 op1(execStack.pop<float>());
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(op1 + op2);
			}
			break;

		case AddRealCol:
			{
				Color	 op1(execStack.pop<float>());
				Color	&op2 = execStack.pop<Color>();
				execStack.push(op1 + op2);
			}
			break;

		case AddVecReal:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Vector3	 op2(execStack.pop<float>());
				execStack.push(op1 + op2);
			}
			break;

		case AddVecVec:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(op1 + op2);
			}
			break;

		case AddVecCol:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Color	&op2 = execStack.pop<Color>();
				execStack.push(op1 + op2);
			}
			break;

		case AddColReal:
			{
				Color	&op1 = execStack.pop<Color>();
				Color	 op2(execStack.pop<float>());
				execStack.push(op1 + op2);
			}
			break;

		case AddColVec:
			{
				Color	&op1 = execStack.pop<Color>();
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(op1 + op2);
			}
			break;

		case AddColCol:
			{
				Color	&op1 = execStack.pop<Color>();
				Color	&op2 = execStack.pop<Color>();
				execStack.push(op1 + op2);
			}
			break;
#pragma endregion
		
#pragma region Substraction
		case SubRealReal:
			{
				float &op1 = execStack.pop<float>();
				float &op2 = execStack.pop<float>();
				execStack.push(op1 - op2);
			}
			break;

		case SubRealVec:
			{
				Vector3	 op1(execStack.pop<float>());
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(op1 - op2);
			}
			break;

		case SubRealCol:
			{
				Color	 op1(execStack.pop<float>());
				Color	&op2 = execStack.pop<Color>();
				execStack.push(op1 - op2);
			}
			break;

		case SubVecReal:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Vector3	 op2(execStack.pop<float>());
				execStack.push(op1 - op2);
			}
			break;

		case SubVecVec:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(op1 - op2);
			}
			break;

		case SubVecCol:
			{
				Vector3	&op1 = execStack.pop<Vector3>();
				Color	&op2 = execStack.pop<Color>();
				execStack.push(op1 - op2);
			}
			break;

		case SubColReal:
			{
				Color	&op1 = execStack.pop<Color>();
				Color	 op2(execStack.pop<float>());
				execStack.push(op1 - op2);
			}
			break;

		case SubColVec:
			{
				Color	&op1 = execStack.pop<Color>();
				Vector3	&op2 = execStack.pop<Vector3>();
				execStack.push(op1 - op2);
			}
			break;

		case SubColCol:
			{
				Color	&op1 = execStack.pop<Color>();
				Color	&op2 = execStack.pop<Color>();
				execStack.push(op1 - op2);
			}
			break;
#pragma endregion

#pragma region Popping
		case PopVecVec:
			{
				VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				var = execStack.pop<Vector3>();
			}
			break;

		case PopVecReal:
			{
				VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				var = Vector3(execStack.pop<float>());
			}
			break;

		case PopVecCol:
			{
				VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				var = Vector3(execStack.pop<Color>());
			}
			break;

		case PopColCol:
			{
				VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				var = execStack.pop<Color>();
			}
			break;

		case PopColReal:
			{
				VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				var = Color(execStack.pop<float>());
			}
			break;

		case PopColVec:
			{
				VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				var = Color(execStack.pop<Vector3>());
			}
			break;

		case PopRealReal:
			{
				VarValue &var = rtVarTable[boost::get<int>(eip->second)];
				var = execStack.pop<float>();
			}
			break;
#pragma endregion

		default:
			//! \todo throw unsupported operation exception
			break;
		};

		++eip;
	}
}
