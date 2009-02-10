#ifndef CRT_SL_ASTNODES_H
#define CRT_SL_ASTNODES_H

#include <ASTCreator.h>

#include "SL_ASTVisitor.h"

class SLNode : public ASTNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor) = 0;

protected:
	SLNode() {}
};

class TermNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class FileRootNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class ShaderRootNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class BlockNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class VarDeclBlockNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class VarDefNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class VarDefMultExprNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class VarDefExprNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class VarInitNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class StmtListNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class StmtNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class ReturnStmtNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class ExprNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class PrimNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class AsgnExprNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class TypeCtorNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class ProcCallNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

class ProcArgsNode : public SLNode
{
public:
	virtual void accept(SL_ASTVisitor &visitor)	{ visitor.visit(*this); }
};

#endif
