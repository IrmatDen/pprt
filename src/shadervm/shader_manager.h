#ifndef CRT_SHADERVM_SHADER_MANAGER_H
#define CRT_SHADERVM_SHADER_MANAGER_H

#include <map>
#include <string>

#include "compiled_shader.h"

class ShaderManager
{
public:
	void				addShaderFromMnemonics(const std::string &mnemonics);
	
	bool				containsShaderName(const std::string &shaderName) const;
	CompiledShader		instanciate(const std::string &shaderName) const;

private:
	typedef std::map<std::string, CompiledShader> CompiledShaders;

private:
	CompiledShaders		shaders;
};

#endif
