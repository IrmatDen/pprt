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
	CompiledShader::fnMappings["color"]		= CompiledShader::ShaderFunction(&CompiledShader::color4Ctor);
}

CompiledShader::CompiledShader()
{
	if (!opCodeMappingsInitialized)
	{
		initOpCodeMappings();
		opCodeMappingsInitialized = true;
	}
}

CompiledShader(const CompiledShader &other)
:shaderName(other.shaderName), varTable(other.varTable),
code(other.code)
{
}

void CompiledShader::fromMnemonics(const string &mnemonics)
{
	shaderName	= "";
	varTable	= VariableTable();
	code		= Instructions();

	typedef vector<string> Lines;
	Lines lines;
	split(lines, mnemonics, is_any_of("\n"));

	feedStandardVars();

	enum 
	{
		Segment_ShaderId,
		Segment_VarDecl,
		Segment_Code
	} processingStage = Segment_ShaderId;

	for (Lines::const_iterator lineIt = lines.begin(); lineIt != lines.end(); ++lineIt)
	{
		const string &line = *lineIt;
		if (line == "")
			continue;

		switch(processingStage)
		{
		case Segment_ShaderId:
			if (line.find(".shaderid") != string::npos)
				shaderName = line.substr(line.rfind(' '));
			//! \todo Throw exception if no .shaderId is found
			processingStage = Segment_VarDecl;
			break;

		case Segment_VarDecl:
			if (line == "" || line.find(".vars") != string::npos)
				continue;
			if (line.find(".code") != string::npos)
			{
				processingStage = Segment_Code;
				continue;
			}

			parseVarDecl(line);
			break;

		case Segment_Code:
			if (line == "")
				continue;
			parseInstr(line);
			break;

		default:
			//! \todo unknown processing stage, throw exception
			break;
		}
	}
}

void CompiledShader::feedStandardVars()
{
	// Feed output variables
	Variable out;
	out.storageType	= VST_Varying;
	out.type		= VT_Color;
	out.name		= "out";
	out.content		= Color4();
	
	varTable.push_back(out);
}

void CompiledShader::parseVarDecl(const string &varDecl)
{
	typedef vector<string> Tokens;
	Tokens tokens;
	split(tokens, varDecl, is_any_of(" "));

	//! \todo throw exception if tokens doesn't contain 3 strings

	Variable out;

	if (tokens[0] == "uniform")		out.storageType = VST_Uniform;
	else /* varying */				out.storageType = VST_Varying;
	
	if (tokens[1] == "color")
	{
		out.type	= VT_Color;
		out.content	= Color4();
	}
	//! \todo throw exception if variable type is unknown
	//else /* unknown */			
	
	out.name = tokens[2];
	
	varTable.push_back(out);
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
			execStack.push(make_pair(TF_Double, eip->second));
			break;

		case Pushv:
			{
				const Variable &var = varTable[any_cast<int>(eip->second)];
				switch(var.type)
				{
				case VT_Color:
					execStack.push(make_pair(TF_Color, any_cast<Color4>(var.content)));
					break;

				default:
					//! \todo throw unsupported source type exception
					break;
				}
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
				switch(pse.first)
				{
				case TF_Double:
					assert(var.type == VT_Double);
					var.content = pse.second;
					break;

				case TF_Color:
					assert(var.type == VT_Color);
					var.content = pse.second;
					break;

				default:
					//! \todo throw unsupported storable type exception
					break;
				}

				execStack.pop();
			}

		default:
			//! \todo throw unsupported operation exception
			break;
		};

		++eip;
	}

	// "out" variable should always be at index 0
	out = any_cast<Color4>(varTable[0].content);
}
