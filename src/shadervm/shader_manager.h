#ifndef CRT_SHADERVM_SHADER_MANAGER_H
#define CRT_SHADERVM_SHADER_MANAGER_H

#include <map>
#include <string>

#include <CGTFile.h>

#include "../scene/color4.h"
#include "shader_param.h"

class Vec3;
class Scene;

class ShaderManager
{
public:
	~ShaderManager();

	static inline ShaderManager& getInstance()
	{
		static ShaderManager sm;
		return sm;
	}

	void setScene(Scene &scn)									{ scene = &scn; }

	void loadFile(const std::string &fileName);

	void registerP(const Vec3 &v);
	void registerN(const Vec3 &v);
	void registerI(const Vec3 &v);

	void execute(const std::string &shaderName, const ShaderParams &params, Color4 &out);

	// Functions acting as lua bindings to get scene related info
	void diffuse(const Vec3 &N, Color4 &out) const;
	void specular(const Vec3 &N, double roughness, Color4 &out) const;

private:
	ShaderManager();

private:
	struct ExecEnv
	{
		Vec3 P, N, I;
	} env;

private:
	Scene			*	scene;
	CGTFile				grammar;
};

#endif
