#include <iostream>
#include <sstream>
#include <fstream>
//#include <functional>

#include "shader_manager.h"
#include "compiled_shader.h"

#include "../shader_compiler/SL_ASTCreator.h"
#include "../shader_compiler/SL_ASTNodes.h"
#include "../shader_compiler/SL_ASTVisitor.h"
#include "../shader_compiler/stdout_visitor.h"
#include "../shader_compiler/useless_nodes_removal_visitor.h"
#include "../shader_compiler/mnemonic_gen_visitor.h"

using namespace std;

ShaderManager::ShaderManager()
{
	bool cgtLoaded = grammar.load("./src/shadervm/crtsl.cgt");
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

	UselessNodesRemovalVisitor cleaner;
	cleaner.visit(*root);

	StdoutVisitor printer;
	printer.visit(*root);

	cout << "loaded " << fileName.c_str() << endl;

	std::ostringstream oss;

	MnemonicGenVisitor mnemonicGen(oss);
	mnemonicGen.visit(*root);
	cout << oss.str();

	CompiledShader shader;
	shader.fromMnemonics(oss.str());

	shaders[shader.name()] = shader;
	
	delete buffer;
	delete root;
}

CompiledShader ShaderManager::instanciate(const std::string &shaderName) const
{
	//! \todo throw not found exception if needed
	CompiledShaders::const_iterator it = shaders.find(shaderName);
	if (it != shaders.end())
		return it->second;

	return CompiledShader();
}
