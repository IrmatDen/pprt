#ifndef PPRT_SHADERVM_SHADER_MANAGER_H
#define PPRT_SHADERVM_SHADER_MANAGER_H

#include <map>
#include <string>

#include <CGTFile.h>
#undef byte

#include "compiled_shader.h"

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	void				setScene(Scene &scn)					{ scene = &scn; }

	void				reset();
	void				clear();
	void				loadFile(const std::string &fileName);
	void				addShader(const CompiledShader &shader);
	CompiledShader		instanciate(const std::string &shaderName) const;

private:
	typedef std::map<std::string, CompiledShader>				CompiledShaders;
	typedef std::map<std::string, CompiledShader::ByteCode*>	ShadersCode;

private:
	Scene				*	scene;
	CompiledShaders			shaders;
	ShadersCode				shadersCodes;
	CGTFile					grammar;
};

#endif
