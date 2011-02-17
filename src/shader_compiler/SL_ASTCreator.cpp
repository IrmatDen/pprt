#include <string>

#include "SL_ASTCreator.h"
#include "SL_ASTNodes.h"
#include "CRTSL_grammar_constants.h"

using namespace std;

ASTNode* SL_ASTCreator::getASTNode (const Symbol *reduction, ASTNode *parent)
{
	if (!reduction)
		return 0;

	SymbolConstants sym = (SymbolConstants)reduction->symbolIndex;

	deque <Symbol*> rdcChildren;
	if (reduction->type == NON_TERMINAL)
		rdcChildren = ((NonTerminal*) reduction)->children;
	else
	{
		CREATE_NODE(TermNode, term);
		return term;
	}

// DECLARATIONS
	if (sym == SYMBOL_DEFINITIONS)
	{
		CREATE_NODE(FileRootNode, definitions);

		if (rdcChildren.size() != 0)
		{
			definitions->addChild (getASTNode(rdcChildren[0], definitions));	// first shader or function definition
			if ( ((NonTerminal*)rdcChildren[1])->children.size() > 0)
				definitions->addChild (getASTNode(rdcChildren[1], definitions));	// eventual other vars decl
		}

		return definitions;
	}

	if (sym == SYMBOL_SHADER_DEFINITION)
	{
		CREATE_NODE(ShaderRootNode, shader_definition);

		shader_definition->addChild (getASTNode(rdcChildren[0], shader_definition));	// shader type (terminal)
		shader_definition->addChild (getASTNode(rdcChildren[1], shader_definition));	// shader name (terminal)
		shader_definition->addChild (getASTNode(rdcChildren[3], shader_definition));	// args
		shader_definition->addChild (getASTNode(rdcChildren[5], shader_definition));	// body
		return shader_definition;
	}

	if (sym == SYMBOL_FORMALS)
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

	if (sym == SYMBOL_BLOCK)
	{
		CREATE_NODE(BlockNode, block);

		block->addChild (getASTNode(rdcChildren[1], block));	// variables
		block->addChild (getASTNode(rdcChildren[2], block));	// statements
		return block;
	}

	if (sym == SYMBOL_VARIABLES)
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

	if (sym == SYMBOL_VARIABLE_DEFINITIONS)
	{
		CREATE_NODE(VarDefNode, variable_definitions);

		variable_definitions->addChild (getASTNode(rdcChildren[0], variable_definitions));	// type (terminal)
		variable_definitions->addChild (getASTNode(rdcChildren[1], variable_definitions));	// definition(s)
		return variable_definitions;
	}

	if (sym == SYMBOL_DEF_EXPRESSIONS)
	{
		CREATE_NODE(VarDefMultExprNode, def_expressions);
			
		def_expressions->addChild (getASTNode(rdcChildren[0], def_expressions));	// at least one var decl is mandatory
		
		if (rdcChildren.size() == 3)
			def_expressions->addChild (getASTNode(rdcChildren[2], def_expressions));// others vars as needed

		return def_expressions;
	}

	if (sym == SYMBOL_DEF_EXPRESSION)
	{
		CREATE_NODE(VarDefExprNode, def_expression);
			
		def_expression->addChild (getASTNode(rdcChildren[0], def_expression));	// var name (terminal)
		def_expression->addChild (getASTNode(rdcChildren[1], def_expression));	// eventual definition

		return def_expression;
	}

	if (sym == SYMBOL_DEF_INIT)
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
	if (sym == SYMBOL_STATEMENTS)
	{
		CREATE_NODE(StmtListNode, statements);
		
		if ( ((NonTerminal*)rdcChildren[0])->children.size() > 0)
			statements->addChild (getASTNode(rdcChildren[0], statements));	// optionnal other statements
		statements->addChild (getASTNode(rdcChildren[1], statements));		// statement

		return statements;
	}

	if (sym == SYMBOL_STATEMENT)
	{
		CREATE_NODE(StmtNode, statement);
		statement->addChild (getASTNode(rdcChildren[0], statement));		// statement (omit ';')
		return statement;
	}

	if (sym == SYMBOL_RETURNSTMT)
	{
		CREATE_NODE(ReturnStmtNode, returnStmt);
		returnStmt->addChild (getASTNode(rdcChildren[1], returnStmt));		// return statement (omit 'return' and ';')
		return returnStmt;
	}

// EXPRESSIONS
	if (sym == SYMBOL_EXPRESSION)
	{
		CREATE_NODE(ExprNode, expression);
		expression->addChild (getASTNode(rdcChildren[0], expression));		// statement
		return expression;
	}

	if (sym == SYMBOL_PRIMARY)
	{
		CREATE_NODE(PrimNode, expression);
		if(rdcChildren.size() == 1)
		{
			expression->addChild (getASTNode(rdcChildren[0], expression));		// statement
		}
		else // it is a prioritized expression "( <expr> )"
		{
			expression->addChild (getASTNode(rdcChildren[1], expression));		// statement
		}
		return expression;
	}
	
	if (sym == SYMBOL_ASSIGNEXPRESSION)
	{
		CREATE_NODE(AsgnExprNode, assignexpression);
		
		assignexpression->addChild (getASTNode(rdcChildren[0], assignexpression));		// assigned variable
		assignexpression->addChild (getASTNode(rdcChildren[2], assignexpression));		// expression output to variable

		return assignexpression;
	}
	
	if (sym == SYMBOL_ADD_EXPR)
	{
		CREATE_NODE(AddExprNode, addExpr);
		
		addExpr->addChild (getASTNode(rdcChildren[0], addExpr));		// first operand

		// Check if we're not only a passthrough to a real value
		if (rdcChildren.size() == 3)
			addExpr->addChild (getASTNode(rdcChildren[2], addExpr));		// second operand

		return addExpr;
	}
	
	if (sym == SYMBOL_SUB_EXPR)
	{
		CREATE_NODE(SubExprNode, subExpr);
		
		subExpr->addChild (getASTNode(rdcChildren[0], subExpr));		// first operand

		// Check if we're not only a passthrough to a real value
		if (rdcChildren.size() == 3)
			subExpr->addChild (getASTNode(rdcChildren[2], subExpr));		// second operand

		return subExpr;
	}
	
	if (sym == SYMBOL_MULT_EXPR)
	{
		CREATE_NODE(MultExprNode, multExpr);
		
		multExpr->addChild (getASTNode(rdcChildren[0], multExpr));		// first operand

		// Check if we're not only a passthrough to a real value
		if (rdcChildren.size() == 3)
			multExpr->addChild (getASTNode(rdcChildren[2], multExpr));		// second operand

		return multExpr;
	}
	
	if (sym == SYMBOL_DOT_EXPR)
	{
		CREATE_NODE(DotExprNode, dotExpr);
		
		dotExpr->addChild (getASTNode(rdcChildren[0], dotExpr));		// first operand

		// Check if we're not only a passthrough to a real value
		if (rdcChildren.size() == 3)
			dotExpr->addChild (getASTNode(rdcChildren[2], dotExpr));		// second operand

		return dotExpr;
	}
	
	if (sym == SYMBOL_NEGATE)
	{
		CREATE_NODE(NegateExprNode, negExpr);
		
		negExpr->addChild (getASTNode(rdcChildren[1], negExpr));		// first & only operand

		return negExpr;
	}
	
	if (sym == SYMBOL_TYPE_CTOR)
	{
		CREATE_NODE(TypeCtorNode, type_ctor);
		
		type_ctor->addChild (getASTNode(rdcChildren[0], type_ctor));		// type
		type_ctor->addChild (getASTNode(rdcChildren[2], type_ctor));		// arg(s)

		return type_ctor;
	}
	
	if (sym == SYMBOL_PROCEDURECALL)
	{
		CREATE_NODE(ProcCallNode, procedurecall);
		
		procedurecall->addChild (getASTNode(rdcChildren[0], procedurecall));		// function name
		procedurecall->addChild (getASTNode(rdcChildren[2], procedurecall));		// arg(s)

		return procedurecall;
	}
	
	if (sym == SYMBOL_PROC_ARGUMENTS)
	{
		CREATE_NODE(ProcArgsNode, procedurecall);
		
		procedurecall->addChild (getASTNode(rdcChildren[0], procedurecall));		// argument(s) list

		return procedurecall;
	}
	
	if (sym == SYMBOL_PROC_ARGUMENTS_LIST)
	{
		CREATE_NODE(ProcArgsListNode, proc_arguments);
		
		deque <Symbol*> args = ((NonTerminal*) rdcChildren[0])->children;
		
		switch(args.size())
		{
		case 1:
			proc_arguments->addChild (getASTNode(args[0], proc_arguments));		// only arg
			break;
		case 3:
			proc_arguments->addChild (getASTNode(args[0], proc_arguments));		// first arg
			proc_arguments->addChild (getASTNode(args[2], proc_arguments));		// other arg(s)
			break;
		}

		return proc_arguments;
	}

	return 0;
}