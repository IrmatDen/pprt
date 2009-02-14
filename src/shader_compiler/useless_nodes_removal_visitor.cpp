#include <iostream>

#include <ASTCreator.h>

#include "useless_nodes_removal_visitor.h"
#include "SL_ASTNodes.h"

using namespace std;

void UselessNodesRemovalVisitor::visit(TermNode &node)
{
}

void UselessNodesRemovalVisitor::visit(FileRootNode &node)
{
	visitChildrenOf(node);

	deleteUselessNodes();
}

void UselessNodesRemovalVisitor::visit(ShaderRootNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(BlockNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(VarDeclBlockNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(VarDefNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(VarDefMultExprNode &node)
{
	visitChildrenOf(node);
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
	if(children.size() == 1)
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

void UselessNodesRemovalVisitor::visit(StmtNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(ReturnStmtNode &node)
{
	visitChildrenOf(node);
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

	vector<ASTNode*> &children = *node.getChildren();
	if(children.size() == 1)
	{
		uselessNodes.push(&node);
		children[0]->setParent(node.getParent());
		updateParentChildren(node, *(SLNode*)children[0]);
		children.erase(children.begin());
	}
}

void UselessNodesRemovalVisitor::visit(AsgnExprNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(TypeCtorNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(ProcCallNode &node)
{
	visitChildrenOf(node);
}

void UselessNodesRemovalVisitor::visit(ProcArgsNode &node)
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
	cout << "Removed " << uselessNodes.size() << " nodes." << endl;

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
