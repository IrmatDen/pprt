#ifndef PPRT_STDOUT_VISITOR_H
#define PPRT_STDOUT_VISITOR_H

#include "SL_ASTVisitor.h"

class StdoutVisitor : public SL_ASTVisitor
{
public:
	StdoutVisitor() : currentIndent(0) {}

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
	virtual void visit(AddExprNode &node);
	virtual void visit(SubExprNode &node);
	virtual void visit(MultExprNode &node);
	virtual void visit(DotExprNode &node);
	virtual void visit(NegateExprNode &node);
	virtual void visit(TypeCtorNode &node);
	virtual void visit(ProcCallNode &node);
	virtual void visit(ProcArgsNode &node);
	virtual void visit(ProcArgsListNode &node);

private:
	int currentIndent;
};

#endif
