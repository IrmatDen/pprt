#include <string>

#include "SL_ASTCreator.h"
#include "SL_ASTNodes.h"

using namespace std;

ASTNode* SL_ASTCreator::getASTNode (const Symbol *reduction, ASTNode *parent)
{
	if (!reduction)
		return 0;

	wstring sym = reduction->symbol;

	deque <Symbol*> rdcChildren;
	if (reduction->type == NON_TERMINAL)
		rdcChildren = ((NonTerminal*) reduction)->children;
	else
	{
		CREATE_NODE(TermNode, term);
		return term;
	}

// DECLARATIONS
	if (sym == L"definitions")
	{
		CREATE_NODE(FileRootNode, definitions);

		definitions->addChild(getASTNode(rdcChildren[0], definitions));
		return definitions;
	}

	if (sym == L"shader_definition")
	{
		CREATE_NODE(ShaderRootNode, shader_definition);

		shader_definition->addChild (getASTNode(rdcChildren[0], shader_definition));	// shader type (terminal)
		shader_definition->addChild (getASTNode(rdcChildren[1], shader_definition));	// shader name (terminal)
		shader_definition->addChild (getASTNode(rdcChildren[3], shader_definition));	// args
		shader_definition->addChild (getASTNode(rdcChildren[5], shader_definition));	// body
		return shader_definition;
	}

	if (sym == L"formals")
	{
		CREATE_NODE(FormalsNode, formals);

		if (rdcChildren.size() != 0)
		{
			formals->addChild (getASTNode(rdcChildren[0], formals));	// first var (or multiple vars of the same type)
			if ( ((NonTerminal*)rdcChildren[1])->children.size() > 0)
				formals->addChild (getASTNode(rdcChildren[1], formals));	// eventual other vars decl
		}
		return formals;
	}

	if (sym == L"block")
	{
		CREATE_NODE(BlockNode, block);

		block->addChild (getASTNode(rdcChildren[1], block));	// variables
		block->addChild (getASTNode(rdcChildren[2], block));	// statements
		return block;
	}

	if (sym == L"variables")
	{
		CREATE_NODE(VarDeclBlockNode, variables);

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
		CREATE_NODE(VarDefNode, variable_definitions);

		variable_definitions->addChild (getASTNode(rdcChildren[0], variable_definitions));	// type (terminal)
		variable_definitions->addChild (getASTNode(rdcChildren[1], variable_definitions));	// definition(s)
		return variable_definitions;
	}

	if (sym == L"def_expressions")
	{
		CREATE_NODE(VarDefMultExprNode, def_expressions);
			
		def_expressions->addChild (getASTNode(rdcChildren[0], def_expressions));	// at least one var decl is mandatory
		
		if (rdcChildren.size() == 3)
			def_expressions->addChild (getASTNode(rdcChildren[2], def_expressions));// others vars as needed

		return def_expressions;
	}

	if (sym == L"def_expression")
	{
		CREATE_NODE(VarDefExprNode, def_expression);
			
		def_expression->addChild (getASTNode(rdcChildren[0], def_expression));	// var name (terminal)
		def_expression->addChild (getASTNode(rdcChildren[1], def_expression));	// eventual definition

		return def_expression;
	}

	if (sym == L"def_init")
	{
		// def_init is optionnal, so check first!
		if(rdcChildren.size() > 0)
		{
			CREATE_NODE(VarInitNode, def_init);
			def_init->addChild (getASTNode(rdcChildren[1], def_init));	// var initialization expression
			return def_init;
		}
		return 0;
	}

// STATEMENTS
	if (sym == L"statements")
	{
		CREATE_NODE(StmtListNode, statements);
		
		if ( ((NonTerminal*)rdcChildren[0])->children.size() > 0)
			statements->addChild (getASTNode(rdcChildren[0], statements));	// optionnal other statements
		statements->addChild (getASTNode(rdcChildren[1], statements));		// statement

		return statements;
	}

	if (sym == L"statement")
	{
		CREATE_NODE(StmtNode, statement);
		statement->addChild (getASTNode(rdcChildren[0], statement));		// statement (omit ';')
		return statement;
	}

	if (sym == L"returnstmt")
	{
		CREATE_NODE(ReturnStmtNode, returnStmt);
		returnStmt->addChild (getASTNode(rdcChildren[1], returnStmt));		// return statement (omit 'return' and ';')
		return returnStmt;
	}

// EXPRESSIONS
	if (sym == L"expression")
	{
		CREATE_NODE(ExprNode, expression);
		expression->addChild (getASTNode(rdcChildren[0], expression));		// statement
		return expression;
	}
	
	if (sym == L"primary")
	{
		CREATE_NODE(PrimNode, primary);
		primary->addChild (getASTNode(rdcChildren[0], primary));		// primary expression
		return primary;
	}
	
	if (sym == L"assignexpression")
	{
		CREATE_NODE(AsgnExprNode, assignexpression);
		
		assignexpression->addChild (getASTNode(rdcChildren[0], assignexpression));		// assigned variable
		assignexpression->addChild (getASTNode(rdcChildren[2], assignexpression));		// expression output to variable

		return assignexpression;
	}
	
	if (sym == L"type_ctor")
	{
		CREATE_NODE(TypeCtorNode, type_ctor);
		
		type_ctor->addChild (getASTNode(rdcChildren[0], type_ctor));		// type
		type_ctor->addChild (getASTNode(rdcChildren[2], type_ctor));		// arg(s)

		return type_ctor;
	}
	
	if (sym == L"procedurecall")
	{
		CREATE_NODE(ProcCallNode, procedurecall);
		
		procedurecall->addChild (getASTNode(rdcChildren[0], procedurecall));		// function name
		procedurecall->addChild (getASTNode(rdcChildren[2], procedurecall));		// arg(s)

		return procedurecall;
	}
	
	if (sym == L"proc_arguments")
	{
		CREATE_NODE(ProcArgsNode, proc_arguments);
		
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