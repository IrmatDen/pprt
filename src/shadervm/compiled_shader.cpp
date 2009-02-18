#include <cassert>
#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "compiled_shader.h"

#include "../Scene/color4.h"
#include "../Scene/scene.h"

using namespace std;
using namespace boost;

CompiledShader::OpCodeMapping CompiledShader::opCodeMappings;
CompiledShader::FunctionMapping CompiledShader::fnMappings;
bool CompiledShader::opCodeMappingsInitialized = false;

void initOpCodeMappings()
{
	// mnemonic - Opcodes mapping
	CompiledShader::opCodeMappings["push"]	= CompiledShader::Pushd;
	CompiledShader::opCodeMappings["mult"]	= CompiledShader::Mult;
	CompiledShader::opCodeMappings["add"]	= CompiledShader::Add;
	CompiledShader::opCodeMappings["call"]	= CompiledShader::Call;
	CompiledShader::opCodeMappings["pop"]	= CompiledShader::Pop;
	CompiledShader::opCodeMappings["ret"]	= CompiledShader::Ret;

	// Function name - fn pointers mapping
		// Type ctors
	CompiledShader::fnMappings["color4"]	= CompiledShader::ShaderFunction(&CompiledShader::color4Ctor);
		
		// Geometric
	CompiledShader::fnMappings["faceforward"]	= CompiledShader::ShaderFunction(&CompiledShader::faceForward);
	CompiledShader::fnMappings["reflect"]	= CompiledShader::ShaderFunction(&CompiledShader::reflect);
		
		// Shading & lighting
	CompiledShader::fnMappings["diffuse"]	= CompiledShader::ShaderFunction(&CompiledShader::diffuse);
	CompiledShader::fnMappings["specular"]	= CompiledShader::ShaderFunction(&CompiledShader::specular);
	CompiledShader::fnMappings["trace"]	= CompiledShader::ShaderFunction(&CompiledShader::trace);
}

CompiledShader::CompiledShader(ShaderType shaderType)
:type(shaderType), scene(0)
{
	if (!opCodeMappingsInitialized)
	{
		initOpCodeMappings();
		opCodeMappingsInitialized = true;
	}
	
	addVar(VST_Varying, VT_Color,	"out",	Color4());
	addVar(VST_Varying, VT_Vector,	"P",	Vec3());
	addVar(VST_Varying, VT_Vector,	"N",	Vec3());
	addVar(VST_Varying, VT_Vector,	"Ng",	Vec3());
	addVar(VST_Varying, VT_Vector,	"I",	Vec3());

	switch(type)
	{
	case ST_Surface:
		break;
	}
}

CompiledShader::CompiledShader(const CompiledShader &other)
:type(other.type), shaderName(other.shaderName), varTable(other.varTable),
code(other.code), codePtr(other.codePtr), codeSize(other.codeSize), codePtrEnd(other.codePtrEnd),
scene(other.scene)
{
}

CompiledShader& CompiledShader::operator=(const CompiledShader &other)
{
	shaderName	= other.shaderName;
	varTable	= other.varTable;
	code		= other.code;
	scene		= other.scene;

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

void CompiledShader::addVar(VariableStorageType varST, VariableType varT, const std::string &name, boost::any value)
{
	Variable v;
	v.storageType	= varST;
	v.type			= varT;
	v.name			= name;
	v.content		= value;
	
	varTable.push_back(v);
}

void CompiledShader::setVarValue(const std::string &name, boost::any value)
{
	for(VariableTable::iterator it = varTable.begin(); it != varTable.end(); ++it)
	{
		if ((*it).name == name)
		{
			(*it).content = value;
		}
	}
}

void CompiledShader::setVarValueByIndex(size_t index, boost::any value)
{
	assert(index < varTable.size());

	varTable[index].content = value;
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

	// Push <something>
	if (bc.first == Pushd)
	{
		// Check if it's a number, in which case, it is pushed.
		try
		{
			bc.second = lexical_cast<Real>(tokens[1]);
		}
		catch (bad_lexical_cast &)
		{
			// Otherwise, it's a var name, so search for it
			int varIdx;
			if (findVarIdx(tokens[1], varIdx))
			{
				// Change the opcode to match the fact we're pushing a variable index and not a number.
				bc.first	= Pushv;
				bc.second	= varIdx;
			}
			else
			{
				//! \todo Throw an exception as variable is unknown
			}
		}
	}
	// Mult (single operand)
	else if (bc.first == Mult)
	{
		// Second operand of bytecode is useless (ATM that is)
	}
	// Pop <something>
	else if (bc.first == Pop)
	{
		// Popping is only possible in a variable, so no need to search something else.
		int varIdx;
		if (findVarIdx(tokens[1], varIdx))
		{
			bc.second	= varIdx;
		}
		else
		{
			//! \todo Throw an exception as variable is unknown
		}
	}
	// Call <proc>
	else if (bc.first == Call)
	{
		ShaderFunction f(0);
		if (findFunRef(tokens[1], f))
		{
			bc.second = f;
		}
		else
		{
			//! \todo Throw an exception as function is unknown
		}
	}

	code.push_back(bc);
}

bool CompiledShader::findVarIdx(const std::string &str, int &varIdx)
{
	size_t i;
	for (i = 0; i < varTable.size(); i++)
	{
		if (varTable.at(i).name == str)
		{
			varIdx	= (int)i;
			return true;
		}
	}

	return false;
}

bool CompiledShader::findFunRef(const std::string &str, ShaderFunction &fnRef)
{
	// Found instruction opcode
	FunctionMapping::const_iterator mappedFnRef = fnMappings.find(str);

	//! \todo throw exception if fnMappings doesn't contain the current function name.
	if (mappedFnRef == fnMappings.end())
	{
		fnRef = ShaderFunction(0);
		return false;
	}

	fnRef = mappedFnRef->second;

	return true;
}

void CompiledShader::exec(Color4 &out)
{
	eip = codePtr;
	esp = execStack;

	while (eip != codePtrEnd)
	{
		switch(eip->first)
		{
		case Pushd:
			*esp = make_pair(VT_Real, eip->second);
			++esp;
			break;

		case Pushv:
			{
				const Variable &var = varTable[*any_cast<int>(&eip->second)];
				*esp = make_pair(var.type, var.content);
				++esp;
				break;
			}

		case Call:
			(this->*any_cast<ShaderFunction>(eip->second))();
			break;

		case Mult:
			{
				--esp;	ProgramStackElement &op1 = *esp;
				--esp;	ProgramStackElement &op2 = *esp;
				switch(op1.first)
				{
				case VT_Real:
					{
						switch(op2.first)
						{
						case VT_Real:
							{
								Real op1d = any_cast<Real>(op1.second);
								Real op2d = any_cast<Real>(op2.second);
								*esp = make_pair(VT_Real, op1d * op2d);
								++esp;
								break;
							}
							
						case VT_Color:
							{
								float	op1f = (float)any_cast<Real>(op1.second);
								Color4	*op2c = any_cast<Color4>(&op2.second);
								*esp = make_pair(VT_Color, *op2c * op1f);
								++esp;
								break;
							}
							
						case VT_Vector:
							{
								Real	op1d = any_cast<Real>(op1.second);
								Vec3	*op2v = any_cast<Vec3>(&op2.second);
								*esp = make_pair(VT_Vector, *op2v * op1d);
								++esp;
								break;
							}
						}
						break;
					}

				case VT_Color:
					{
						switch(op2.first)
						{
						case VT_Real:
							{
								Color4	*op1c = any_cast<Color4>(&op1.second);
								float	op2f = (float)any_cast<Real>(op2.second);
								*esp = make_pair(VT_Color, *op1c * op2f);
								++esp;
								break;
							}
							
						case VT_Color:
							{
								Color4	*op1c = any_cast<Color4>(&op1.second);
								Color4	*op2c = any_cast<Color4>(&op2.second);
								*esp = make_pair(VT_Color, *op2c * *op1c);
								++esp;
								break;
							}
							
						case VT_Vector:
							{
								// Color by vector mult is non-sense
								*esp = make_pair(VT_Color, Color4(1, 0, 1, 1));
								++esp;
								break;
							}
						}
						break;
					}

				case VT_Vector:
					{
						switch(op2.first)
						{
						case VT_Real:
							{
								Vec3	*op1v = any_cast<Vec3>(&op1.second);
								Real	op2d = any_cast<Real>(op2.second);
								*esp = make_pair(VT_Vector, *op1v * op2d);
								++esp;
								break;
							}
							
						case VT_Color:
							{
								// vector by color mult is non-sense
								*esp = make_pair(VT_Vector, Vec3());
								++esp;
								break;
							}
							
						case VT_Vector:
							{
								// vector by vector mult is not supported by the mult operator;
								// only by dot & cross products
								*esp = make_pair(VT_Vector, Vec3());
								++esp;
								break;
							}
						}
						break;
					}
				}
			}
			break;

		case Add:
			{
				--esp;	ProgramStackElement &op1 = *esp;
				--esp;	ProgramStackElement &op2 = *esp;
				switch(op1.first)
				{
				case VT_Real:
					{
						switch(op2.first)
						{
						case VT_Real:
							{
								Real op1d = any_cast<Real>(op1.second);
								Real op2d = any_cast<Real>(op2.second);
								*esp = make_pair(VT_Real, op1d + op2d);
								++esp;
								break;
							}
							
						case VT_Color:
							{
								float	op1f = (float)any_cast<Real>(op1.second);
								Color4	*op2c = any_cast<Color4>(&op2.second);
								*esp = make_pair(VT_Color, *op2c + op1f);
								++esp;
								break;
							}
							
						case VT_Vector:
							{
								// Real + vec3 is unsupported
								*esp = make_pair(VT_Vector, Vec3());
								++esp;
								break;
							}
						}
						break;
					}

				case VT_Color:
					{
						switch(op2.first)
						{
						case VT_Real:
							{
								Color4	*op1c = any_cast<Color4>(&op1.second);
								float	op2f = (float)any_cast<Real>(op2.second);
								*esp = make_pair(VT_Color, *op1c + op2f);
								++esp;
								break;
							}
							
						case VT_Color:
							{
								Color4	*op1c = any_cast<Color4>(&op1.second);
								Color4	*op2c = any_cast<Color4>(&op2.second);
								*esp = make_pair(VT_Color, *op2c + *op1c);
								++esp;
								break;
							}
							
						case VT_Vector:
							{
								// Color + vector is non-sense
								*esp = make_pair(VT_Color, Color4(1, 0, 1, 1));
								++esp;
								break;
							}
						}
						break;
					}

				case VT_Vector:
					{
						switch(op2.first)
						{
						case VT_Real:
							{
								// vector + Real is not supported
								*esp = make_pair(VT_Vector, Vec3());
								++esp;
								break;
							}
							
						case VT_Color:
							{
								// vector + color is non-sense
								*esp = make_pair(VT_Vector, Vec3());
								++esp;
								break;
							}
							
						case VT_Vector:
							{
								Vec3	*op1v = any_cast<Vec3>(&op1.second);
								Vec3	*op2v = any_cast<Vec3>(&op2.second);
								*esp = make_pair(VT_Vector, *op1v + *op2v);
								++esp;
								break;
							}
						}
						break;
					}
				}
			}
			break;

		case Pop:
			{
				--esp;
				ProgramStackElement &pse = *esp;
				Variable &var = varTable[*any_cast<int>(&eip->second)];
				var.content = pse.second;
			}

		default:
			//! \todo throw unsupported operation exception
			break;
		};

		++eip;
	}

	// "out" variable should always be at index 0
	assert(varTable[0].name == "out");
	out = any_cast<Color4>(varTable[0].content);
}
