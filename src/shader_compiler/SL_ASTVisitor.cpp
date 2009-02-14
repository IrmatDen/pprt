#include "SL_ASTVisitor.h"
#include "SL_ASTNodes.h"

void SL_ASTVisitor::visitChildrenOf(SLNode &node)
{
	vector<ASTNode*> &children = *node.getChildren();
	for(vector<ASTNode*>::iterator it = children.begin();
		it != children.end();
		++it)
	{
		if(*it != 0)
			((SLNode*)(*it))->accept(*this);
	}
}
