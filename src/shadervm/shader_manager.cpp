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

#include "../common.h"

using namespace std;

ShaderManager::ShaderManager()
:scene(0)
{
	bool cgtLoaded = grammar.load("./src/shadervm/crtsl.cgt");
}

ShaderManager::~ShaderManager()
{
	//! \todo free shader code
}

void ShaderManager::loadFile(const std::string &fileName)
{
	ifstream fileIn(fileName.c_str());
	fileIn.seekg(0, ios::end);
	int size = fileIn.tellg();
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
	CompiledShader::ByteCode *bcode = memory::construct<CompiledShader::ByteCode>(code.size());
	shadersCodes[shader.name()] = bcode;
	for(CompiledShader::Instructions::iterator it = code.begin(); it != code.end(); ++it, ++bcode)
		*bcode = *it;

	cout << "Added shader: " << shader.name() << endl;
}

CompiledShader ShaderManager::instanciate(const std::string &shaderName) const
{
	//! \todo throw not found exception if needed
	CompiledShaders::const_iterator it = shaders.find(shaderName);
	ShadersCode::const_iterator cit = shadersCodes.find(shaderName);
	if (it != shaders.end())
		return it->second.cloneWithCodePtr(cit->second, it->second.getCode().size());

	return CompiledShader();
}
