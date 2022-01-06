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
    "INT", "REAL", "BOOL", "TRUE", "FALSE",
    "IF", "WHILE", "SWITCH", "CASE", "NOT",
    "PUBLIC", "PRIVATE", "PLUS", "MINUS", "MULT", 
    "DIV", "GREATER", "LESS", "GTEQ", "LTEQ",
    "NOTEQUAL", "LPAREN", "RPAREN", "NUM", "REALNUM",
    "EQUAL", "COLON", "COMMA", "SEMICOLON", "LBRACE",
    "RBRACE", "ID", "UNKNOWN" // Extras to include for errors
};

#define KEYWORDS_COUNT 12
string keyword[] = { "INT", "REAL", "BOOL", "TRUE", "FALSE", "IF", "WHILE", "SWITCH", "CASE", "NOT", "PUBLIC", "PRIVATE" };

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
    tmp.token_type = UNKNOWN;
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
    return UNKNOWN;
}


// Scanning when lexeme starts with a digit, can return NUM, REALNUM, BASE08NUM, BASE16NUM, or ERROR
Token LexicalAnalyzer::ScanNumber() {
    
    char c, c1, c2;                     // Make variables c to store chars in
    bool isZeroNum = false;             // Seat a boolean variiable to check if the leading char is a zero
    input.GetChar(c);                   // Store first char in input to c

    // If c is a digit, enter this if statement
    if (isdigit(c)) {

        // If c is 0, it is a NUM 
        if (c == '0') {
            tmp.lexeme = "0";
            isZeroNum = true;
            input.GetChar(c);
        } else {
            // Lexeme does not start with 0, and could be any of the 4 types
            tmp.lexeme = "";

            // Continue to get characters and concat them to the end of tmp.lexeme to get all leading digits before any letters or DOT
            while (!input.EndOfInput() && isdigit(c)) {
                tmp.lexeme += c;
                input.GetChar(c);
            }
        }

        // LEADING DIGITS NOW STORED IN tmp.lexeme      
       
        // Check if it could be REALNUM, where c should be DOT
        if (c == '.') {
            isZeroNum = false;              // Since we know this is a valid leading 0, we set this back to false
            string afterDecimal = "";       // Declare a string to hold all of the numbers after the decimal
            input.GetChar(c1);              // Start getting numbers after the decimal and storing to c1
            
            // If c2 is a digit, enter here
            if (isdigit(c1)) {
                // Continue getting all of the numbers after the decimal and concatenating them to the end of afterDecimal
                while(!input.EndOfInput() && isdigit(c1)){
                    afterDecimal += c1;
                    input.GetChar(c1);
                }
                // Unget char c2 if it is not an EOF character
                if (!input.EndOfInput()){
                    input.UngetChar(c1);
                }
                tmp.lexeme += c;            // Add the decimal place to the lexeme
                tmp.lexeme += afterDecimal; // Add the post decimal numbers to the lexeme
                tmp.token_type = REALNUM;   // Set the token_type
                tmp.line_no = line_no;      // Set the line_no
                return tmp;

            // If c2 was not a digit enter here
            } else {
                input.UngetChar(c1);        // Unget the char because c1 should only be a digit
            }

            // input.UngetChar(c);             // Unget the DOT
        }

        // End of my implementation
        input.UngetChar(c);
        tmp.token_type = NUM;
        tmp.line_no = line_no;
        return tmp;

    // If c was not a digit enter here
    } else {
        // Check if c was an EOF, and if it is not, unget it
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";                // Set the lexeme to empty
        tmp.token_type = UNKNOWN;         // Set the token_type to ERROR
        tmp.line_no = line_no;          // Set the line_no
        return tmp;
    }
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
        if (tmp.lexeme == "int") {
            tmp.token_type = INT;
        } else if (tmp.lexeme == "real") {
            tmp.token_type = REAL;
        } else if (tmp.lexeme == "bool") {
            tmp.token_type = BOOL;
        } else if (tmp.lexeme == "true") {
            tmp.token_type = TRUE;
        } else if (tmp.lexeme == "false") {
            tmp.token_type = FALSE;
        } else if (tmp.lexeme == "if") {
            tmp.token_type = IF;
        } else if (tmp.lexeme == "while") {
            tmp.token_type = WHILE;
        } else if (tmp.lexeme == "switch") {
            tmp.token_type = SWITCH;
        } else if (tmp.lexeme == "case") {
            tmp.token_type = CASE;
        } else if (tmp.lexeme == "public") {
            tmp.token_type = PUBLIC;
        } else if (tmp.lexeme == "private") {
            tmp.token_type = PRIVATE;
        } else {
            tmp.token_type = ID;
        }
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = UNKNOWN;
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

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '!':
            tmp.lexeme = "!";
            tmp.token_type = NOT;
            return tmp;
        case '+':
            tmp.lexeme = "+";
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.lexeme = "-";
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.lexeme = "/";
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.lexeme = "*";
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.lexeme = "=";
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.lexeme = ":";
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.lexeme = ",";
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.lexeme = ";";
            tmp.token_type = SEMICOLON;
            return tmp;
        case '{':
            tmp.lexeme = "{";
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.lexeme = "}";
            tmp.token_type = RBRACE;
            return tmp;
        case '(':
            tmp.lexeme = "(";
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.lexeme = ")";
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                // tmp.lexeme = "<=";
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                // tmp.lexeme = "<>";
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                // tmp.lexeme = "<";
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                // tmp.lexeme = ">=";
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                // tmp.lexeme = ">";
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput()) {
                // tmp.lexeme = "EOF";
                tmp.token_type = END_OF_FILE;
            } else {
                // tmp.lexeme = "Error";
                tmp.token_type = UNKNOWN;
            }
            return tmp;
    }
}
