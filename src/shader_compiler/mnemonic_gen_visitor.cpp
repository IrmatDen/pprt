#include <ASTCreator.h>

#include "mnemonic_gen_visitor.h"
#include "SL_ASTNodes.h"

using namespace std;

std::string wstringToString(const std::wstring &ws)
{
	return std::string(ws.begin(), ws.end());
}

void MnemonicGenVisitor::visit(TermNode &node)
{
}

void MnemonicGenVisitor::visit(FileRootNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(ShaderRootNode &node)
{
	out << ".shaderid " << wstringToString(node.getChildren()->at(1)->getImage()) << endl << endl;

	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(BlockNode &node)
{
	nextVarInitStatementPos = node.getChildren()->at(1)->getChildren()->begin();

	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(VarDeclBlockNode &node)
{
	out << ".vars" << endl;

	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(VarDefNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(VarDefMultExprNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(VarDefExprNode &node)
{
	out << "uniform ";

	// Get type
	ASTNode *typeNode = node.getParent()->getParent();
	out << wstringToString(typeNode->getChildren()->at(0)->getImage()) << " ";

	// Get var name
	out << wstringToString(node.getChildren()->at(0)->getImage()) << endl;

	// If initializer is provided, defer it as a statement.
	if (node.getChildren()->size() == 2)
	{
		ASTNode *initializer = node.getChildren()->at(1);
		ASTNode *statementsNode = typeNode->getParent()->getParent()->getChildren()->at(1);
		vector<ASTNode*> &statementsNodeChildren = *statementsNode->getChildren();

		// Create the new statement
		StmtNode *statement = new StmtNode();
		statement->setImage(L"statement");
		statement->setSymbol(L"statement");
		statement->setParent(statementsNode);

		// As an assign expression
		AsgnExprNode *assignexpression = new AsgnExprNode();
		assignexpression->setImage(L"assignexpression");
		assignexpression->setSymbol(L"assignexpression");
		assignexpression->setParent(statement);
		statement->addChild(assignexpression);

		// Var name node
		TermNode *varNameNode = new TermNode();
		varNameNode->setParent(assignexpression);
		varNameNode->setImage(node.getChildren()->at(0)->getImage());
		varNameNode->setSymbol(node.getChildren()->at(0)->getSymbol());
		assignexpression->addChild(varNameNode);

		// Break link from this node with the initializer.
		assignexpression->addChild(initializer);
		initializer->setParent(assignexpression);
		node.getChildren()->erase(node.getChildren()->begin() + 1);

		// Prepend the newly created statement to the statements block
		nextVarInitStatementPos = statementsNodeChildren.insert(nextVarInitStatementPos, statement);
		nextVarInitStatementPos++;
		statementsNodeChildren[0];
	}
}

void MnemonicGenVisitor::visit(VarInitNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(StmtListNode &node)
{
	out << endl << ".code" << endl;

	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(StmtNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(ReturnStmtNode &node)
{
	// If the returned expression is a terminal node, push it; otherwise evaluate it
	ASTNode &retExpr = *node.getChildren()->at(0);
	if (retExpr.getChildren()->size() == 0)
	{
		out << "push " << wstringToString(retExpr.getImage()) << endl;
	}
	else
	{
		((SLNode*)&retExpr)->accept(*this);
	}

	out << "ret" << endl;
}

void MnemonicGenVisitor::visit(ExprNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(PrimNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(AsgnExprNode &node)
{
	// If the expression is a terminal node, push it
	ASTNode &expr = *node.getChildren()->at(1);
	if (expr.getChildren()->size() == 0)
	{
		out << "push " << wstringToString(expr.getImage()) << endl;
	}
	else
	{
		((SLNode*)&expr)->accept(*this);
	}

	out << "pop " << wstringToString(node.getChildren()->at(0)->getImage()) << endl;
}

void MnemonicGenVisitor::visit(TypeCtorNode &node)
{
	// Evaluate arguments, if any
	vector<ASTNode*> &children = *node.getChildren();
	if (children.size() == 2)
	{
		((SLNode*)children[1])->accept(*this);
	}

	out << "call " << wstringToString(children[0]->getImage()) << endl;
}

void MnemonicGenVisitor::visit(ProcCallNode &node)
{
	// Evaluate arguments, if any
	vector<ASTNode*> &children = *node.getChildren();
	if (children.size() == 2)
	{
		((SLNode*)children[1])->accept(*this);
	}

	out << "call " << wstringToString(children[0]->getImage()) << endl;
}

void MnemonicGenVisitor::visit(ProcArgsNode &node)
{
	vector<ASTNode*> &children = *node.getChildren();
	for(vector<ASTNode*>::reverse_iterator it = children.rbegin(); it != children.rend(); ++it)
	{
		ASTNode *c = *it;
		if (c->getChildren()->size() == 0)
		{
			out << "push " << wstringToString(c->getImage()) << endl;
		}
		else
		{
			((SLNode*)c)->accept(*this);
		}
	}
}
