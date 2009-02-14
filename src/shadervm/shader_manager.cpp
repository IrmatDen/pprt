#include <iostream>
#include <fstream>
#include <functional>

#include "shader_manager.h"

#include "../scene/ray.h"
#include "../scene/vector3.h"
#include "../scene/scene.h"

#include "../shader_compiler/SL_ASTCreator.h"
#include "../shader_compiler/SL_ASTNodes.h"
#include "../shader_compiler/SL_ASTVisitor.h"
#include "../shader_compiler/stdout_visitor.h"
#include "../shader_compiler/useless_nodes_removal_visitor.h"

using namespace std;

ShaderManager::ShaderManager()
:scene(0)
{
	bool cgtLoaded = grammar.load("./src/shadervm/crtsl.cgt");
}

ShaderManager::~ShaderManager()
{
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
	vector<Token*>& tokens = scanner->getTokens();

	LALR *parser = grammar.getParser();
	parser->init(tokens);
	Symbol *sym = parser->parse(tokens, true, false);

	SL_ASTCreator astCreator;
	FileRootNode *root = (FileRootNode*)(astCreator.createTree(*sym));

	UselessNodesRemovalVisitor cleaner;
	cleaner.visit(*root);

	StdoutVisitor printer;
	printer.visit(*root);
	
	delete buffer;
	delete root;

	cout << "loaded " << fileName.c_str() << endl;
}

void ShaderManager::registerP(const Vec3 &v)
{
	env.P = v;
}

void ShaderManager::registerN(const Vec3 &v)
{
	env.N = v;
}

void ShaderManager::registerI(const Vec3 &v)
{
	env.I = v;
}

void ShaderManager::execute(const string &shaderName, const ShaderParams &params, Color4 &out)
{
}

void ShaderManager::diffuse(const Vec3 &N, Color4 &out) const
{
	scene->diffuse(Ray(env.P, N), out);
}

void ShaderManager::specular(const Vec3 &N, double roughness, Color4 &out) const
{
	scene->specular(Ray(env.P, N), env.I, roughness, out);
}
