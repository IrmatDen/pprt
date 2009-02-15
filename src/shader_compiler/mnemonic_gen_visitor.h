#ifndef CRT_MNEMONIC_GEN_VISITOR_H
#define CRT_MNEMONIC_GEN_VISITOR_H

#include <sstream>

#include "SL_ASTVisitor.h"

#include "../shadervm/compiled_shader.h"

class MnemonicGenVisitor : public SL_ASTVisitor
{
public:
	MnemonicGenVisitor():shader(0) {}
	~MnemonicGenVisitor()
	{
		delete shader;
	}

	virtual void visit(TermNode &node);
	virtual void visit(FileRootNode &node);
	virtual void visit(ShaderRootNode &node);
	virtual void visit(FormalsNode &node);
	virtual void visit(BlockNode &node);
	virtual void visit(VarDeclBlockNode &node);
	virtual void visit(VarDefNode &node);
	virtual void visit(VarDefMultExprNode &node);
	virtual void visit(VarDefExprNode &node);
	virtual void visit(VarInitNode &node);
	virtual void visit(StmtListNode &node);
	virtual void visit(StmtNode &node);
	virtual void visit(ReturnStmtNode &node);
	virtual void visit(ExprNode &node);
	virtual void visit(PrimNode &node);
	virtual void visit(AsgnExprNode &node);
	virtual void visit(MultExprNode &node);
	virtual void visit(TypeCtorNode &node);
	virtual void visit(ProcCallNode &node);
	virtual void visit(ProcArgsNode &node);

	CompiledShader* getShader()			{ return shader; }

private:
	CompiledShader *shader;
};

#endif
