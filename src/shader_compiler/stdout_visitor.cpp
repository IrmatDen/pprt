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

#define GEN_STDOUTVISITOR_METHOD(NodeType)	\
	void StdoutVisitor::visit(NodeType &node) \
	{ \
		generate_n(ostream_iterator<char*>(cout), currentIndent - 1, space); \
		cout << "|-  "; \
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

GEN_STDOUTVISITOR_METHOD(FileRootNode)
GEN_STDOUTVISITOR_METHOD(ShaderRootNode)
GEN_STDOUTVISITOR_METHOD(BlockNode)
GEN_STDOUTVISITOR_METHOD(VarDeclBlockNode)
GEN_STDOUTVISITOR_METHOD(VarDefNode)
GEN_STDOUTVISITOR_METHOD(VarDefMultExprNode)
GEN_STDOUTVISITOR_METHOD(VarDefExprNode)
GEN_STDOUTVISITOR_METHOD(VarInitNode)
GEN_STDOUTVISITOR_METHOD(StmtListNode)
GEN_STDOUTVISITOR_METHOD(StmtNode)
GEN_STDOUTVISITOR_METHOD(ReturnStmtNode)
GEN_STDOUTVISITOR_METHOD(ExprNode)
GEN_STDOUTVISITOR_METHOD(PrimNode)
GEN_STDOUTVISITOR_METHOD(AsgnExprNode)
GEN_STDOUTVISITOR_METHOD(TypeCtorNode)
GEN_STDOUTVISITOR_METHOD(ProcCallNode)
GEN_STDOUTVISITOR_METHOD(ProcArgsNode)