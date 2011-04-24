#ifndef PPRT_SL_ASTVISITOR_H
#define PPRT_SL_ASTVISITOR_H

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
class AddExprNode;
class SubExprNode;
class MultExprNode;
class DotExprNode;
class NegateExprNode;
class TypeCtorNode;
class ProcCallNode;
class ProcArgsNode;
class ProcArgsListNode;

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
	virtual void visit(AddExprNode &node)			= 0;
	virtual void visit(SubExprNode &node)			= 0;
	virtual void visit(MultExprNode &node)			= 0;
	virtual void visit(DotExprNode &node)			= 0;
	virtual void visit(NegateExprNode &node)		= 0;
	virtual void visit(TypeCtorNode &node) 			= 0;
	virtual void visit(ProcCallNode &node) 			= 0;
	virtual void visit(ProcArgsNode &node) 			= 0;
	virtual void visit(ProcArgsListNode &node) 		= 0;

protected:
	void visitChildrenOf(SLNode &node);

protected:
	SL_ASTVisitor() {}
};

#define DECLARE_VISIT_FUNCS \
	virtual void visit(TermNode &node); \
	virtual void visit(FileRootNode &node); \
	virtual void visit(ShaderRootNode &node); \
	virtual void visit(FormalsNode &node); \
	virtual void visit(BlockNode &node); \
	virtual void visit(VarDeclBlockNode &node); \
	virtual void visit(VarDefNode &node); \
	virtual void visit(VarDefMultExprNode &node); \
	virtual void visit(VarDefExprNode &node); \
	virtual void visit(VarInitNode &node); \
	virtual void visit(StmtListNode &node); \
	virtual void visit(StmtNode &node); \
	virtual void visit(ReturnStmtNode &node); \
	virtual void visit(ExprNode &node); \
	virtual void visit(PrimNode &node); \
	virtual void visit(AsgnExprNode &node); \
	virtual void visit(AddExprNode &node); \
	virtual void visit(SubExprNode &node); \
	virtual void visit(MultExprNode &node); \
	virtual void visit(DotExprNode &node); \
	virtual void visit(NegateExprNode &node); \
	virtual void visit(TypeCtorNode &node); \
	virtual void visit(ProcCallNode &node); \
	virtual void visit(ProcArgsNode &node); \
	virtual void visit(ProcArgsListNode &node); \

#define DEFINE_VISIT_EMPTY_NODE(CLASS, NODETYPE) \
	void CLASS::visit(NODETYPE &node)	{}

#define DEFINE_VISIT_CHILDREN_NODE(CLASS, NODETYPE) \
	void CLASS::visit(NODETYPE &node) \
	{ \
		visitChildrenOf(node); \
	} \

#endif
