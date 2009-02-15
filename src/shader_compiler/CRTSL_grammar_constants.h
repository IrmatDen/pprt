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
   SYMBOL_COMMA                =  6, // ','
   SYMBOL_SEMI                 =  7, // ';'
   SYMBOL_LBRACE               =  8, // '{'
   SYMBOL_RBRACE               =  9, // '}'
   SYMBOL_EQ                   = 10, // '='
   SYMBOL_IDENTIFIER           = 11, // identifier
   SYMBOL_NUMBER               = 12, // number
   SYMBOL_RETURN               = 13, // return
   SYMBOL_SHADER_TYPE          = 14, // 'shader_type'
   SYMBOL_STRINGCONSTANT       = 15, // stringconstant
   SYMBOL_TYPE                 = 16, // type
   SYMBOL_ASSIGNEXPRESSION     = 17, // <assignexpression>
   SYMBOL_BLOCK                = 18, // <block>
   SYMBOL_DEF_EXPRESSION       = 19, // <def_expression>
   SYMBOL_DEF_EXPRESSIONS      = 20, // <def_expressions>
   SYMBOL_DEF_INIT             = 21, // <def_init>
   SYMBOL_DEFINITIONS          = 22, // <definitions>
   SYMBOL_EXPRESSION           = 23, // <expression>
   SYMBOL_FORMALS              = 24, // <formals>
   SYMBOL_PRIMARY              = 25, // <primary>
   SYMBOL_PROC_ARGUMENTS       = 26, // <proc_arguments>
   SYMBOL_PROCEDURECALL        = 27, // <procedurecall>
   SYMBOL_RETURNSTMT           = 28, // <returnstmt>
   SYMBOL_SHADER_DEFINITION    = 29, // <shader_definition>
   SYMBOL_STATEMENT            = 30, // <statement>
   SYMBOL_STATEMENTS           = 31, // <statements>
   SYMBOL_TYPE_CTOR            = 32, // <type_ctor>
   SYMBOL_VARIABLE_DEFINITIONS = 33, // <variable_definitions>
   SYMBOL_VARIABLES            = 34  // <variables>
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
   RULE_DEF_INIT_EQ                                            = 10, // <def_init> ::= '=' <primary>
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
   RULE_EXPRESSION2                                            = 21, // <expression> ::= <primary>
   RULE_ASSIGNEXPRESSION_IDENTIFIER_EQ                         = 22, // <assignexpression> ::= identifier '=' <primary>
   RULE_TYPE_CTOR_TYPE_LPARAN_RPARAN                           = 23, // <type_ctor> ::= type '(' <proc_arguments> ')'
   RULE_PROCEDURECALL_IDENTIFIER_LPARAN_RPARAN                 = 24, // <procedurecall> ::= identifier '(' <proc_arguments> ')'
   RULE_PROC_ARGUMENTS_COMMA                                   = 25, // <proc_arguments> ::= <expression> ',' <proc_arguments>
   RULE_PROC_ARGUMENTS                                         = 26, // <proc_arguments> ::= <expression>
   RULE_PRIMARY_NUMBER                                         = 27, // <primary> ::= number
   RULE_PRIMARY_IDENTIFIER                                     = 28, // <primary> ::= identifier
   RULE_PRIMARY_STRINGCONSTANT                                 = 29, // <primary> ::= stringconstant
   RULE_PRIMARY                                                = 30, // <primary> ::= <type_ctor>
   RULE_PRIMARY2                                               = 31, // <primary> ::= <procedurecall>
   RULE_PRIMARY_LPARAN_RPARAN                                  = 32  // <primary> ::= '(' <expression> ')'
};

#endif
