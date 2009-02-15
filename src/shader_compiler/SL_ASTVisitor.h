#ifndef CRT_SL_ASTVISITOR_H
#define CRT_SL_ASTVISITOR_H

class SLNode;
class TermNode;
class FileRootNode;
class ShaderRootNode;
class FormalsNode;
class BlockNode;
class VarDeclBlockNode;
class VarDefNode;
class VarDefMultExprNode;
class VarDefExprNode;
class VarInitNode;
class StmtListNode;
class StmtNode;
class ReturnStmtNode;
class ExprNode;
class PrimNode;
class AsgnExprNode;
class MultExprNode;
class TypeCtorNode;
class ProcCallNode;
class ProcArgsNode;

class SL_ASTVisitor
{
public:
	virtual void visit(TermNode &node)				= 0;
	virtual void visit(FileRootNode &node)			= 0;
	virtual void visit(ShaderRootNode &node)		= 0;
	virtual void visit(FormalsNode &node)			= 0;
	virtual void visit(BlockNode &node)				= 0;
	virtual void visit(VarDeclBlockNode &node)		= 0;
	virtual void visit(VarDefNode &node)			= 0;
	virtual void visit(VarDefMultExprNode &node)	= 0;
	virtual void visit(VarDefExprNode &node)		= 0;
	virtual void visit(VarInitNode &node)			= 0;
	virtual void visit(StmtListNode &node)			= 0;
	virtual void visit(StmtNode &node)				= 0;
	virtual void visit(ReturnStmtNode &node)		= 0;
	virtual void visit(ExprNode &node)				= 0;
	virtual void visit(PrimNode &node)				= 0;
	virtual void visit(AsgnExprNode &node)			= 0;
	virtual void visit(MultExprNode &node)			= 0;
	virtual void visit(TypeCtorNode &node) 			= 0;
	virtual void visit(ProcCallNode &node) 			= 0;
	virtual void visit(ProcArgsNode &node) 			= 0;

protected:
	void visitChildrenOf(SLNode &node);

protected:
	SL_ASTVisitor() {}
};

#endif
