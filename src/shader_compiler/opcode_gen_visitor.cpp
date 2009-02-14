#include <iostream>

#include <ASTCreator.h>

#include "opcode_gen_visitor.h"
#include "SL_ASTNodes.h"

using namespace std;

void OpCodeGenVisitor::visit(TermNode &node)
{
}

void OpCodeGenVisitor::visit(FileRootNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(ShaderRootNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(BlockNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(VarDeclBlockNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(VarDefNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(VarDefMultExprNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(VarDefExprNode &node)
{
	// If the expression is a terminal node, push it
	if (node.getChildren()->size() == 2)
	{
		ASTNode &expr = *node.getChildren()->at(1);
		if (expr.getChildren()->size() == 0)
		{
			wcout << "PUSH " << expr.getImage() << endl;
		}
		else
		{
			((SLNode*)&expr)->accept(*this);
		}

		wcout << "POP " << node.getChildren()->at(0)->getImage() << endl;
	}
}

void OpCodeGenVisitor::visit(VarInitNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(StmtListNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(StmtNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(ReturnStmtNode &node)
{
	// If the returned expression is a terminal node, push it; otherwise evaluate it
	ASTNode &retExpr = *node.getChildren()->at(0);
	if (retExpr.getChildren()->size() == 0)
	{
		wcout << "PUSH " << retExpr.getImage() << endl;
	}
	else
	{
		((SLNode*)&retExpr)->accept(*this);
	}

	wcout << "RET" << endl;
}

void OpCodeGenVisitor::visit(ExprNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(PrimNode &node)
{
	visitChildrenOf(node);
}

void OpCodeGenVisitor::visit(AsgnExprNode &node)
{
	// If the expression is a terminal node, push it
	ASTNode &expr = *node.getChildren()->at(1);
	if (expr.getChildren()->size() == 0)
	{
		wcout << "PUSH " << expr.getImage() << endl;
	}
	else
	{
		((SLNode*)&expr)->accept(*this);
	}

	wcout << "POP " << node.getChildren()->at(0)->getImage() << endl;
}

void OpCodeGenVisitor::visit(TypeCtorNode &node)
{
	// Evaluate arguments, if any
	vector<ASTNode*> &children = *node.getChildren();
	if (children.size() == 2)
	{
		((SLNode*)children[1])->accept(*this);
	}

	wcout << "CALL " << children[0]->getImage() << endl;
}

void OpCodeGenVisitor::visit(ProcCallNode &node)
{
	// Evaluate arguments, if any
	vector<ASTNode*> &children = *node.getChildren();
	if (children.size() == 2)
	{
		((SLNode*)children[1])->accept(*this);
	}

	wcout << "CALL " << children[0]->getImage() << endl;
}

void OpCodeGenVisitor::visit(ProcArgsNode &node)
{
	vector<ASTNode*> &children = *node.getChildren();
	for(vector<ASTNode*>::reverse_iterator it = children.rbegin(); it != children.rend(); ++it)
	{
		ASTNode *c = *it;
		if (c->getChildren()->size() == 0)
		{
			wcout << "PUSH " << c->getImage() << endl;
		}
		else
		{
			((SLNode*)c)->accept(*this);
		}
	}
}
