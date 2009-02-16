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

	void				setScene(Scene &scn)					{ scene = &scn; }

	void				loadFile(const std::string &fileName);

	void				addShader(const CompiledShader &shader);

	CompiledShader		instanciate(const std::string &shaderName) const;

private:
	typedef std::map<std::string, CompiledShader> CompiledShaders;

private:
	Scene				*	scene;
	CompiledShaders			shaders;
	CGTFile					grammar;
};

#endif
