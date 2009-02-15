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
   SYMBOL_PLUS                 = 11, // '+'
   SYMBOL_EQ                   = 12, // '='
   SYMBOL_IDENTIFIER           = 13, // identifier
   SYMBOL_NUMBER               = 14, // number
   SYMBOL_RETURN               = 15, // return
   SYMBOL_SHADER_TYPE          = 16, // 'shader_type'
   SYMBOL_STRINGCONSTANT       = 17, // stringconstant
   SYMBOL_TYPE                 = 18, // type
   SYMBOL_ADD_EXPR             = 19, // <add_expr>
   SYMBOL_ASSIGNEXPRESSION     = 20, // <assignexpression>
   SYMBOL_BLOCK                = 21, // <block>
   SYMBOL_DEF_EXPRESSION       = 22, // <def_expression>
   SYMBOL_DEF_EXPRESSIONS      = 23, // <def_expressions>
   SYMBOL_DEF_INIT             = 24, // <def_init>
   SYMBOL_DEFINITIONS          = 25, // <definitions>
   SYMBOL_EXPRESSION           = 26, // <expression>
   SYMBOL_FORMALS              = 27, // <formals>
   SYMBOL_MULT_EXPR            = 28, // <mult_expr>
   SYMBOL_PRIMARY              = 29, // <primary>
   SYMBOL_PROC_ARGUMENTS       = 30, // <proc_arguments>
   SYMBOL_PROCEDURECALL        = 31, // <procedurecall>
   SYMBOL_RETURNSTMT           = 32, // <returnstmt>
   SYMBOL_SHADER_DEFINITION    = 33, // <shader_definition>
   SYMBOL_STATEMENT            = 34, // <statement>
   SYMBOL_STATEMENTS           = 35, // <statements>
   SYMBOL_TYPE_CTOR            = 36, // <type_ctor>
   SYMBOL_VARIABLE_DEFINITIONS = 37, // <variable_definitions>
   SYMBOL_VARIABLES            = 38  // <variables>
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
   RULE_DEF_INIT_EQ                                            = 10, // <def_init> ::= '=' <add_expr>
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
   RULE_EXPRESSION2                                            = 21, // <expression> ::= <add_expr>
   RULE_ASSIGNEXPRESSION_IDENTIFIER_EQ                         = 22, // <assignexpression> ::= identifier '=' <add_expr>
   RULE_TYPE_CTOR_TYPE_LPARAN_RPARAN                           = 23, // <type_ctor> ::= type '(' <proc_arguments> ')'
   RULE_PROCEDURECALL_IDENTIFIER_LPARAN_RPARAN                 = 24, // <procedurecall> ::= identifier '(' <proc_arguments> ')'
   RULE_PROC_ARGUMENTS_COMMA                                   = 25, // <proc_arguments> ::= <expression> ',' <proc_arguments>
   RULE_PROC_ARGUMENTS                                         = 26, // <proc_arguments> ::= <expression>
   RULE_ADD_EXPR_PLUS                                          = 27, // <add_expr> ::= <add_expr> '+' <mult_expr>
   RULE_ADD_EXPR                                               = 28, // <add_expr> ::= <mult_expr>
   RULE_MULT_EXPR_TIMES                                        = 29, // <mult_expr> ::= <mult_expr> '*' <primary>
   RULE_MULT_EXPR                                              = 30, // <mult_expr> ::= <primary>
   RULE_PRIMARY_NUMBER                                         = 31, // <primary> ::= number
   RULE_PRIMARY_IDENTIFIER                                     = 32, // <primary> ::= identifier
   RULE_PRIMARY_STRINGCONSTANT                                 = 33, // <primary> ::= stringconstant
   RULE_PRIMARY                                                = 34, // <primary> ::= <type_ctor>
   RULE_PRIMARY2                                               = 35, // <primary> ::= <procedurecall>
   RULE_PRIMARY_LPARAN_RPARAN                                  = 36  // <primary> ::= '(' <expression> ')'
};

#endif
