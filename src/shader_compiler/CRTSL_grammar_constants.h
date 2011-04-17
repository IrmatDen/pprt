#ifndef PPRT_GRAMMAR_CONSTANTS_H
#define PPRT_GRAMMAR_CONSTANTS_H

enum SymbolConstants
{
   SYMBOL_EOF                  =  0, // (EOF)
   SYMBOL_ERROR                =  1, // (Error)
   SYMBOL_WHITESPACE           =  2, // (Whitespace)
   SYMBOL_COMMENTLINE          =  3, // (Comment Line)
   SYMBOL_MINUS                =  4, // '-'
   SYMBOL_LPARAN               =  5, // '('
   SYMBOL_RPARAN               =  6, // ')'
   SYMBOL_TIMES                =  7, // '*'
   SYMBOL_COMMA                =  8, // ','
   SYMBOL_DOT                  =  9, // '.'
   SYMBOL_SEMI                 = 10, // ';'
   SYMBOL_LBRACE               = 11, // '{'
   SYMBOL_RBRACE               = 12, // '}'
   SYMBOL_PLUS                 = 13, // '+'
   SYMBOL_EQ                   = 14, // '='
   SYMBOL_IDENTIFIER           = 15, // identifier
   SYMBOL_NUMBER               = 16, // number
   SYMBOL_RETURN               = 17, // return
   SYMBOL_SHADER_TYPE          = 18, // 'shader_type'
   SYMBOL_STRINGCONSTANT       = 19, // stringconstant
   SYMBOL_TYPE                 = 20, // type
   SYMBOL_ADD_EXPR             = 21, // <add_expr>
   SYMBOL_ASSIGNEXPRESSION     = 22, // <assignexpression>
   SYMBOL_BLOCK                = 23, // <block>
   SYMBOL_DEF_EXPRESSION       = 24, // <def_expression>
   SYMBOL_DEF_EXPRESSIONS      = 25, // <def_expressions>
   SYMBOL_DEF_INIT             = 26, // <def_init>
   SYMBOL_DEFINITIONS          = 27, // <definitions>
   SYMBOL_DOT_EXPR             = 28, // <dot_expr>
   SYMBOL_EXPRESSION           = 29, // <expression>
   SYMBOL_FORMALS              = 30, // <formals>
   SYMBOL_MULT_EXPR            = 31, // <mult_expr>
   SYMBOL_NEGATE               = 32, // <negate>
   SYMBOL_PRIMARY              = 33, // <primary>
   SYMBOL_PROC_ARGUMENTS       = 34, // <proc_arguments>
   SYMBOL_PROC_ARGUMENTS_LIST  = 35, // <proc_arguments_list>
   SYMBOL_PROCEDURECALL        = 36, // <procedurecall>
   SYMBOL_RETURNSTMT           = 37, // <returnstmt>
   SYMBOL_SHADER_DEFINITION    = 38, // <shader_definition>
   SYMBOL_STATEMENT            = 39, // <statement>
   SYMBOL_STATEMENTS           = 40, // <statements>
   SYMBOL_SUB_EXPR             = 41, // <sub_expr>
   SYMBOL_TYPE_CTOR            = 42, // <type_ctor>
   SYMBOL_VARIABLE_DEFINITIONS = 43, // <variable_definitions>
   SYMBOL_VARIABLES            = 44  // <variables>
};

enum RuleConstants
{
   RULE_DEFINITIONS                                            =  0, // <definitions> ::= <shader_definition> <definitions>
   RULE_DEFINITIONS2                                           =  1, // <definitions> ::= 
   RULE_SHADER_DEFINITION_SHADER_TYPE_IDENTIFIER_LPARAN_RPARAN =  2, // <shader_definition> ::= 'shader_type' identifier '(' <formals> ')' <block>
   RULE_FORMALS                                                =  3, // <formals> ::= <variable_definitions> <formals>
   RULE_FORMALS2                                               =  4, // <formals> ::= 
   RULE_VARIABLES                                              =  5, // <variables> ::= <variable_definitions> <variables>
   RULE_VARIABLES2                                             =  6, // <variables> ::= 
   RULE_VARIABLE_DEFINITIONS_TYPE_SEMI                         =  7, // <variable_definitions> ::= type <def_expressions> ';'
   RULE_DEF_EXPRESSIONS                                        =  8, // <def_expressions> ::= <def_expression>
   RULE_DEF_EXPRESSIONS_COMMA                                  =  9, // <def_expressions> ::= <def_expression> ',' <def_expressions>
   RULE_DEF_EXPRESSION_IDENTIFIER                              = 10, // <def_expression> ::= identifier <def_init>
   RULE_DEF_INIT_EQ                                            = 11, // <def_init> ::= '=' <add_expr>
   RULE_DEF_INIT                                               = 12, // <def_init> ::= 
   RULE_BLOCK_LBRACE_RBRACE                                    = 13, // <block> ::= '{' <variables> <statements> '}'
   RULE_STATEMENTS                                             = 14, // <statements> ::= <statements> <statement>
   RULE_STATEMENTS2                                            = 15, // <statements> ::= 
   RULE_STATEMENT_SEMI                                         = 16, // <statement> ::= <assignexpression> ';'
   RULE_STATEMENT_SEMI2                                        = 17, // <statement> ::= <procedurecall> ';'
   RULE_STATEMENT_SEMI3                                        = 18, // <statement> ::= <returnstmt> ';'
   RULE_STATEMENT                                              = 19, // <statement> ::= <block>
   RULE_RETURNSTMT_RETURN                                      = 20, // <returnstmt> ::= return <expression>
   RULE_EXPRESSION                                             = 21, // <expression> ::= <assignexpression>
   RULE_EXPRESSION2                                            = 22, // <expression> ::= <add_expr>
   RULE_ASSIGNEXPRESSION_IDENTIFIER_EQ                         = 23, // <assignexpression> ::= identifier '=' <add_expr>
   RULE_TYPE_CTOR_TYPE_LPARAN_RPARAN                           = 24, // <type_ctor> ::= type '(' <proc_arguments> ')'
   RULE_PROCEDURECALL_IDENTIFIER_LPARAN_RPARAN                 = 25, // <procedurecall> ::= identifier '(' <proc_arguments> ')'
   RULE_PROC_ARGUMENTS                                         = 26, // <proc_arguments> ::= <proc_arguments_list>
   RULE_PROC_ARGUMENTS2                                        = 27, // <proc_arguments> ::= 
   RULE_PROC_ARGUMENTS_LIST_COMMA                              = 28, // <proc_arguments_list> ::= <expression> ',' <proc_arguments_list>
   RULE_PROC_ARGUMENTS_LIST                                    = 29, // <proc_arguments_list> ::= <expression>
   RULE_ADD_EXPR_PLUS                                          = 30, // <add_expr> ::= <add_expr> '+' <sub_expr>
   RULE_ADD_EXPR                                               = 31, // <add_expr> ::= <sub_expr>
   RULE_SUB_EXPR_MINUS                                         = 32, // <sub_expr> ::= <sub_expr> '-' <mult_expr>
   RULE_SUB_EXPR                                               = 33, // <sub_expr> ::= <mult_expr>
   RULE_MULT_EXPR_TIMES                                        = 34, // <mult_expr> ::= <mult_expr> '*' <dot_expr>
   RULE_MULT_EXPR                                              = 35, // <mult_expr> ::= <dot_expr>
   RULE_DOT_EXPR_DOT                                           = 36, // <dot_expr> ::= <dot_expr> '.' <primary>
   RULE_DOT_EXPR                                               = 37, // <dot_expr> ::= <negate>
   RULE_DOT_EXPR2                                              = 38, // <dot_expr> ::= <primary>
   RULE_NEGATE_MINUS                                           = 39, // <negate> ::= '-' <primary>
   RULE_PRIMARY_NUMBER                                         = 40, // <primary> ::= number
   RULE_PRIMARY_IDENTIFIER                                     = 41, // <primary> ::= identifier
   RULE_PRIMARY_STRINGCONSTANT                                 = 42, // <primary> ::= stringconstant
   RULE_PRIMARY                                                = 43, // <primary> ::= <type_ctor>
   RULE_PRIMARY2                                               = 44, // <primary> ::= <procedurecall>
   RULE_PRIMARY_LPARAN_RPARAN                                  = 45  // <primary> ::= '(' <expression> ')'
};

#endif
