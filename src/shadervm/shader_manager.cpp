#include "shader_manager.h"

using namespace std;

void ShaderManager::addShaderFromMnemonics(const string &mnemonics)
{
	CompiledShader cs;
	cs.fromMnemonics(mnemonics);

	shaders[cs.name()] = cs;
}

bool ShaderManager::containsShaderName(const std::string &shaderName) const
{
	return shaders.find(shaderName) != shaders.end();
}

CompiledShader ShaderManager::instanciate(const std::string &shaderName) const
{
	//! \todo throw not found exception if needed
	CompiledShaders::const_iterator it = shaders.find(shaderName);
	if (it != shaders.end())
		return it->second;

	return CompiledShader();
}
