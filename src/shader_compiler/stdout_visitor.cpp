#include <iostream>
#include <algorithm>

#include <ASTCreator.h>

#include "stdout_visitor.h"
#include "SL_ASTNodes.h"

using namespace std;

static char* space() { static char tree[] = "|   "; return tree; }

void StdoutVisitor::visit(TermNode &node)
{
	generate_n(ostream_iterator<char*>(cout), currentIndent - 1, space);
	cout << "|-  ";
	wcout << node.getImage() << endl;
}

#define GEN_STDOUTVISITOR_METHOD(isRoot, NodeType)	\
	void StdoutVisitor::visit(NodeType &node) \
	{ \
		generate_n(ostream_iterator<char*>(cout), currentIndent - 1, space); \
		if (!isRoot)	cout << "|-  "; \
		if (&node == 0) \
		{ \
			cout << "<null>" << endl; \
			return; \
		} \
		wcout << node.getImage() << endl; \
		 \
		currentIndent++; \
		visitChildrenOf(node); \
		currentIndent--; \
	}

GEN_STDOUTVISITOR_METHOD(true, FileRootNode)
GEN_STDOUTVISITOR_METHOD(false, ShaderRootNode)
GEN_STDOUTVISITOR_METHOD(false, FormalsNode)
GEN_STDOUTVISITOR_METHOD(false, BlockNode)
GEN_STDOUTVISITOR_METHOD(false, VarDeclBlockNode)
GEN_STDOUTVISITOR_METHOD(false, VarDefNode)
GEN_STDOUTVISITOR_METHOD(false, VarDefMultExprNode)
GEN_STDOUTVISITOR_METHOD(false, VarDefExprNode)
GEN_STDOUTVISITOR_METHOD(false, VarInitNode)
GEN_STDOUTVISITOR_METHOD(false, StmtListNode)
GEN_STDOUTVISITOR_METHOD(false, StmtNode)
GEN_STDOUTVISITOR_METHOD(false, ReturnStmtNode)
GEN_STDOUTVISITOR_METHOD(false, ExprNode)
GEN_STDOUTVISITOR_METHOD(false, PrimNode)
GEN_STDOUTVISITOR_METHOD(false, AsgnExprNode)
GEN_STDOUTVISITOR_METHOD(false, MultExprNode)
GEN_STDOUTVISITOR_METHOD(false, DotExprNode)
GEN_STDOUTVISITOR_METHOD(false, NegateExprNode)
GEN_STDOUTVISITOR_METHOD(false, AddExprNode)
GEN_STDOUTVISITOR_METHOD(false, SubExprNode)
GEN_STDOUTVISITOR_METHOD(false, TypeCtorNode)
GEN_STDOUTVISITOR_METHOD(false, ProcCallNode)
GEN_STDOUTVISITOR_METHOD(false, ProcArgsNode)
