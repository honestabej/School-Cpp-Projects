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
                // parse_epsilon()
                void parse_var_decl_list();
                    void parse_var_decl();
                        void parse_var_list();
                            // void parse_var_list()
                        void parse_type_name();
                    // void parse_var_decl_list()
            void parse_body();
                void parse_stmt_list();
                    void parse_stmt();
                        void parse_assignment_stmt();
                            Token parse_expression(int = 0);
                                Token parse_primary();
                                TokenType parse_binary_operator();
                                // void parse_expression()
                                // void parse_expression()
                                void parse_unary_operator();
                                // void parse_expression()
                        void parse_if_stmt();
                            // void parse_expression()
                        void parse_while_stmt();
                            // void parse_expression()
                        void parse_switch_stmt();
                            // void parse_expression()
                            void parse_case_list();
                                void parse_case();
                                    // void parse_body()
                                // void parse_case_list()
                    // void parse_stmt_list()


    private: 
        LexicalAnalyzer lexer;
        void syntax_error();
        void type_mismatch(int, string);
        Token peek(int, string = "");
        Token expect(TokenType expected_type);
        bool check_bool();
        void initialize_vars();
        int check_var_index(string);

};


#endif