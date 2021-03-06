#include <iostream>

#include <ASTCreator.h>

#include "useless_nodes_removal_visitor.h"
#include "SL_ASTNodes.h"

using namespace std;

DEFINE_VISIT_EMPTY_NODE(UselessNodesRemovalVisitor, TermNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, ShaderRootNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, BlockNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, VarDefNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, StmtNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, ReturnStmtNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, AsgnExprNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, NegateExprNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, TypeCtorNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, ProcCallNode)
DEFINE_VISIT_CHILDREN_NODE(UselessNodesRemovalVisitor, ProcArgsNode)

void UselessNodesRemovalVisitor::visit(FileRootNode &node)
{
	visitChildrenOf(node);

	// Eventually flatten "definitions" tree
	if (node.getChildren()->size() == 2)
	{
		ASTNode *defsChild = (*node.getChildren())[1];
		if(defsChild->getImage() == L"definitions" && defsChild->getChildren()->size() > 0)
		{
			uselessNodes.push((SLNode*)defsChild);
			reparentAllChildren(node, *(SLNode*)defsChild);
			defsChild->getChildren()->clear();
			node.getChildren()->erase(node.getChildren()->begin() + 1);
		}
	}

	deleteUselessNodes();
}

void UselessNodesRemovalVisitor::visit(FormalsNode &node)
{
	visitChildrenOf(node);

	// Eventually flatten "formals" tree
	if (node.getChildren()->size() == 2)
	{
		ASTNode *varsChild = (*node.getChildren())[1];
		if(varsChild->getImage() == L"formals")
		{
			uselessNodes.push((SLNode*)varsChild);
			reparentAllChildren(node, *(SLNode*)varsChild);
			varsChild->getChildren()->clear();
			node.getChildren()->erase(node.getChildren()->begin() + 1);
		}
	}
}

void UselessNodesRemovalVisitor::visit(VarDeclBlockNode &node)
{
	visitChildrenOf(node);

	// Eventually flatten "variables" tree
	if (node.getChildren()->size() == 2)
	{
		ASTNode *varsChild = (*node.getChildren())[1];
		if(varsChild->getImage() == L"variables")
		{
			uselessNodes.push((SLNode*)varsChild);
			reparentAllChildren(node, *(SLNode*)varsChild);
			varsChild->getChildren()->clear();
			node.getChildren()->erase(node.getChildren()->begin() + 1);
		}
	}
}

void UselessNodesRemovalVisitor::visit(VarDefMultExprNode &node)
{
	visitChildrenOf(node);

	// Eventually flatten "def_expressions" tree
	if (node.getChildren()->size() == 2)
	{
		ASTNode *varsChild = (*node.getChildren())[1];
		if(varsChild->getImage() == L"def_expressions")
		{
			uselessNodes.push((SLNode*)varsChild);
			reparentAllChildren(node, *(SLNode*)varsChild);
			varsChild->getChildren()->clear();
			node.getChildren()->erase(node.getChildren()->begin() + 1);
		}
	}
}

void UselessNodesRemovalVisitor::visit(VarDefExprNode &node)
{
	if (node.getChildren()->at(1) == 0)
		node.getChildren()->pop_back();

	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(VarInitNode &node)
{
	visitChildrenOf(node);

	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1 && children[0]->getSymbol() != L"identifier")
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(StmtListNode &node)
{
	visitChildrenOf(node);

	// Eventually flatten "statements" tree
	ASTNode *firstChild = (*node.getChildren())[0];
	if(firstChild->getImage() == L"statements")
	{
		uselessNodes.push((SLNode*)firstChild);
		reparentAllChildren(node, *(SLNode*)firstChild);
		firstChild->getChildren()->clear();
		node.getChildren()->push_back(node.getChildren()->at(1));
		node.getChildren()->erase(node.getChildren()->begin(), node.getChildren()->begin() + 2);
	}
}

void UselessNodesRemovalVisitor::visit(ExprNode &node)
{
	visitChildrenOf(node);

	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1)
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(PrimNode &node)
{
	visitChildrenOf(node);

	// Remove this node in case it's only a passthrough
	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1)
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(AddExprNode &node)
{
	visitChildrenOf(node);

	// Remove this node in case it's only a passthrough
	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1)
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(SubExprNode &node)
{
	visitChildrenOf(node);

	// Remove this node in case it's only a passthrough
	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1)
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(MultExprNode &node)
{
	visitChildrenOf(node);

	// Remove this node in case it's only a passthrough
	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1)
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(DotExprNode &node)
{
	visitChildrenOf(node);

	// Remove this node in case it's only a passthrough
	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1)
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(ProcArgsListNode &node)
{
	visitChildrenOf(node);

	std::vector<ASTNode*> &children = *node.getChildren();
	if (children.size() == 2 && children[1]->getImage() == L"proc_arguments")
	{
		uselessNodes.push((SLNode*)children[1]);
		reparentAllChildren(node, *(SLNode*)children[1]);
		children[1]->getChildren()->clear();
		children.erase(children.begin() + 1);
	}
}

void UselessNodesRemovalVisitor::deleteUselessNodes()
{
	while(!uselessNodes.empty())
	{
		delete uselessNodes.top();
		uselessNodes.pop();
	}
}

void UselessNodesRemovalVisitor::updateParentChildren(SLNode &removedNode, SLNode &newChild)
{
	vector<ASTNode*> &parentChildren	= *removedNode.getParent()->getChildren();
	for(vector<ASTNode*>::iterator it = parentChildren.begin(); it != parentChildren.end(); ++it)
	{
		if (*it == &removedNode)
		{
			*it = &newChild;
			return;
		}
	}
}

void UselessNodesRemovalVisitor::reparentAllChildren(SLNode &newParent, SLNode &removedNode)
{
	vector<ASTNode*> &children = *removedNode.getChildren();
	for(vector<ASTNode*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		(*it)->setParent(&newParent);
		newParent.addChild(*it);
	}
}
