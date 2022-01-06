/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "PUBLIC", "PRIVATE", "COMMENT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRACE", "RBRACE", "ID", "ERROR" // TODO: Add labels for new token types here
};

#define KEYWORDS_COUNT 5
string keyword[] = {"public", "private", "//" };

// The print output statement, the prints the 
void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

// Sets the defualt temp lexical analyzer values
LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

// I used to skip the space in the input
bool LexicalAnalyzer::SkipSpace()
{
    // Variables to hold the space in 'c' and make bool value that by default is false for if a space is encountered
    char c;
    bool space_encountered = false;

    // Get the next char and store in c and increment line_no 
    input.GetChar(c);
    line_no += (c == '\n');

    // If the input is not at the EOF and c was a space enter the loop
    while (!input.EndOfInput() && isspace(c)) {
        // Set space_encountered to true since thats the only way tthis loop is entered
        space_encountered = true;
        // Get the next char and store in c and increment line_no
        input.GetChar(c);
        line_no += (c == '\n');
    }

    // Now that you have broken the loop it means c is not a space, so unless it is EOF you should unget the char
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

// Checks if the string is one of the set keywords
bool LexicalAnalyzer::IsKeyword(string s)
{
    // Increment through the entire keyword array and check if there is a match
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}


TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    // Increment through the entire keyword array then get the index of the matching keyword and add 1 to it to get the corresponding TokenType index
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

// Scanning when lexeme starts with a letter, can return ID, KEYWORD, or ERROR
Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);


    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;

        if (tmp.lexeme == "public"){
            tmp.token_type = PUBLIC;
        } else if (tmp.lexeme == "private"){
            tmp.token_type = PRIVATE;
        } else {
            tmp.token_type = ID;
        }
    } else if (c == '/') {
        input.GetChar(c);
        if (c == '/') {
            tmp.lexeme = "//";
            tmp.line_no = line_no;
            tmp.token_type = COMMENT;
        } else {
            if (!input.EndOfInput()) {
            input.UngetChar(c);
            }
            tmp.lexeme = "";
            tmp.token_type = ERROR;
        }
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken()
{
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    // SkipSpace();
    // tmp.lexeme = "";
    // tmp.line_no = line_no;
    // input.GetChar(c);
    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    tmp.token_type = END_OF_FILE;
    if (!input.EndOfInput())
        input.GetChar(c);
    else
        return tmp;

    switch (c) {
        case '=': 
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;

        default:
            if (c == '/'){
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (isdigit(c)) {
                input.UngetChar(c);
                // return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

// int main()
// {
//     LexicalAnalyzer lexer;
//     Token token;

//     token = lexer.GetToken();
//     token.Print();
//     while (token.token_type != END_OF_FILE)
//     {
//         token = lexer.GetToken();
//         token.Print();
//     }
// }
