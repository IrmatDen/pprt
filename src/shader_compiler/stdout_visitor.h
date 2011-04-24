#ifndef PPRT_STDOUT_VISITOR_H
#define PPRT_STDOUT_VISITOR_H

#include "SL_ASTVisitor.h"

class StdoutVisitor : public SL_ASTVisitor
{
public:
	StdoutVisitor() : currentIndent(0) {}
	
	DECLARE_VISIT_FUNCS

private:
	int currentIndent;
};

#endif
