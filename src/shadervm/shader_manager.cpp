#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "shader_manager.h"
#include "compiled_shader.h"

#include "../shader_compiler/SL_ASTCreator.h"
#include "../shader_compiler/SL_ASTNodes.h"
#include "../shader_compiler/SL_ASTVisitor.h"
#include "../shader_compiler/stdout_visitor.h"
#include "../shader_compiler/useless_nodes_removal_visitor.h"
#include "../shader_compiler/mnemonic_gen_visitor.h"

#include "../memory.h"

using namespace std;

ShaderManager::ShaderManager()
:scene(nullptr)
{
	reset();
}

ShaderManager::~ShaderManager()
{
	clear();
}

void ShaderManager::reset()
{
	grammar.load("crtsl.cgt");

	clear();
}

void ShaderManager::clear()
{
	shaders.clear();

	for_each(shadersCodes.begin(), shadersCodes.end(),
		[] (ShadersCode::value_type &val) { delete [] val.second; } );
	shadersCodes.clear();
}

void ShaderManager::loadFile(const std::string &fileName)
{
	ifstream fileIn(fileName.c_str());
	fileIn.seekg(0, ios::end);
	unsigned int size = (unsigned int)fileIn.tellg();
	fileIn.seekg(0, ios::beg);
	char *buffer = new char[size+1];
	memset(buffer, 0, size+1);
	fileIn.read(buffer, size);

	DFA *scanner = grammar.getScanner();
	bool scanSuccess = scanner->scan(buffer);
	if (!scanSuccess)
	{
		scanner->getErrors()->print();
		delete buffer;
		return;
	}

	vector<Token*>& tokens = scanner->getTokens();

	LALR *parser = grammar.getParser();
	parser->init(tokens);
	Symbol *sym = parser->parse(tokens, false, false);

	SL_ASTCreator astCreator;
	FileRootNode *root = (FileRootNode*)(astCreator.createTree(*sym));
	if (!root)
	{
		cout << "Can't load " << fileName << endl;
		delete buffer;
		return;
	}

	UselessNodesRemovalVisitor cleaner;
	cleaner.visit(*root);

	/*StdoutVisitor printer;
	printer.visit(*root);*/

	MnemonicGenVisitor mnemonicGen(*this);
	mnemonicGen.visit(*root);

	cout << "Loaded " << fileName << endl;
	
	delete buffer;
	delete root;
}

void ShaderManager::addShader(const CompiledShader &shader)
{
	shaders[shader.name()] = shader;
	shaders[shader.name()].setScene(scene);

	CompiledShader::Instructions code = shader.getCode();
	CompiledShader::ByteCode *bcode = new CompiledShader::ByteCode[code.size()];
	shadersCodes[shader.name()] = bcode;
	for(CompiledShader::Instructions::iterator it = code.begin(); it != code.end(); ++it, ++bcode)
		*bcode = *it;

	cout << "Added shader: " << shader.name() << endl;
}

CompiledShader ShaderManager::instanciate(const std::string &shaderName) const
{
	CompiledShaders::const_iterator it = shaders.find(shaderName);
	ShadersCode::const_iterator cit = shadersCodes.find(shaderName);
	if (it != shaders.end())
		return it->second.cloneWithCodePtr(cit->second, it->second.getCode().size());

	cout << "Shader named \"" << shaderName << "\" can't be found, defaulting to matte" << endl;
	it = shaders.find("matte");
	cit = shadersCodes.find("matte");
	if (it != shaders.end())
		return it->second.cloneWithCodePtr(cit->second, it->second.getCode().size());
    
	cout << "\"matte\" shader can't be found, hide your eyes from an invasion of magenta" << endl;
	return CompiledShader();
}
