#ifndef CRT_SL_ASTCREATOR_H
#define CRT_SL_ASTCREATOR_H

#include <ASTCreator.h>

class SL_ASTCreator : public ASTCreator
{
protected:
	virtual ASTNode* getASTNode (const Symbol *reduction, ASTNode *parent);
};

#endif
