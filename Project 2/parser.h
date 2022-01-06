#ifndef __PARSER_H__
#define __PARSER_H__

#include <cstdlib>
#include <stdlib.h>
#include "lexer.h"
using namespace std;

class Parser {
    public: 
        void parse_program();
            void parse_global_vars();
                void parse_var_list();
            void parse_scope();
                void parse_public_vars();
                    // void parse_var_list();
                void parse_private_vars();
                    // void parse_var_list();
                void parse_stmt_list();
                    void parse_stmt();
                        // void parse_scope();
        void parse_comments();
        void token_check();


    private: 
        LexicalAnalyzer lexer;
        Token peek(int, string = "");
        void syntax_error();
        Token expect(TokenType expected_type);

};


#endif