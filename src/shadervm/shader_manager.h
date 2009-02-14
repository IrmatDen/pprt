#ifndef CRT_SHADERVM_SHADER_MANAGER_H
#define CRT_SHADERVM_SHADER_MANAGER_H

#include <map>
#include <string>

#include <CGTFile.h>

#include "compiled_shader.h"

class ShaderManager
{
public:
	ShaderManager();

	void				loadFile(const std::string &fileName);
	CompiledShader		instanciate(const std::string &shaderName) const;

private:
	typedef std::map<std::string, CompiledShader> CompiledShaders;

private:
	CompiledShaders		shaders;
	CGTFile				grammar;
};

#endif
