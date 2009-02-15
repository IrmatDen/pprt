#ifndef CRT_GRAMMAR_CONSTANTS_H
#define CRT_GRAMMAR_CONSTANTS_H

enum SymbolConstants
{
   SYMBOL_EOF                  =  0, // (EOF)
   SYMBOL_ERROR                =  1, // (Error)
   SYMBOL_WHITESPACE           =  2, // (Whitespace)
   SYMBOL_COMMENTLINE          =  3, // (Comment Line)
   SYMBOL_LPARAN               =  4, // '('
   SYMBOL_RPARAN               =  5, // ')'
   SYMBOL_TIMES                =  6, // '*'
   SYMBOL_COMMA                =  7, // ','
   SYMBOL_SEMI                 =  8, // ';'
   SYMBOL_LBRACE               =  9, // '{'
   SYMBOL_RBRACE               = 10, // '}'
   SYMBOL_EQ                   = 11, // '='
   SYMBOL_IDENTIFIER           = 12, // identifier
   SYMBOL_NUMBER               = 13, // number
   SYMBOL_RETURN               = 14, // return
   SYMBOL_SHADER_TYPE          = 15, // 'shader_type'
   SYMBOL_STRINGCONSTANT       = 16, // stringconstant
   SYMBOL_TYPE                 = 17, // type
   SYMBOL_ASSIGNEXPRESSION     = 18, // <assignexpression>
   SYMBOL_BLOCK                = 19, // <block>
   SYMBOL_DEF_EXPRESSION       = 20, // <def_expression>
   SYMBOL_DEF_EXPRESSIONS      = 21, // <def_expressions>
   SYMBOL_DEF_INIT             = 22, // <def_init>
   SYMBOL_DEFINITIONS          = 23, // <definitions>
   SYMBOL_EXPRESSION           = 24, // <expression>
   SYMBOL_FORMALS              = 25, // <formals>
   SYMBOL_MULT_EXPR            = 26, // <mult_expr>
   SYMBOL_PRIMARY              = 27, // <primary>
   SYMBOL_PROC_ARGUMENTS       = 28, // <proc_arguments>
   SYMBOL_PROCEDURECALL        = 29, // <procedurecall>
   SYMBOL_RETURNSTMT           = 30, // <returnstmt>
   SYMBOL_SHADER_DEFINITION    = 31, // <shader_definition>
   SYMBOL_STATEMENT            = 32, // <statement>
   SYMBOL_STATEMENTS           = 33, // <statements>
   SYMBOL_TYPE_CTOR            = 34, // <type_ctor>
   SYMBOL_VARIABLE_DEFINITIONS = 35, // <variable_definitions>
   SYMBOL_VARIABLES            = 36  // <variables>
};

enum RuleConstants
{
   RULE_DEFINITIONS                                            =  0, // <definitions> ::= <shader_definition>
   RULE_SHADER_DEFINITION_SHADER_TYPE_IDENTIFIER_LPARAN_RPARAN =  1, // <shader_definition> ::= 'shader_type' identifier '(' <formals> ')' <block>
   RULE_FORMALS                                                =  2, // <formals> ::= <variable_definitions> <formals>
   RULE_FORMALS2                                               =  3, // <formals> ::= 
   RULE_VARIABLES                                              =  4, // <variables> ::= <variable_definitions> <variables>
   RULE_VARIABLES2                                             =  5, // <variables> ::= 
   RULE_VARIABLE_DEFINITIONS_TYPE_SEMI                         =  6, // <variable_definitions> ::= type <def_expressions> ';'
   RULE_DEF_EXPRESSIONS                                        =  7, // <def_expressions> ::= <def_expression>
   RULE_DEF_EXPRESSIONS_COMMA                                  =  8, // <def_expressions> ::= <def_expression> ',' <def_expressions>
   RULE_DEF_EXPRESSION_IDENTIFIER                              =  9, // <def_expression> ::= identifier <def_init>
   RULE_DEF_INIT_EQ                                            = 10, // <def_init> ::= '=' <mult_expr>
   RULE_DEF_INIT                                               = 11, // <def_init> ::= 
   RULE_BLOCK_LBRACE_RBRACE                                    = 12, // <block> ::= '{' <variables> <statements> '}'
   RULE_STATEMENTS                                             = 13, // <statements> ::= <statements> <statement>
   RULE_STATEMENTS2                                            = 14, // <statements> ::= 
   RULE_STATEMENT_SEMI                                         = 15, // <statement> ::= <assignexpression> ';'
   RULE_STATEMENT_SEMI2                                        = 16, // <statement> ::= <procedurecall> ';'
   RULE_STATEMENT_SEMI3                                        = 17, // <statement> ::= <returnstmt> ';'
   RULE_STATEMENT                                              = 18, // <statement> ::= <block>
   RULE_RETURNSTMT_RETURN                                      = 19, // <returnstmt> ::= return <expression>
   RULE_EXPRESSION                                             = 20, // <expression> ::= <assignexpression>
   RULE_EXPRESSION2                                            = 21, // <expression> ::= <mult_expr>
   RULE_ASSIGNEXPRESSION_IDENTIFIER_EQ                         = 22, // <assignexpression> ::= identifier '=' <mult_expr>
   RULE_TYPE_CTOR_TYPE_LPARAN_RPARAN                           = 23, // <type_ctor> ::= type '(' <proc_arguments> ')'
   RULE_PROCEDURECALL_IDENTIFIER_LPARAN_RPARAN                 = 24, // <procedurecall> ::= identifier '(' <proc_arguments> ')'
   RULE_PROC_ARGUMENTS_COMMA                                   = 25, // <proc_arguments> ::= <expression> ',' <proc_arguments>
   RULE_PROC_ARGUMENTS                                         = 26, // <proc_arguments> ::= <expression>
   RULE_MULT_EXPR_TIMES                                        = 27, // <mult_expr> ::= <mult_expr> '*' <primary>
   RULE_MULT_EXPR                                              = 28, // <mult_expr> ::= <primary>
   RULE_PRIMARY_NUMBER                                         = 29, // <primary> ::= number
   RULE_PRIMARY_IDENTIFIER                                     = 30, // <primary> ::= identifier
   RULE_PRIMARY_STRINGCONSTANT                                 = 31, // <primary> ::= stringconstant
   RULE_PRIMARY                                                = 32, // <primary> ::= <type_ctor>
   RULE_PRIMARY2                                               = 33, // <primary> ::= <procedurecall>
   RULE_PRIMARY_LPARAN_RPARAN                                  = 34  // <primary> ::= '(' <expression> ')'
};

#endif
