#include <boost/lexical_cast.hpp>

#include <ASTCreator.h>

#include "mnemonic_gen_visitor.h"
#include "SL_ASTNodes.h"

#include "../shadervm/symtab.h"

using namespace std;
using namespace boost;

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
	delete shader;
	shader = 0;

	if (node.getChildren()->at(0)->getImage() == L"surface")
		shader = new CompiledShader(CompiledShader::ST_Surface);

	shader->setName(wstringToString(node.getChildren()->at(1)->getImage()));

	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(FormalsNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(BlockNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(VarDeclBlockNode &node)
{
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
	Variable v;
	v.storageType = VST_Varying;

	// Get type
	ASTNode *typeNode = node.getParent()->getParent()->getChildren()->at(0);
	const wstring &type = typeNode->getImage();
	if (type == L"color4")
	{
		v.type = VT_Color;
		v.content = Color4();
	}
	else if(type == L"real")
	{
		v.type = VT_Double;
		v.content = 0;
	}
	else if(type == L"vec3")
	{
		v.type = VT_Vector;
		v.content = Vec3();
	}

	// Get var name
	v.name = wstringToString(node.getChildren()->at(0)->getImage());

	// If initializer is provided, defer it as a statement.
	if (node.getChildren()->size() == 2)
	{
		ASTNode *initializer = node.getChildren()->at(1);
		bool initializationHandled = false;

		if (initializer->getChildren()->size() == 0)
		{
			// initializer is a terminal, check if it's a constant, if so, use it to initialize the var
			try
			{
				double value = lexical_cast<double>(wstringToString(initializer->getImage()));
				if (type == L"color4")
					v.content = Color4((float)value);
				else if (type == L"real")
					v.content = value;

				initializationHandled = true;
			}
			catch (bad_lexical_cast &)
			{
				// if it's not a double, then it probably is a var, and so initialization is deferred at runtime.
			}
		}
		else if (initializer->getImage() == L"type_ctor")
		{
			vector<ASTNode*> &typeCtorChildren = *initializer->getChildren();
			wstring ctorName = typeCtorChildren[0]->getImage();
			assert(ctorName == type);

			vector<ASTNode*> &args = *typeCtorChildren[1]->getChildren();

			// If arguments are all constants
			initializationHandled = true;
			for (size_t i = 0; i < args.size(); i++)
			{
				if (args[i]->getChildren()->size() != 0)
				{
					initializationHandled = false;
				}
				else if (type == L"color4")
				{
					try
					{
						double value = lexical_cast<double>(wstringToString(args[i]->getImage()));
					}
					catch (bad_lexical_cast &)
					{
						// if it's not a double, then it probably is a var, and so initialization is deferred at runtime.
						initializationHandled = false;
					}
				}
			}

			// Checks on constantnes is done, do the proper initialization.
			if (initializationHandled)
			{
				if (type == L"color4")
				{
					switch(args.size())
					{
					case 1:
						v.content = Color4((float)lexical_cast<double>(wstringToString(args[0]->getImage())));
						break;

					case 3:
						{
							Color4 c;
							c.r = (float)lexical_cast<double>(wstringToString(args[0]->getImage()));
							c.g = (float)lexical_cast<double>(wstringToString(args[1]->getImage()));
							c.b = (float)lexical_cast<double>(wstringToString(args[2]->getImage()));
							v.content = c;
							break;
						}

					case 4:
						{
							Color4 c;
							c.r = (float)lexical_cast<double>(wstringToString(args[0]->getImage()));
							c.g = (float)lexical_cast<double>(wstringToString(args[1]->getImage()));
							c.b = (float)lexical_cast<double>(wstringToString(args[2]->getImage()));
							c.a = (float)lexical_cast<double>(wstringToString(args[3]->getImage()));
							v.content = c;
							break;
						}

					default:
						// Nothing atm.
						break;
					}
				}
				else if (type == L"real")
				{
					assert(args.size() == 1);
					v.content = lexical_cast<double>(wstringToString(args[0]->getImage()));
				}
			}
		}

		if (!initializationHandled)
		{
			// Defer the initialization to runtime
			((SLNode*)initializer)->accept(*this);
			string instr("pop ");
			instr += wstringToString(node.getChildren()->at(0)->getImage());
			shader->parseInstr(instr);
		}
	}

	shader->addVar(v);
}

void MnemonicGenVisitor::visit(VarInitNode &node)
{
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(StmtListNode &node)
{
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
		string instr("push ");
		instr += wstringToString(retExpr.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&retExpr)->accept(*this);
	}

	shader->parseInstr("ret");
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
		string instr("push ");
		instr += wstringToString(expr.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&expr)->accept(*this);
	}

	string instr("pop ");
	instr += wstringToString(node.getChildren()->at(0)->getImage());
	shader->parseInstr(instr);
}

void MnemonicGenVisitor::visit(TypeCtorNode &node)
{
	// Evaluate arguments, if any
	vector<ASTNode*> &children = *node.getChildren();
	if (children.size() == 2)
	{
		((SLNode*)children[1])->accept(*this);
	}

	string instr("call ");
	instr += wstringToString(children[0]->getImage());
	shader->parseInstr(instr);
}

void MnemonicGenVisitor::visit(ProcCallNode &node)
{
	// Evaluate arguments, if any
	vector<ASTNode*> &children = *node.getChildren();
	if (children.size() == 2)
	{
		((SLNode*)children[1])->accept(*this);
	}

	string instr("call ");
	instr += wstringToString(children[0]->getImage());
	shader->parseInstr(instr);
}

void MnemonicGenVisitor::visit(ProcArgsNode &node)
{
	vector<ASTNode*> &children = *node.getChildren();
	for(vector<ASTNode*>::reverse_iterator it = children.rbegin(); it != children.rend(); ++it)
	{
		ASTNode *c = *it;
		if (c->getChildren()->size() == 0)
		{
			string instr("push ");
			instr += wstringToString(c->getImage());
			shader->parseInstr(instr);
		}
		else
		{
			((SLNode*)c)->accept(*this);
		}
	}
}
