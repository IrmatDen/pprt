#ifndef PPRT_MNEMONIC_GEN_VISITOR_H
#define PPRT_MNEMONIC_GEN_VISITOR_H

#include <sstream>

#include "SL_ASTVisitor.h"

class ShaderManager;
class CompiledShader;

class MnemonicGenVisitor : public SL_ASTVisitor
{
public:
	MnemonicGenVisitor(ShaderManager &shaderManager);

	~MnemonicGenVisitor();

	DECLARE_VISIT_FUNCS

private:
	CompiledShader	*	shader;
	ShaderManager	&	shaderMgr;
};

#endif
