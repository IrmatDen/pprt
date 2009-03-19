#include <cassert>
#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "compiled_shader.h"

#include "../scene/scene.h"

using namespace std;
using namespace boost;

CompiledShader::OpCodeMapping	CompiledShader::opCodeMappings;
CompiledShader::FunctionMapping CompiledShader::fnMappings;
bool							CompiledShader::opCodeMappingsInitialized = false;

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
	CompiledShader::fnMappings["color"]		= CompiledShader::ShaderFunction(&CompiledShader::colorCtor);
	CompiledShader::fnMappings["vec3"]		= CompiledShader::ShaderFunction(&CompiledShader::vec3Ctor);
	
		// Mathematical
	CompiledShader::fnMappings["smoothstep"]	= CompiledShader::ShaderFunction(&CompiledShader::smoothstep);
		
		// Geometric
	CompiledShader::fnMappings["faceforward"]	= CompiledShader::ShaderFunction(&CompiledShader::faceForward);
	CompiledShader::fnMappings["normalize"]		= CompiledShader::ShaderFunction(&CompiledShader::normalize);
	CompiledShader::fnMappings["reflect"]		= CompiledShader::ShaderFunction(&CompiledShader::reflect);
	
		// Color
	CompiledShader::fnMappings["mix"]		= CompiledShader::ShaderFunction(&CompiledShader::mix);
		
		// Shading & lighting
	CompiledShader::fnMappings["diffuse"]	= CompiledShader::ShaderFunction(&CompiledShader::diffuse);
	CompiledShader::fnMappings["specular"]	= CompiledShader::ShaderFunction(&CompiledShader::specular);
	CompiledShader::fnMappings["trace"]		= CompiledShader::ShaderFunction(&CompiledShader::trace);
}

CompiledShader::CompiledShader(ShaderType shaderType)
:type(shaderType), scene(0)
{
	if (!opCodeMappingsInitialized)
	{
		initOpCodeMappings();
		opCodeMappingsInitialized = true;
	}
	
	// input
	addVar(VST_Varying, VT_Color,	"Cs",	Color());
	addVar(VST_Varying, VT_Color,	"Os",	Color());
	addVar(VST_Varying, VT_Vector,	"P",	Vec3());
	addVar(VST_Varying, VT_Vector,	"N",	Vec3());
	addVar(VST_Varying, VT_Vector,	"Ng",	Vec3());
	addVar(VST_Varying, VT_Vector,	"s",	0.f);
	addVar(VST_Varying, VT_Vector,	"t",	0.f);
	addVar(VST_Varying, VT_Vector,	"I",	Vec3());

	// output
	addVar(VST_Varying, VT_Color,	"Ci",	Color());
	addVar(VST_Varying, VT_Color,	"Oi",	Color());

	switch(type)
	{
	case ST_Surface:
		break;
	}
}

CompiledShader::CompiledShader(const CompiledShader &other, bool runtime)
:varTable(other.varTable),
codePtr(other.codePtr), codeSize(other.codeSize), codePtrEnd(other.codePtrEnd),
scene(other.scene)
{
	if (!runtime)
	{
		type = other.type;
		shaderName = other.shaderName;
		code = other.code;
	}
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

void CompiledShader::addVar(VariableStorageType varST, VariableType varT, const std::string &name, const VarValue &value)
{
	Variable v;
	v.storageType	= varST;
	v.type			= varT;
	v.name			= name;
	v.content		= value;
	
	varTable.push_back(v);
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
	assert(index < varTable.size());

	varTable[index].content = value;
}

void CompiledShader::getOutput(Color &color, Color &opacity)
{
	color	= varTable[Ci].content;
	opacity	= varTable[Oi].content;
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
			bc.second = lexical_cast<float>(tokens[1]);
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

void CompiledShader::exec()
{
	eip = codePtr;
	esp = execStack;

	while (eip != codePtrEnd)
	{
		switch(eip->first)
		{
		case Pushd:
			esp->first = VT_Float;
			esp->second = boost::get<float>(eip->second);
			++esp;
			break;

		case Pushv:
			{
				const Variable &var = varTable[boost::get<int>(eip->second)];
				esp->first = var.type;
				esp->second = var.content;
				++esp;
				break;
			}

		case Call:
			{
				ShaderFunction f = boost::get<ShaderFunction>(eip->second);
				(this->*f)();
				break;
			}

		case Dot:
			{
				--esp;	ProgramStackElement &op1 = *esp;
				--esp;	ProgramStackElement &op2 = *esp;

				Vec3	&op1v = op1.second;
				Vec3	&op2v = op2.second;
				esp->first = VT_Float;
				esp->second = op1v.dot(op2v);
				++esp;
				break;
			}

		case Negate:
			{
				--esp;	ProgramStackElement &op = *esp;
				switch(op.first)
				{
				case VT_Float:
					{
						float v = op.second;
						esp->second = -v;
						break;
					}

				case VT_Vector:
					{
						const Vec3 &v = op.second;
						esp->second = Vec3(-v.x, -v.y, -v.z, v.w);
						break;
					}
				}
				++esp;
				break;
			}

		case Mult:
			{
				--esp;	ProgramStackElement &op1 = *esp;
				--esp;	ProgramStackElement &op2 = *esp;
				switch(op1.first)
				{
				case VT_Float:
					{
						float &op1r = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float &op2d = op2.second;
								esp->first = VT_Float;
								esp->second = op1r * op2d;
								break;
							}
							
						case VT_Color:
							{
								Color	&op2c = op2.second;
								esp->first = VT_Color;
								esp->second = op2c * op1r;
								break;
							}
							
						case VT_Vector:
							{
								Vec3	&op2v = op2.second;
								esp->first = VT_Vector;
								esp->second = op2v * op1r;
								break;
							}
						}
						++esp;
						break;
					}

				case VT_Color:
					{
						Color	&op1c = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float	op2f = op2.second;
								esp->second = op1c * op2f;
								break;
							}
							
						case VT_Vector:
						case VT_Color:
							{
								Color	&op2c = op2.second;
								esp->second = op2c * op1c;
								break;
							}
						}
						esp->first = VT_Color;
						++esp;
						break;
					}

				case VT_Vector:
					{
						switch(op2.first)
						{
						case VT_Float:
							{
								Vec3 &op1v = op1.second;
								float op2d = op2.second;
								esp->first = VT_Vector;
								esp->second = op1v * op2d;
								break;
							}
							
						case VT_Color:
							{
								Color	&op1c = op1.second;
								Color	&op2c = op2.second;
								esp->first = VT_Color;
								esp->second = op2c * op1c;
								break;
							}
							
						case VT_Vector:
							{
								// vector by vector mult is not supported by the mult operator;
								// only by dot & cross products
								esp->first = VT_Vector;
								esp->second = Vec3();
								break;
							}
						}
						++esp;
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
				case VT_Float:
					{
						float &op1r = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float &op2d = op2.second;
								esp->first = VT_Float;
								esp->second = op1r + op2d;
								break;
							}
							
						case VT_Color:
							{
								Color	&op2c = op2.second;
								esp->first = VT_Color;
								esp->second = op2c + op1r;
								break;
							}
							
						case VT_Vector:
							{
								Vec3	&op2v = op2.second;
								esp->first = VT_Vector;
								esp->second = op2v + op1r;
								break;
							}
						}
						++esp;
						break;
					}

				case VT_Color:
					{
						Color	&op1c = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float	op2f = op2.second;
								esp->second = op1c + op2f;
								break;
							}
							
						case VT_Vector:
						case VT_Color:
							{
								Color	&op2c = op2.second;
								esp->second = op2c + op1c;
								break;
							}
						}
						esp->first = VT_Color;
						++esp;
						break;
					}

				case VT_Vector:
					{
						Vec3	&op1v = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float	op2f = op2.second;
								esp->first = VT_Vector;
								esp->second = op1v + op2f;
								break;
							}
							
						case VT_Color:
							{
								Color	&op1c = op1.second;
								Color	&op2c = op2.second;
								esp->first = VT_Color;
								esp->second = op2c + op1c;
								break;
							}
							
						case VT_Vector:
							{
								Vec3	&op2v = op2.second;
								esp->first = VT_Vector;
								esp->second = op1v + op2v;
								break;
							}
						}
						++esp;
						break;
					}
				}
			}
			break;

		case Sub:
			{
				--esp;	ProgramStackElement &op1 = *esp;
				--esp;	ProgramStackElement &op2 = *esp;
				switch(op1.first)
				{
				case VT_Float:
					{
						float &op1r = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float &op2d = op2.second;
								esp->first = VT_Float;
								esp->second = op1r - op2d;
								break;
							}
							
						case VT_Color:
							{
								Color	&op2c = op2.second;
								esp->first = VT_Color;
								esp->second = op2c - op1r;
								break;
							}
							
						case VT_Vector:
							{
								Vec3	&op2v = op2.second;
								esp->first = VT_Vector;
								esp->second = op2v - op1r;
								break;
							}
						}
						++esp;
						break;
					}

				case VT_Color:
					{
						Color	&op1c = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float	op2f = op2.second;
								esp->second = op1c - op2f;
								break;
							}
							
						case VT_Vector:
						case VT_Color:
							{
								Color	&op2c = op2.second;
								esp->second = op2c - op1c;
								break;
							}
						}
						esp->first = VT_Color;
						++esp;
						break;
					}

				case VT_Vector:
					{
						Vec3	&op1v = op1.second;

						switch(op2.first)
						{
						case VT_Float:
							{
								float	op2f = op2.second;
								esp->first = VT_Vector;
								esp->second = op1v - op2f;
								break;
							}
							
						case VT_Color:
							{
								Color	&op1c = op1.second;
								Color	&op2c = op2.second;
								esp->first = VT_Color;
								esp->second = op2c - op1c;
								break;
							}
							
						case VT_Vector:
							{
								Vec3	&op2v = op2.second;
								esp->first = VT_Vector;
								esp->second = op1v - op2v;
								break;
							}
						}
						++esp;
						break;
					}
				}
			}
			break;

		case Pop:
			{
				--esp;
				ProgramStackElement &pse = *esp;
				Variable &var = varTable[boost::get<int>(eip->second)];
				if (var.type == pse.first)
					var.content = pse.second;
				else
				{
					switch (var.type)
					{
					case VT_Color:
						switch(pse.first)
						{
						case VT_Float:
							var.content = Color((float)pse.second);
							break;

						case VT_Vector:
							var.content = Color(pse.second);
							break;
						}
						break;
						
					case VT_Vector:
						switch(pse.first)
						{
						case VT_Float:
							var.content = Vec3((float)pse.second);
							break;

						case VT_Color:
							{
								Color &c = pse.second;
								var.content = Vec3(c.r, c.g, c.b);
								break;
							}
						}
						break;
					}
				}
			}

		default:
			//! \todo throw unsupported operation exception
			break;
		};

		++eip;
	}
}
