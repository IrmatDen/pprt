#include <vector>
#include <algorithm>
#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "compiled_shader.h"

#include "../Scene/color4.h"

using namespace std;
using namespace boost;

CompiledShader::OpCodeMapping CompiledShader::opCodeMappings;
bool CompiledShader::opCodeMappingsInitialized = false;

void initOpCodeMappings()
{
	CompiledShader::opCodeMappings["push"]	= CompiledShader::Push;
	CompiledShader::opCodeMappings["call"]	= CompiledShader::Call;
	CompiledShader::opCodeMappings["pop"]	= CompiledShader::Pop;
}

CompiledShader::CompiledShader()
{
	if (!opCodeMappingsInitialized)
	{
		initOpCodeMappings();
		opCodeMappingsInitialized = true;
	}
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

	// Set operation argument
	if (bc.first == Push)
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
				bc.first	= Pushv;
				bc.second	= varIdx;
			}
			else
			{
				//! \todo Throw an exception as variable is unknown
			}
		}
	}
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
	else if (bc.first == Call)
	{
		bc.second = mem_fun_t<void, CompiledShader>(&CompiledShader::color4Ctor);
	}

	code.push(bc);
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
