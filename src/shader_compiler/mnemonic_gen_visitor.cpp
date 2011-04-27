#include <boost/lexical_cast.hpp>

#include <ASTCreator.h>

#include "mnemonic_gen_visitor.h"
#include "SL_ASTNodes.h"

#include "../shadervm/shader_manager.h"
#include "../shadervm/compiled_shader.h"
#include "../shadervm/symtab.h"

#include "../memory.h"

using namespace std;
using namespace boost;

namespace
{
    // Shader category names
    static const std::wstring SC_SURFACE(L"surface");

    // Type names
    static const std::wstring TN_FLOAT(L"float");
    static const std::wstring TN_VECTOR(L"vector");
    static const std::wstring TN_NORMAL(L"normal");
    static const std::wstring TN_POINT(L"point");
    static const std::wstring TN_COLOR(L"color");

    // Misc. ?
    static const std::wstring MISC_TYPE_CTOR(L"type_ctor");
    

    std::string wstringToString(const std::wstring &ws)
    {
	    return std::string(ws.begin(), ws.end());
    }

    template <typename T>
    T initFromSingleFloat(ASTNode &arg)
    {
        return T(lexical_cast<float>(wstringToString(arg.getImage())));
    }

    template <typename T>
    T initFromTriple(ASTNode &arg1, ASTNode &arg2, ASTNode &arg3)
    {
        return T(lexical_cast<float>(wstringToString(arg1.getImage())),
				 lexical_cast<float>(wstringToString(arg2.getImage())),
				 lexical_cast<float>(wstringToString(arg3.getImage())));
    }

    template <typename T>
    T initFromArgs(const vector<ASTNode*> &args)
    {
		switch(args.size())
		{
		case 1:
			return initFromSingleFloat<T>(*args[0]);

		case 3:
            return initFromTriple<T>(*args[0], *args[1], *args[2]);

		default:
			cout << "Wrong number of arguments given <somewhere>. Default initialization ensue." << endl;
		}
        return T();
    }

    template <typename T>
    T initFromSingleArg(const vector<ASTNode*> &args)
    {
		switch(args.size())
		{
		case 1:
			return initFromSingleFloat<T>(*args[0]);

		default:
			cout << "Wrong number of arguments given <somewhere>. Default initialization ensue." << endl;
		}
        return T();
    }
}

MnemonicGenVisitor::MnemonicGenVisitor(ShaderManager &shaderManager)
	:shader(0), shaderMgr(shaderManager)
{
}

MnemonicGenVisitor::~MnemonicGenVisitor()
{
	memory::destroy(shader);
}

DEFINE_VISIT_EMPTY_NODE(MnemonicGenVisitor, TermNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, FileRootNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, FormalsNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, BlockNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, VarDeclBlockNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, VarDefNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, VarDefMultExprNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, StmtListNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, StmtNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, ExprNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, PrimNode)
DEFINE_VISIT_CHILDREN_NODE(MnemonicGenVisitor, ProcArgsNode)

void MnemonicGenVisitor::visit(ShaderRootNode &node)
{
	memory::destroy(shader);
	shader = 0;

	if (node.getChildren()->at(0)->getImage() == SC_SURFACE)
		shader = memory::construct<CompiledShader>(CompiledShader::ST_Surface);
	else
		return; // Other shaders not yet supported

	shader->setName(wstringToString(node.getChildren()->at(1)->getImage()));

	visitChildrenOf(node);

	shaderMgr.addShader(*shader);
}

void MnemonicGenVisitor::visit(VarDefExprNode &node)
{
	Variable v;
	v.storageType = VST_Varying;

	// Get type
	ASTNode *typeNode = node.getParent()->getParent()->getChildren()->at(0);
	const wstring &type = typeNode->getImage();
	if (type == TN_COLOR)
	{
		v.type = VT_Color;
		v.content = Color(0.f);
	}
	else if(type == TN_FLOAT)
	{
		v.type = VT_Float;
		v.content = 0.f;
	}
	else if(type == TN_VECTOR)
	{
		v.type = VT_Vector;
		v.content = Vector3(0.f);
	}
	else if(type == TN_NORMAL)
	{
		v.type = VT_Normal;
		v.content = Normal(0.f);
	}
	else if(type == TN_POINT)
	{
		v.type = VT_Point;
		v.content = Point3(0.f);
	}
    else
    {
        wcout << L"Unsupported typename: " << type << endl;
        return;
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

                switch(v.type)
                {
                case VT_Color:
					v.content = Color(value);
                    break;

                case VT_Float:
					v.content = value;
                    break;

                case VT_Vector:
					v.content = Vector3(value);
                    break;

                case VT_Normal:
					v.content = Normal(value);
                    break;

                case VT_Point:
					v.content = Point3(value);
                    break;
                }

				initializationHandled = true;
			}
			catch (bad_lexical_cast &)
			{
				// if it's not a float, then it probably is a var, and so initialization is deferred at runtime.
			}
		}
		else if (initializer->getImage() == MISC_TYPE_CTOR)
		{
			vector<ASTNode*> &typeCtorChildren = *initializer->getChildren();
			wstring ctorName = typeCtorChildren[0]->getImage();
			assert(ctorName == type);

			vector<ASTNode*> &args = *typeCtorChildren[1]->getChildren();

			// If arguments are all constants
			initializationHandled = true;
			for (size_t i = 0; i < args.size(); i++)
			{
                // If one of the argument is not a terminal, it is a compound expression and initialization is deferred at runtime
				if (args[i]->getChildren()->size() != 0)
				{
					initializationHandled = false;
				}
				else // Should insert a switch for the type for types not initialized by floats (eg. strings)
				{
					try
					{
                        // We're not sure to be interested by the actual value for now (there may be expression as initializers later on)
						float dummyValue = lexical_cast<float>(wstringToString(args[i]->getImage()));
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
                switch (v.type)
                {
                case VT_Color:
					v.content = initFromArgs<Color>(args);
                    break;

                case VT_Float:
					v.content = initFromSingleArg<float>(args);
                    break;

                case VT_Vector:
					v.content = initFromArgs<Vector3>(args);
                    break;

                case VT_Normal:
					v.content = initFromArgs<Normal>(args);
                    break;

                case VT_Point:
					v.content = initFromArgs<Point3>(args);
                    break;
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

void MnemonicGenVisitor::visit(ProcArgsListNode &node)
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
