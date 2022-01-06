#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include "parser.h"
#include "lexer.h"

using namespace std;

// Data structures
struct item{
    string permissions;
    vector<struct scope*> scopes;
    string varName;
};

struct scope{
    string scopeName;
    int scopeNum;
};

vector<struct item*> itemsVec;
vector<struct scope*> currentScope;
string currentPermission;
string output;
int scopeNumber = 0;

// Parser methods
void Parser::syntax_error() {
    cout << "Syntax Error\n";
    exit(1);
}

// Begin parsing the entire program
void Parser::parse_program() {
    token_check();
    parse_global_vars();
    parse_scope();
    // for (int k = 0; k < itemsVec.size(); k++) {
    //     cout << "Name: " << itemsVec[k]->varName << "\tpermission: " << itemsVec[k]->permissions << "\tscope: " << itemsVec[k]->scope << endl;
    // }
    token_check();
    expect(END_OF_FILE);
    return;
}

// For parsing the global variables
void Parser::parse_global_vars() {
    token_check();
    struct scope* globalScope = new scope;
    globalScope->scopeName = "::";
    globalScope->scopeNum = scopeNumber;
    scopeNumber++;
    currentScope.push_back(globalScope);
    currentPermission = "::";
    Token t = peek(2);    // Peek 2 tokens ahead to see if there is an LBRACE, determining if this is a variable ID or scope ID
    if (t.token_type == LBRACE) {
        currentScope.pop_back();
        return;
    } else {
        parse_var_list();
        token_check();
        expect(SEMICOLON);
        currentScope.pop_back();
        return;
    }
}

// For parsing a global variable
void Parser::parse_var_list() {
    token_check();
    struct item* it = new item;
    Token t = expect(ID);
    bool found = false;
    
    // If permission is private, treat it as a new item
    if (currentPermission == "private"){
        // Check if a private item with this varName already exists in the scope
        for (int k = 0; k < itemsVec.size(); k++) {
            if (t.lexeme == itemsVec[k]->varName && itemsVec[k]->scopes.back() == currentScope.back()){
                itemsVec[k]->permissions = currentPermission;
                itemsVec[k]->scopes.push_back(currentScope.back());
                found = true;
            }
        }
        if (found == false) {
            it->permissions = currentPermission;
            it->scopes.push_back(currentScope.back());
            it->varName = t.lexeme;
            itemsVec.push_back(it);
        }

    // If permission is not private, check if the item name already exists and overwrite it, if not found make the new item
    } else {
        for (int k = 0; k < itemsVec.size(); k++) {
            if (t.lexeme == itemsVec[k]->varName && itemsVec[k]->permissions != "private"){
                itemsVec[k]->permissions = currentPermission;
                itemsVec[k]->scopes.push_back(currentScope.back());
                found = true;
            }
        }
        if (found == false) {
            it->permissions = currentPermission;
            it->scopes.push_back(currentScope.back());
            it->varName = t.lexeme;
            itemsVec.push_back(it);
        }
    }
    token_check();
    t = peek(1);
    if (t.token_type != COMMA && t.token_type != SEMICOLON) { 
        syntax_error();
    } else {
        if (t.token_type == COMMA) {
            token_check();
            expect(COMMA);
            parse_var_list();
        }
        return;
    }
}

// For parsing the current scope
void Parser::parse_scope() {
    token_check();
    struct scope* newScope = new scope;
    Token t = expect(ID);
    newScope->scopeName = t.lexeme;
    newScope->scopeNum = scopeNumber;
    scopeNumber++;
    currentScope.push_back(newScope);
    token_check();
    expect(LBRACE); 
    parse_public_vars();
    parse_private_vars();
    token_check();
    t = peek(1);
    if (t.token_type != ID){
        syntax_error();
    }
    parse_stmt_list();
    token_check();
    expect(RBRACE);
    for (int g = 0; g < itemsVec.size(); g++){
        if (itemsVec[g]->scopes.back() == currentScope.back() && itemsVec[g]->permissions == "public") {
            itemsVec[g]->scopes.pop_back();
        }
    }
    currentScope.pop_back();
    return;
}

// For parsing the list of public variables in the current scope
void Parser::parse_public_vars(){
    token_check();
    Token t = peek(1);
    if (t.token_type == PUBLIC){
        token_check();
        expect(PUBLIC);
        token_check();
        expect(COLON);
        currentPermission = "public";
        parse_var_list();
        token_check();
        expect(SEMICOLON);
        return;
    } else {
        return;
    }
}

// For parsing the list of private variables in the current scope
void Parser::parse_private_vars(){
    token_check();
    Token t = peek(1);
    if (t.token_type == PRIVATE){
        token_check();
        expect(PRIVATE);
        token_check();
        expect(COLON);
        currentPermission = "private";
        parse_var_list();
        token_check();
        expect(SEMICOLON);
        return;
    } else {
        return;
    }
}

// For parsing the list of statements in the current scope
void Parser::parse_stmt_list(){
    token_check();
    parse_stmt(); 
    token_check();
    Token t = peek(1);
    if (t.token_type == ID) {
        parse_stmt_list(); 
    }
    return;
}

// Check for nested scope and parse the statement if it is not
void Parser::parse_stmt(){
    token_check();
    Token t = peek(2);
    Token tok;
    struct scope* aScope = new scope;
    struct item* first;
    struct item* second;
    int firstFound = 0;
    int secondFound = 0;

    if (t.token_type == LBRACE){
        parse_scope();
        return;
    } else {
        token_check();
        t = expect(ID);
        
        // Get the first item
        for (int i = 0; i < itemsVec.size(); i++) {
            // t is found and is a private item in this scope
            if (itemsVec[i]->varName == t.lexeme && itemsVec[i]->permissions == "private" && itemsVec[i]->scopes.back() == currentScope.back()){
                first = itemsVec[i];
                firstFound = 1;
                break;  // Prioritize private variables of current scope so stop searching when found

            // t is found and is global
            } else if (itemsVec[i]->varName == t.lexeme && itemsVec[i]->permissions == "::") {
                first = itemsVec[i];
                firstFound = 2;

            // t is found and is public
            } else if (itemsVec[i]->varName == t.lexeme && itemsVec[i]->permissions == "public") {
                // Check that it is in a nested scope of the declaration
                for (int l = 0; l < currentScope.size(); l++){
                    if (itemsVec[i]->scopes.back() == currentScope[l]) {
                        first = itemsVec[i];
                        firstFound = 2;
                        break;
                    }
                }
            }
        }
        // Append first variable to output
        if (firstFound == 1) {
            string appending = first->scopes.back()->scopeName + "." + first->varName + " = ";
            output.append(appending);
        // } else if (firstFound == 2) {
        //     string appending = "?." + first->varName + " = ";
        //     output.append(appending);
        } else if (firstFound == 2) {
            string appending = "";
            if (first->permissions == "public"){
                appending = first->scopes.back()->scopeName + "." + first->varName + " = ";
            } else {
                appending = first->scopes.back()->scopeName + first->varName + " = ";
            }
            output.append(appending);
        } else {
            string appending = "?." + t.lexeme + " = ";
            output.append(appending);
        }

        token_check();
        expect(EQUAL);
        token_check();
        tok = expect(ID);

        // Get the second item
        for (int j = 0; j < itemsVec.size(); j++) {
            // t is found and is a private item in this scope
            if (itemsVec[j]->varName == tok.lexeme && itemsVec[j]->permissions == "private" && itemsVec[j]->scopes.back() == currentScope.back()){
                second = itemsVec[j];
                secondFound = 1;
                break;  // Prioritize private variables of current scope so stop searching when found

            // t is found but is global
            } else if (itemsVec[j]->varName == tok.lexeme && itemsVec[j]->permissions == "::") {
                second = itemsVec[j];
                secondFound = 2;

            // t is found and is public
            } else if (itemsVec[j]->varName == tok.lexeme && itemsVec[j]->permissions == "public") {
                // Check that it is in a nested scope of the declaration
                for(int u = 0; u < currentScope.size(); u++) {
                    if (itemsVec[j]->scopes.back() == currentScope[u]){
                        second = itemsVec[j];
                        secondFound = 2;
                        break;
                    }
                }
            }
        }
        // Append the second variable to the output
        if (secondFound == 1) {
            string appending = second->scopes.back()->scopeName + "." + second->varName + "\n";
            output.append(appending);
        } else if (secondFound == 2) {
            string appending = "";
            if (second->permissions == "public") {
                appending = second->scopes.back()->scopeName + "." + second->varName + "\n";
            } else {
                appending = second->scopes.back()->scopeName + second->varName + "\n";
            }
            output.append(appending);
        } else {
            string appending = "?." + tok.lexeme + "\n";
            output.append(appending);
        }

        token_check();
        expect(SEMICOLON);

        // Setting equal and printing
        
        return;
    }
}

// Check if next token is a comment
void Parser::token_check(){
    Token t = peek(1);
    if (t.token_type == COMMENT) {
        parse_comments();
    }
}

// Function to parse any comments
void Parser::parse_comments(){
    Token t = lexer.GetToken();
    int com = t.line_no;
    t = peek(1);
    while (t.line_no == com && t.token_type != END_OF_FILE){
        t = lexer.GetToken();
        t = peek(1);
    }
    token_check();
}


// Expect function to get token and check for the correct type
Token Parser::expect(TokenType expected_type) {
    Token t = lexer.GetToken();

    if (t.token_type != expected_type)
        // cout << "Token type is: " << t.token_type << " but expected was: " << expected_type << endl;
        syntax_error();
    return t;
}

Token Parser::peek(int howFar, string tmp) {
    Token tok;
    vector<Token> toks;
    for (int i = 0; i < howFar; i++) {
        tok = lexer.GetToken();
        toks.push_back(tok);
    }
    for (int j = howFar - 1; j >= 0; j--) {
        lexer.UngetToken(toks[j]);
    }
    return tok;
}

int main() {
    Parser parser;
    parser.parse_program();
    cout << output;
}