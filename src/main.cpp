#include <windows.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <CGTFile.h>
#include <ASTCreator.h>

#include "scene/scene.h"

using namespace std;

class CRTSL_ASTNode : public ASTNode
{
public:
	enum Type
	{
		Terminal,
		FileRoot,
		ShaderRoot,
		Block,
		VarDeclBlock,
		VarDef,
		VarDefMultExpr,
		VarDefExpr,
		VarInit,
		StmtList,
		Stmt,
		ReturnStmt,
		Expr,
		Prim,
		AsgnExpr,
		TypeCtor,
		ProcCall,
		ProcArgs
	};

public:
	CRTSL_ASTNode(Type t) : type(t) {}

	Type getType() const { return type; }

protected:
	Type type;
};

#define CREATE_CRTSL_NODE(X,x,t) X *x = new X(t); x->init(*reduction); x->setParent(parent);

class MyASTCreator : public ASTCreator
{
protected:
	virtual ASTNode* getASTNode (const Symbol *reduction, ASTNode *parent)
	{
		wstring sym = reduction->symbol;

		deque <Symbol*> rdcChildren;
		if (reduction->type == NON_TERMINAL)
			rdcChildren = ((NonTerminal*) reduction)->children;
		else
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, term, CRTSL_ASTNode::Terminal);
			return term;
		}

	// DECLARATIONS
		if (sym == L"definitions")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, definitions, CRTSL_ASTNode::FileRoot);

			definitions->addChild(getASTNode(rdcChildren[0], definitions));
			return definitions;
		}

		if (sym == L"shader_definition")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, shader_definition, CRTSL_ASTNode::ShaderRoot);

			shader_definition->addChild (getASTNode(rdcChildren[0], shader_definition));	// shader type (terminal)
			shader_definition->addChild (getASTNode(rdcChildren[1], shader_definition));	// shader name (terminal)
			shader_definition->addChild (getASTNode(rdcChildren[3], shader_definition));	// args
			shader_definition->addChild (getASTNode(rdcChildren[5], shader_definition));	// body
			return shader_definition;
		}

		if (sym == L"block")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, block, CRTSL_ASTNode::Block);

			block->addChild (getASTNode(rdcChildren[1], block));	// variables
			block->addChild (getASTNode(rdcChildren[2], block));	// statements
			return block;
		}

		if (sym == L"variables")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, variables, CRTSL_ASTNode::VarDeclBlock);

			if (rdcChildren.size() != 0)
			{
				variables->addChild (getASTNode(rdcChildren[0], variables));	// first var (or multiple vars of the same type)
				if ( ((NonTerminal*)rdcChildren[1])->children.size() > 0)
					variables->addChild (getASTNode(rdcChildren[1], variables));	// eventual other vars decl
			}
			return variables;
		}

		if (sym == L"variable_definitions")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, variable_definitions, CRTSL_ASTNode::VarDef);

			variable_definitions->addChild (getASTNode(rdcChildren[0], variable_definitions));	// type (terminal)
			variable_definitions->addChild (getASTNode(rdcChildren[1], variable_definitions));	// definition(s)
			return variable_definitions;
		}

		if (sym == L"def_expressions")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, def_expressions, CRTSL_ASTNode::VarDefMultExpr);
				
			def_expressions->addChild (getASTNode(rdcChildren[0], def_expressions));	// at least one var decl is mandatory
			
			if (rdcChildren.size() == 2)
				def_expressions->addChild (getASTNode(rdcChildren[1], def_expressions));// others vars as needed

			return def_expressions;
		}

		if (sym == L"def_expression")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, def_expression, CRTSL_ASTNode::VarDefExpr);
				
			def_expression->addChild (getASTNode(rdcChildren[0], def_expression));	// var name (terminal)
			def_expression->addChild (getASTNode(rdcChildren[1], def_expression));	// eventual definition

			return def_expression;
		}

		if (sym == L"def_init")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, def_init, CRTSL_ASTNode::VarInit);
			
			def_init->addChild (getASTNode(rdcChildren[1], def_init));	// var initialization expression

			return def_init;
		}

	// STATEMENTS
		if (sym == L"statements")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, statements, CRTSL_ASTNode::StmtList);
			
			if ( ((NonTerminal*)rdcChildren[0])->children.size() > 0)
				statements->addChild (getASTNode(rdcChildren[0], statements));	// optionnal other statements
			statements->addChild (getASTNode(rdcChildren[1], statements));		// statement

			return statements;
		}

		if (sym == L"statement")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, statement, CRTSL_ASTNode::Stmt);
			
			switch(rdcChildren.size())
			{
			case 2:
				statement->addChild (getASTNode(rdcChildren[0], statement));		// statement (omit ';')
				break;
			case 3:
				if (rdcChildren[0]->symbol == L"return")
				{
					CREATE_CRTSL_NODE(CRTSL_ASTNode, returnStmt, CRTSL_ASTNode::ReturnStmt);
					returnStmt->addChild (getASTNode(rdcChildren[1], returnStmt));		// return statement (omit 'return' and ';')
					statement->addChild(returnStmt);
				}
				break;
			}

			return statement;
		}

	// EXPRESSIONS
		if (sym == L"expression")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, expression, CRTSL_ASTNode::Expr);
			
			expression->addChild (getASTNode(rdcChildren[0], expression));		// statement

			return expression;
		}
		
		if (sym == L"primary")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, primary, CRTSL_ASTNode::Prim);
			
			primary->addChild (getASTNode(rdcChildren[0], primary));		// primary expression

			return primary;
		}
		
		if (sym == L"assignexpression")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, assignexpression, CRTSL_ASTNode::AsgnExpr);
			
			assignexpression->addChild (getASTNode(rdcChildren[0], assignexpression));		// assigned variable
			assignexpression->addChild (getASTNode(rdcChildren[2], assignexpression));		// expression output to variable

			return assignexpression;
		}
		
		if (sym == L"type_ctor")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, type_ctor, CRTSL_ASTNode::TypeCtor);
			
			type_ctor->addChild (getASTNode(rdcChildren[0], type_ctor));		// type
			type_ctor->addChild (getASTNode(rdcChildren[2], type_ctor));		// arg(s)

			return type_ctor;
		}
		
		if (sym == L"procedurecall")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, procedurecall, CRTSL_ASTNode::ProcCall);
			
			procedurecall->addChild (getASTNode(rdcChildren[0], procedurecall));		// function name
			procedurecall->addChild (getASTNode(rdcChildren[2], procedurecall));		// arg(s)

			return procedurecall;
		}
		
		if (sym == L"proc_arguments")
		{
			CREATE_CRTSL_NODE(CRTSL_ASTNode, proc_arguments, CRTSL_ASTNode::ProcArgs);
			
			switch(rdcChildren.size())
			{
			case 1:
				proc_arguments->addChild (getASTNode(rdcChildren[0], proc_arguments));		// only arg
				break;
			case 3:
				proc_arguments->addChild (getASTNode(rdcChildren[0], proc_arguments));		// first arg
				proc_arguments->addChild (getASTNode(rdcChildren[2], proc_arguments));		// other arg(s)
				break;
			}

			return proc_arguments;
		}

		return 0;
	}
};

void printCRTSLAST(ASTNode *node, int indent = 0)
{
	if (!node)
		return;

	for (int i=0; i < indent; i++)
	{
		cout << " ";
	}

	CRTSL_ASTNode *me = (CRTSL_ASTNode*)node;

	switch(me->getType())
	{
	case CRTSL_ASTNode::Terminal:
		cout << "Terminal";
		break;
	case CRTSL_ASTNode::FileRoot:
		cout << "FileRoot";
		break;
	case CRTSL_ASTNode::ShaderRoot:
		cout << "ShaderRoot";
		break;
	case CRTSL_ASTNode::Block:
		cout << "Block";
		break;
	case CRTSL_ASTNode::VarDeclBlock:
		cout << "VarDeclBlock";
		break;
	case CRTSL_ASTNode::VarDef:
		cout << "VarDef";
		break;
	case CRTSL_ASTNode::VarDefMultExpr:
		cout << "VarDefMultExpr";
		break;
	case CRTSL_ASTNode::VarDefExpr:
		cout << "VarDefExpr";
		break;
	case CRTSL_ASTNode::VarInit:
		cout << "VarInit";
		break;
	case CRTSL_ASTNode::StmtList:
		cout << "StmtList";
		break;
	case CRTSL_ASTNode::Stmt:
		cout << "Stmt";
		break;
	case CRTSL_ASTNode::ReturnStmt:
		cout << "ReturnStmt";
		break;
	case CRTSL_ASTNode::Expr:
		cout << "Expr";
		break;
	case CRTSL_ASTNode::Prim:
		cout << "Prim";
		break;
	case CRTSL_ASTNode::AsgnExpr:
		cout << "AsgnExpr";
		break;
	case CRTSL_ASTNode::TypeCtor:
		cout << "TypeCtor";
		break;
	case CRTSL_ASTNode::ProcCall:
		cout << "ProcCall";
		break;
	case CRTSL_ASTNode::ProcArgs:
		cout << "ProcArgs";
		break;
	};

	cout << endl;

	if (me->getType() != CRTSL_ASTNode::Terminal)
	{
		for (size_t i = 0; i < node->getChildren()->size(); i++)
		{
			vector <ASTNode*> *children = node->getChildren();
			printCRTSLAST((*children)[i], indent + 2);
		}
	}
}

int main(int argc, char **argv)
{
	CGTFile f;
	bool cgtLoaded = f.load("./src/shadervm/crtsl.cgt");

	ifstream fileIn("./Shaders/sample.crtsl");
	fileIn.seekg(0, ios::end);
	int size = fileIn.tellg();
	fileIn.seekg(0, ios::beg);
	char *buffer = new char[size+1];
	memset(buffer, 0, size+1);
	fileIn.read(buffer, size);

	DFA *scanner = f.getScanner();
	bool scanSuccess = scanner->scan(buffer);
	vector<Token*>& tokens = scanner->getTokens();

	LALR *parser = f.getParser();
	parser->init(tokens);
	Symbol *sym = parser->parse(tokens, false, false);
	//parser->printReductionTree(sym, 10);

	MyASTCreator astCreator;
	ASTNode *root = astCreator.createTree(*sym);

	printCRTSLAST(root);
	
	delete buffer;
	return 0;

	/*string scnFile = "ScnSamples/specular_shaded_sphere.crtscn";

	Scene scn;
	bool scnLoaded = scn.loadScnFile(scnFile);
	if (scnLoaded)
	{
		cout << endl << "Parsed \"" << scnFile << "\" successfully!" << endl;
		cout << "Start rendering..." << endl;
		__int64 begin = timeGetTime();
		scn.render();
		__int64 end = timeGetTime();
		cout << "Finished rendering, time: " << end - begin << " ms" << endl;
	}
	else
		cout << "Loading " << scnFile << " failed!" << endl;
	
	cout << endl << "Press return to quit..." << endl;
	cin.get();

	return !scnLoaded;*/
}