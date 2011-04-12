#include <boost/lexical_cast.hpp>

#include <ASTCreator.h>

#include "mnemonic_gen_visitor.h"
#include "SL_ASTNodes.h"

#include "../shadervm/shader_manager.h"
#include "../shadervm/compiled_shader.h"
#include "../shadervm/symtab.h"

#include "../common.h"

using namespace std;
using namespace boost;

std::string wstringToString(const std::wstring &ws)
{
	return std::string(ws.begin(), ws.end());
}

MnemonicGenVisitor::MnemonicGenVisitor(ShaderManager &shaderManager)
	:shader(0), shaderMgr(shaderManager)
{
}

MnemonicGenVisitor::~MnemonicGenVisitor()
{
	memory::destroy(shader);
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
	memory::destroy(shader);
	shader = 0;

	if (node.getChildren()->at(0)->getImage() == L"surface")
		shader = memory::construct<CompiledShader>(CompiledShader::ST_Surface);
	else
		return; // Other shaders not yet supported

	shader->setName(wstringToString(node.getChildren()->at(1)->getImage()));

	visitChildrenOf(node);

	shaderMgr.addShader(*shader);
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
	if (type == L"color")
	{
		v.type = VT_Color;
		v.content = Color(0.f);
	}
	else if(type == L"real")
	{
		v.type = VT_Float;
		v.content = 0.f;
	}
	else if(type == L"vec3")
	{
		v.type = VT_Vector;
		v.content = Vector3(0.f);
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
				float value = lexical_cast<float>(wstringToString(initializer->getImage()));
				if (type == L"color")
					v.content = Color((float)value);
				else if (type == L"real")
					v.content = value;

				initializationHandled = true;
			}
			catch (bad_lexical_cast &)
			{
				// if it's not a float, then it probably is a var, and so initialization is deferred at runtime.
			}
		}
		else if (initializer->getImage() == L"type_ctor")
		{
			vector<ASTNode*> &typeCtorChildren = *initializer->getChildren();
			wstring ctorName = typeCtorChildren[0]->getImage();
#ifdef _DEBUG
			assert(ctorName == type);
#endif

			vector<ASTNode*> &args = *typeCtorChildren[1]->getChildren();

			// If arguments are all constants
			initializationHandled = true;
			for (size_t i = 0; i < args.size(); i++)
			{
				if (args[i]->getChildren()->size() != 0)
				{
					initializationHandled = false;
				}
				else if (type == L"color")
				{
					try
					{
						float value = lexical_cast<float>(wstringToString(args[i]->getImage()));
					}
					catch (bad_lexical_cast &)
					{
						// if it's not a float, then it probably is a var, and so initialization is deferred at runtime.
						initializationHandled = false;
					}
				}
			}

			// Checks on constantnes is done, do the proper initialization.
			if (initializationHandled)
			{
				if (type == L"color")
				{
					switch(args.size())
					{
					case 1:
						v.content = Color((float)lexical_cast<float>(wstringToString(args[0]->getImage())));
						break;

					case 3:
						{
							Color c((float)lexical_cast<float>(wstringToString(args[0]->getImage())),
									(float)lexical_cast<float>(wstringToString(args[1]->getImage())),
									(float)lexical_cast<float>(wstringToString(args[2]->getImage())));
							v.content = c;
							break;
						}

					case 4:
						{
							Color c((float)lexical_cast<float>(wstringToString(args[0]->getImage())),
									(float)lexical_cast<float>(wstringToString(args[1]->getImage())),
									(float)lexical_cast<float>(wstringToString(args[2]->getImage())));
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
#ifdef _DEBUG
					assert(args.size() == 1);
#endif
					v.content = lexical_cast<float>(wstringToString(args[0]->getImage()));
				}
			}
		}

		shader->addVar(v);

		if (!initializationHandled)
		{
			// Defer the initialization to runtime
			((SLNode*)initializer)->accept(*this);
			string instr("pop ");
			instr += wstringToString(node.getChildren()->at(0)->getImage());
			shader->parseInstr(instr);
		}
	}
}

void MnemonicGenVisitor::visit(VarInitNode &node)
{
	vector<ASTNode*> &children = *node.getChildren();

	if (children.size() > 1)
		visitChildrenOf(node);
	else
	{
		ASTNode &op = *node.getChildren()->at(0);
		if (op.getChildren()->size() == 0)
		{
			string instr("push ");
			instr += wstringToString(op.getImage());
			shader->parseInstr(instr);
		}
		else
		{
			((SLNode*)&op)->accept(*this);
		}
	}
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

void MnemonicGenVisitor::visit(MultExprNode &node)
{
	// Push the second operand
	ASTNode &op2 = *node.getChildren()->at(1);
	if (op2.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op2.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op2)->accept(*this);
	}
	
	// Push the first operand
	ASTNode &op1 = *node.getChildren()->at(0);
	if (op1.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op1.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op1)->accept(*this);
	}

	string instr("mult");
	shader->parseInstr(instr);
}

void MnemonicGenVisitor::visit(DotExprNode &node)
{
	// Push the second operand
	ASTNode &op2 = *node.getChildren()->at(1);
	if (op2.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op2.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op2)->accept(*this);
	}
	
	// Push the first operand
	ASTNode &op1 = *node.getChildren()->at(0);
	if (op1.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op1.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op1)->accept(*this);
	}

	string instr("dot");
	shader->parseInstr(instr);
}

void MnemonicGenVisitor::visit(NegateExprNode &node)
{
	// Push the only operand
	ASTNode &op = *node.getChildren()->at(0);
	if (op.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op)->accept(*this);
	}

	string instr("negate");
	shader->parseInstr(instr);
}

void MnemonicGenVisitor::visit(AddExprNode &node)
{
	// Push the second operand
	ASTNode &op2 = *node.getChildren()->at(1);
	if (op2.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op2.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op2)->accept(*this);
	}
	
	// Push the first operand
	ASTNode &op1 = *node.getChildren()->at(0);
	if (op1.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op1.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op1)->accept(*this);
	}

	string instr("add");
	shader->parseInstr(instr);
}

void MnemonicGenVisitor::visit(SubExprNode &node)
{
	// Push the second operand
	ASTNode &op2 = *node.getChildren()->at(1);
	if (op2.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op2.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op2)->accept(*this);
	}
	
	// Push the first operand
	ASTNode &op1 = *node.getChildren()->at(0);
	if (op1.getChildren()->size() == 0)
	{
		string instr("push ");
		instr += wstringToString(op1.getImage());
		shader->parseInstr(instr);
	}
	else
	{
		((SLNode*)&op1)->accept(*this);
	}

	string instr("sub");
	shader->parseInstr(instr);
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
	visitChildrenOf(node);
}

void MnemonicGenVisitor::visit(ProcArgsListNode &node)
{
	vector<ASTNode*> &children = *node.getChildren();
	for(vector<ASTNode*>::reverse_iterator it = children.rbegin(); it != children.rend(); ++it)
	{
		ASTNode *c = *it;
		if (c->getChildren()->size() == 0)
		{
			/*// initializer is a terminal, check if it's a constant, if so, use it to initialize the var
			try
			{
				float value = lexical_cast<float>(wstringToString(initializer->getImage()));
				if (type == L"color")
					v.content = Color((float)value);
				else if (type == L"real")
					v.content = value;

				initializationHandled = true;
			}
			catch (bad_lexical_cast &)
			{
				// if it's not a float, then it probably is a var, and so initialization is deferred at runtime.
			}*/
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
