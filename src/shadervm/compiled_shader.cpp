#include <cassert>
#include <vector>
#include <algorithm>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "compiled_shader.h"

#include "../Scene/color4.h"

using namespace std;
using namespace boost;

CompiledShader::OpCodeMapping CompiledShader::opCodeMappings;
CompiledShader::FunctionMapping CompiledShader::fnMappings;
bool CompiledShader::opCodeMappingsInitialized = false;

void initOpCodeMappings()
{
	// mnemonic - Opcodes mapping
	CompiledShader::opCodeMappings["push"]	= CompiledShader::Pushd;
	CompiledShader::opCodeMappings["call"]	= CompiledShader::Call;
	CompiledShader::opCodeMappings["pop"]	= CompiledShader::Pop;

	// Function name - fn pointers mapping
	CompiledShader::fnMappings["color4"]	= CompiledShader::ShaderFunction(&CompiledShader::color4Ctor);
}

CompiledShader::CompiledShader(ShaderType type)
{
	if (!opCodeMappingsInitialized)
	{
		initOpCodeMappings();
		opCodeMappingsInitialized = true;
	}
	
	addVar(VST_Varying, VT_Color, "out", Color4());

	switch(type)
	{
	case ST_Surface:
		break;
	}
}

CompiledShader::CompiledShader(const CompiledShader &other)
:shaderName(other.shaderName), varTable(other.varTable),
code(other.code)
{
}

CompiledShader& CompiledShader::operator=(const CompiledShader &other)
{
	shaderName	= other.shaderName;
	varTable	= other.varTable;
	code		= other.code;

	return *this;
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
			bc.second = lexical_cast<double>(tokens[1]);
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
	eip = code.begin();

	while (eip != code.end())
	{
		switch(eip->first)
		{
		case Pushd:
			execStack.push(make_pair(VT_Double, eip->second));
			break;

		case Pushv:
			{
				const Variable &var = varTable[any_cast<int>(eip->second)];
				execStack.push(make_pair(var.type, var.content));
				break;
			}

		case Call:
			(this->*any_cast<ShaderFunction>(eip->second))();
			break;

		case Pop:
			{
				Variable &var = varTable[any_cast<int>(eip->second)];

				//! \todo replace assert by exception
				assert(execStack.size() >= 1);
				ProgramStackElement pse = execStack.top();
				var.content = pse.second;

				execStack.pop();
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
