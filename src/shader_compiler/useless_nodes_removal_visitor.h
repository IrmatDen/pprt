#ifndef PPRT_USELESS_NODES_REMOVAL_VISITOR_H
#define PPRT_USELESS_NODES_REMOVAL_VISITOR_H

#include "SL_ASTVisitor.h"

#include <stack>

class UselessNodesRemovalVisitor : public SL_ASTVisitor
{
public:
	DECLARE_VISIT_FUNCS

private:
	void deleteUselessNodes();
	void updateParentChildren(SLNode &removedNode, SLNode &newChild);
	void reparentAllChildren(SLNode &newParent, SLNode &removedNode);

private:
	std::stack<SLNode*>		uselessNodes;
};

#endif
