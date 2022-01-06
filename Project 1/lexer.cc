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
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

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


// Scanning when lexeme starts with a digit, can return NUM, REALNUM, BASE08NUM, BASE16NUM, or ERROR
Token LexicalAnalyzer::ScanNumber() {
    
    char c, c1, c2;                     // Make variables c to store chars in
    bool isZeroNum = false;        // Seat a boolean variiable to check if the leading char is a zero
    bool base08num = true;              // Set a boolean variable to check if value is compatible with BASE08NUM requirements
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
                if (c == '8' || c == '9') {
                    base08num = false;  // Set base08num to false because 8 and 9 are not numbers in pdigit8 or digit8
                }
                tmp.lexeme += c;
                input.GetChar(c);
            }
        }

        // LEADING DIGITS NOW STORED IN tmp.lexeme

        // TODO: You can check for REALNUM, BASE08NUM and BASE16NUM here!
        
       
        // Check if it could be REALNUM, where c should be DOT
        if (c == '.') {
            isZeroNum = false;              // Since we know this is a valid leading 0, we set this back to false
            string afterDecimal = "";       // Declare a string to hold all of the numbers aftter the decimal
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

            input.UngetChar(c);             // Unget the DOT
        }

        // Check if it could be BASE08NUM or BASE16NUM when "x__" identifier is next
        else if (c == 'x'){
            // Fill id with the "x__" identifier
            string id = "";
            id += c;
            input.GetChar(c);
            id += c;
            input.GetChar(c);
            id += c;
            
            // Check if it could be a BASE08NUM
            if (id == "x08" && base08num == true){
                // BASE08NUM should only have numbers so the lexeme should already be complete and the "x08" identifier should be next
                if (isZeroNum == true) {
                    tmp.lexeme = "0" + id;  // Add the id to the lexeme with the leading zero
                } else {
                    tmp.lexeme += id;       // Add the id to the lexeme
                }
                tmp.token_type = BASE08NUM; // Set the token_type
                tmp.line_no = line_no;      // Set the line_no
                isZeroNum = false;          // Since we know this is a valid leading 0, we set this back to false
                return tmp; 
            }

            // Check if it could be BASE16NUM
            else if(id == "x16"){
                if (isZeroNum == true) {
                    tmp.lexeme = "0" + id;  // Add the id to the lexeme with the leading zero
                } else {
                    tmp.lexeme += id;       // Add the id to the lexeme
                }
                tmp.token_type = BASE16NUM; // Set the token_type
                tmp.line_no = line_no;      // Set the line_no
                isZeroNum = false;          // Since we know this is a valid leading 0, we set this back to false
                return tmp;
            }

            // If the indentifier was not "x08", "x16", or was "x08" but base08num = false, so unget the id string             
            else {
                input.UngetString(id);
            }
            
        } 

        // Check if it could be a BASE16NUM
        else if ((c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F') && isZeroNum == false) {
            string lexemeString = tmp.lexeme;
            string base16string = "";       // Create a string to hold the remaining letters & numbers in the lexeme

            // Only enter this loop if c2 is a valid base 16 character
            while(!input.EndOfInput() && (c == 'A' || c == 'B' || c == 'C' || c == 'D' || c == 'E' || c == 'F' || isdigit(c))){
                base16string += c;          // Concat the valid base 16 letters and numbers to the end of base16string
                input.GetChar(c);
            }

            // Now that the lexeme should be complete, the "x__" identifier should be next
            string id = "";
            id += c;
            input.GetChar(c);
            id += c;
            input.GetChar(c);
            id += c;

            if (id == "x16") {
                tmp.lexeme = lexemeString + base16string + id;  // Add the id to the lexeme
                tmp.token_type = BASE16NUM;                     // Set the token_type
                tmp.line_no = line_no;                          // Set the line_no
                return tmp;
            } else {
                // Identifier was not "x16" so unget the strings
                input.UngetString(id);
                input.UngetString(base16string);
            }

        // If the second character in the input was not '.', 'x', or a BASE16NUM character then unget it
        } else {
            input.UngetChar(c);
        }

        // If code reached this area before returning that means it was not a REALNUM, BASE08NUM, or BASE16NUM, so it must be NUM 
        if (isZeroNum == true){
            tmp.lexeme = "0";
        }
        // End of my implementation
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
        tmp.token_type = ERROR;         // Set the token_type to ERROR
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
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
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

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
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
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
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
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}
