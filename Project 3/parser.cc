#include <iostream>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include "parser.h"
#include "lexer.h"

using namespace std;

struct variable {
    string varName;
    TokenType varType;
    vector<struct variable*> eq;
    bool printed;
    int ordered;
};

// global vectors needed
vector<string> currentVars;
vector<struct variable*> allVars;
vector<struct variable*> currentOperands;
// global variables needed
bool typeFound;
bool fromSwitchStmt;
int order = 0;
// initialize local functions
void set_eq(variable*, variable*);
void make_var(string, TokenType, bool, int);
bool order_eq(const variable*, const variable*);
void print();

// Parser methods
void Parser::syntax_error() {
    cout << "Syntax Error\n";
    exit(1);
}

void Parser::type_mismatch(int line, string errType) {
    cout << "TYPE MISMATCH " << line << " " << errType << endl;
    exit(1);
}

void Parser::parse_program() {
    parse_global_vars();
    parse_body();
    // for (int k = 0; k < allVars.size(); k++) {
    //     cout << "Variable " << allVars[k]->varName << " has type " << allVars[k]->varType << endl;
    // }
    expect(END_OF_FILE);
    return;
}

void Parser::parse_global_vars() {
    Token t = peek(1);
    if (t.token_type != LBRACE) {
        parse_var_decl_list();
    }
    return;
}

void Parser::parse_var_decl_list() {
    parse_var_decl();
    Token t = peek(1);
    if (t.token_type != LBRACE) {
        parse_var_decl_list();
    }
    return;
}

void Parser::parse_var_decl() {
    parse_var_list();
    expect(COLON);
    parse_type_name();
    expect(SEMICOLON);
    currentVars.clear();
    return;
}

void Parser::parse_var_list() {
    Token tok = peek(1);
    expect(ID);
    currentVars.push_back(tok.lexeme);
    Token t = peek(1);
    if (t.token_type == COMMA) {
        expect(COMMA);
        parse_var_list();
    }
    return;
}

void Parser::parse_type_name() {
    Token t = lexer.GetToken();
    for (int i = 0; i < currentVars.size(); i++) {
        struct variable* newVariable = new variable;
        newVariable->varName = currentVars[i];
        newVariable->varType = t.token_type;
        allVars.push_back(newVariable);
    }
    if (t.token_type != INT && t.token_type != REAL && t.token_type != BOOL) {
        syntax_error();
    } else {
        return;
    }
}



// PARSING BODY BEGINS
void Parser::parse_body() {
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}

void Parser::parse_stmt_list() {
    parse_stmt();
    Token t = peek(1);
    if (t.token_type == RBRACE) {
        return;
    } else {
        parse_stmt_list();
        return;
    } 
}

void Parser::parse_stmt() {
    Token t = peek(1);
    if (t.token_type == IF) {
        parse_if_stmt();
    } else if (t.token_type == WHILE) {
        parse_while_stmt();
    } else if (t.token_type == SWITCH) {
        parse_switch_stmt();
    } else {
        parse_assignment_stmt();
    }
    return;
}

void Parser::parse_assignment_stmt() {
    // Set the expected type from the left hand side. Set typeFound if left var was declared, false if it is implicitly declared
    Token t = peek(1);
    TokenType leftType = UNKNOWN;
    int leftVar = check_var_index(t.lexeme);
    if (leftVar == -1) {
        make_var(t.lexeme, leftType, false, order);
        typeFound = false;
    } else {
        leftType = allVars[leftVar]->varType;
        typeFound = true;
    }
    expect(ID);
    expect(EQUAL);
    bool boolFound = check_bool();
    initialize_vars();
    TokenType rightType = parse_expression(t.line_no).token_type;
    // Check that the left side is the same type as the right side (C1)

    // !ONE WORKS
    // Both sides have a defined type
    if (leftType != UNKNOWN && rightType != UNKNOWN) {
        if (leftType != rightType) {
            type_mismatch(t.line_no, "C1");
        }
        // Set equivalance
        if (boolFound == false) {
            for (int l = 0; l < allVars.size(); l++) {
                if (allVars[l]->varName == t.lexeme) {
                    for (int q = 0; q < currentOperands.size(); q++) {
                        set_eq(allVars[l], currentOperands[q]);
                    }
                    break;
                }
            } 
        }
    }

    // left side does not have a defined type
    else if (leftType == UNKNOWN && rightType != UNKNOWN) {
        allVars[leftVar]->varType = rightType;
        // Set equivalance
        if (boolFound == false) {
            for (int q = 0; q < currentOperands.size(); q++) {
                set_eq(allVars[leftVar], currentOperands[q]);
            }
        }
    } 

    // right side does not have a defined type
    else if (leftType != UNKNOWN && rightType == UNKNOWN) {
        if (boolFound == false) {
            // Assign all right variables (in currentOperands) to the left type
            for (int i = 0; i < currentOperands.size(); i++) {
                currentOperands[i]->varType = leftType;
            }
            // Set equivalancies
            for (int l = 0; l < allVars.size(); l++) {
                if (allVars[l]->varName == t.lexeme) {
                    for (int q = 0; q < currentOperands.size(); q++) {
                        set_eq(allVars[l], currentOperands[q]);
                    }
                    break;
                }
            }
        }
    }

    // both sides are unknown
    else {
        if (boolFound == false) {
            for (int q = 0; q < currentOperands.size(); q++) {
                set_eq(allVars[leftVar], currentOperands[q]);
            }
        }
    }



    // ONE WORKS enters because leftType is found and marked as true (its type is UNKNOWN)
    // if (typeFound == true && rightType != UNKNOWN) {
    //     if (leftType != rightType) {
    //         type_mismatch(t.line_no, "C1");
    //     }
    // }

    expect(SEMICOLON);
    return;
}

void Parser::parse_if_stmt() {
    expect(IF);
    Token lineNum = expect(LPAREN);
    initialize_vars();
    TokenType t = parse_expression(lineNum.line_no).token_type;
    // Condition of if (and while) statements should be bool (C4)
    if (t != BOOL) {
        type_mismatch(lineNum.line_no, "C4");
    }
    expect(RPAREN);
    parse_body();
    return;
}

void Parser::parse_while_stmt() {
    Token lineNum = expect(WHILE);
    expect(LPAREN);
    initialize_vars();
    TokenType t = parse_expression(lineNum.line_no).token_type;
    // Condition of while (and if) statements should be bool (C4)
    if (t != BOOL) {
        type_mismatch(lineNum.line_no, "C4");
    }
    expect(RPAREN);
    parse_body();
    return;
}

void Parser::parse_switch_stmt() {
    Token lineNum = expect(SWITCH);
    expect(LPAREN);
    fromSwitchStmt = true;
    initialize_vars();
    TokenType t = parse_expression(lineNum.line_no).token_type;
    fromSwitchStmt = false;
    // Expression after switch keyword should be type int (C5)
    if (t == UNKNOWN) {
        t = INT;
    }
    if (t != INT) {
        type_mismatch(lineNum.line_no, "C5");
    }
    expect(RPAREN);
    expect(LBRACE);
    parse_case_list();
    expect(RBRACE);
    return;
}

Token Parser::parse_expression(int lineNum) {
    Token tok;
    Token t = peek(1);
    if (t.token_type == ID || t.token_type == NUM || t.token_type == REALNUM || t.token_type == TRUE || t.token_type == FALSE) {
        tok = parse_primary();
        return tok;
    } else if (t.token_type == NOT) {
        parse_unary_operator();
        Token check = parse_expression(lineNum);
        // Check that the unary operand is bool (C3)
        if (fromSwitchStmt == true) {
            type_mismatch(lineNum, "C5");
        } else if (check.token_type != BOOL) {
            type_mismatch(lineNum, "C3"); 
        }
        return check;
    } else {
        TokenType check = parse_binary_operator();
        Token check1 = parse_expression(lineNum);
        Token check2 = parse_expression(lineNum);

        // Check if it should be bool or not
        // if (check != BOOL) {
        // If both expressions were decalred or are constants
        if (check1.token_type != UNKNOWN && check2.token_type != UNKNOWN) { 
            // Check that the two parsed operands are the same type (C2)        
            if (check1.token_type != check2.token_type) {
                type_mismatch(lineNum, "C2"); 
            }
            // Set equivalancies
            for (int i = 0; i < allVars.size(); i++) {
                for (int k = 0; k < allVars.size(); k++) {
                    if (check1.lexeme == allVars[i]->varName && check2.lexeme == allVars[k]->varName) {
                        set_eq(allVars[i], allVars[k]);
                    }
                }
            }
            if (check == BOOL) {
                Token temp; temp.lexeme = "temp"; temp.token_type = BOOL;
                return temp;
            } else {
                return check1;
            }
        // If only the first expression was undeclared
        } else if (check1.token_type == UNKNOWN && check2.token_type != UNKNOWN) { 
            int exp1 = check_var_index(check1.lexeme);
            if (exp1 != -1) { 
                allVars[exp1]->varType = check2.token_type;
                currentOperands.push_back(allVars[exp1]);
                // Set equivalancies
                for (int i = 0; i < allVars.size(); i++) {
                    if (allVars[exp1]->varName == check1.lexeme && allVars[i]->varName == check2.lexeme) {
                        set_eq(allVars[exp1], allVars[i]);
                    }
                }
            }
            if (check == BOOL) {
                Token temp; temp.lexeme = "temp"; temp.token_type = BOOL;
                return temp;
            } else {
                return check2;
            }
        // If only the second expression was undeclared
        } else if (check1.token_type != UNKNOWN && check2.token_type == UNKNOWN) {
            int exp2 = check_var_index(check2.lexeme);  
            if (check2.lexeme != "temp") { 
                allVars[exp2]->varType = check2.token_type;
                currentOperands.push_back(allVars[exp2]);
                // Set equivalancies
                for (int i = 0; i < allVars.size(); i++) {
                    if (allVars[i]->varName == check1.lexeme && allVars[exp2]->varName == check2.lexeme) {
                        set_eq(allVars[exp2], allVars[i]);
                    }
                }
            }
            if (check == BOOL) {
                Token temp; temp.lexeme = "temp"; temp.token_type = BOOL;
                return temp;
            } else {
                return check1;
            }
        // If both expressions were undeclared
        } else {          
            int exp1 = check_var_index(check1.lexeme);
            int exp2 = check_var_index(check2.lexeme);
            if (exp1 != -1) {
                allVars[exp1]->varType = UNKNOWN;
                currentOperands.push_back(allVars[exp1]);
            }
            if (exp2 != -1) {
                allVars[exp2]->varType = UNKNOWN;
                currentOperands.push_back(allVars[exp2]);
            }
            if (exp1 != -1 && exp2 != -1) {
                set_eq(allVars[exp1], allVars[exp2]);
            }

            if (check == BOOL) {
                Token temp; temp.lexeme = "temp"; temp.token_type = BOOL;
                return temp;
            } else {
                Token temp; temp.lexeme = "temp"; temp.token_type = UNKNOWN;
                return temp;
            }
            
        }
    } 
}


Token Parser::parse_primary() {
    Token t = lexer.GetToken();
    if (t.token_type != ID && t.token_type != NUM && t.token_type != REALNUM && t.token_type != TRUE && t.token_type != FALSE) {
        syntax_error();
    }
    if (t.token_type == ID) {
        // Finding variable in vector
        for (int i = 0; i < allVars.size(); i++) {
            if (t.lexeme == allVars[i]->varName) {
                currentOperands.push_back(allVars[i]);
                t.lexeme = allVars[i]->varName;
                t.lexeme = allVars[i]->varType;
                return t;
                break;
            }
        }
        t.token_type = UNKNOWN;
    } else if (t.token_type == NUM) {
        // cout << "NUM ran" << endl;
        t.token_type = INT;
    } else if (t.token_type == REALNUM) {
        // cout << "REAL ran" << endl;
        t.token_type = REAL;
    } else if (t.token_type == TRUE || t.token_type == FALSE) {
        // cout << "TRUE/FALSE ran" << endl;
        t.token_type = BOOL;
    }
    return t;
}

void Parser::parse_unary_operator() {
    expect(NOT);
    return;
}

TokenType Parser::parse_binary_operator() {
    Token t = lexer.GetToken();
    if (t.token_type != PLUS && t.token_type != MINUS && t.token_type != MULT && t.token_type != DIV && t.token_type != GREATER && t.token_type != LESS && t.token_type != GTEQ && t.token_type != LTEQ && t.token_type != EQUAL && t.token_type != NOTEQUAL) {
        syntax_error();
    }
    if (t.token_type == GREATER || t.token_type == LESS || t.token_type == GTEQ || t.token_type == LTEQ || t.token_type == EQUAL || t.token_type == NOTEQUAL) {
        return BOOL;
    } else {
        return t.token_type;
    }
}

void Parser::parse_case_list() {
    parse_case();
    Token t = peek(1);
    if (t.token_type == CASE) {
        parse_case_list();
    }
    return;
}

void Parser::parse_case() {
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
    return;
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

// Expect function to get token and check for the correct type
Token Parser::expect(TokenType expected_type) {
    Token t = lexer.GetToken();

    if (t.token_type != expected_type) {
        cout << "Error on token " << t.lexeme << " on line no: " << t.line_no << ". Expected: " << expected_type << endl;
        syntax_error();
    }
    return t;
}

// Global functions
bool Parser::check_bool() {
    bool foundBool = false;
    vector<Token> tokens;
    Token t = lexer.GetToken();
    tokens.push_back(t);
    while (t.token_type != SEMICOLON) {
        if (t.token_type == GREATER || t.token_type == LESS || t.token_type == GTEQ || t.token_type == LTEQ || t.token_type == EQUAL || t.token_type == NOTEQUAL || t.token_type == NOT) {
            foundBool = true;
            break;
        }
        t = lexer.GetToken();
        tokens.push_back(t);
    }

    for (int i = tokens.size() - 1; i >= 0; i--) {
        lexer.UngetToken(tokens[i]);
    }
    return foundBool;
}

void Parser::initialize_vars() {
    vector<Token> tokens;
    Token t = lexer.GetToken();
    tokens.push_back(t);
    while (t.token_type != SEMICOLON && t.token_type != RPAREN) {
        bool found = false;
        for (int i = 0; i < allVars.size(); i++) {
            if (allVars[i]->varName == t.lexeme) {
                found = true;
            }
        }
        if (found == false && t.token_type == ID) {
            make_var(t.lexeme, UNKNOWN, false, order);
        }
        t = lexer.GetToken();
        tokens.push_back(t);
    }
    for (int i = (tokens.size() - 1); i >= 0; i--) {
        lexer.UngetToken(tokens[i]);
    }
}

int Parser::check_var_index(string variableName) {
    int exists = -1;
    for (int i = 0; i < allVars.size(); i++) {
        if (variableName == allVars[i]->varName) {
            exists = i;
            break;
        }
    }
    return exists;
}
        
// local functions
void set_eq(variable* one, variable* two) {
    if (one->varName != two->varName) {
        bool foundIt = false;
        for (int j = 0; j < one->eq.size(); j++) {
            if (one->eq[j] == two) {
                foundIt = true;
            }
        }
        if (foundIt == false) {
            one->eq.push_back(two);
            bool found;
            for (int k = 0; k < two->eq.size(); k++) {
                found = false;
                for (int i = 0; i < one->eq.size(); i++) {
                    if (two->eq[k] == one->eq[i] || two->eq[k] == one) {
                        found = true;
                    }
                }
                if (found == false) {
                    one->eq.push_back(two->eq[k]);
                }
            }
        }
        foundIt = false;
        for (int j = 0; j < two->eq.size(); j++) {
            if (two->eq[j] == one) {
                foundIt = true;
            }
        }
        if (foundIt == false) {
            two->eq.push_back(one);
            bool found;
            for (int k = 0; k < one->eq.size(); k++) {
                found = false;
                for (int i = 0; i < two->eq.size(); i++) {
                    if (one->eq[k] == two->eq[i] || one->eq[k] == two) {
                        found = true;
                    }
                }
                if (found == false) {
                    two->eq.push_back(one->eq[k]);
                }
            }
        }
    }
}

void make_var(string name, TokenType type, bool print, int ord) {
    struct variable* newVar = new variable;
    newVar->varName = name;
    newVar->varType = type;
    newVar->printed = print;
    newVar->ordered = ord;
    allVars.push_back(newVar);
    order++;
}

bool compare_eq(const variable* a, const variable* b) {
    return a->ordered < b->ordered;
}

void print() {
    // Sort equivalancies
    for (int j = 0; j < allVars.size(); j++) {
        sort(allVars[j]->eq.begin(), allVars[j]->eq.end(), compare_eq);
    }

    // Update equivalancies
    for (int j =0; j < allVars.size(); j++) {
        for (int l = 0; l < allVars[j]->eq.size(); l++) {
            allVars[j]->eq[l]->varType = allVars[j]->varType;
        }
    }

    for (int k = 0; k < allVars.size(); k++) {
        cout << "Variable " << allVars[k]->varName << " has type " << allVars[k]->varType << " order value " << allVars[k]->ordered << " and equivilancies: ";
        for (int j = 0; j < allVars[k]->eq.size(); j++) {
            cout << allVars[k]->eq[j]->varName << " ";
        }
        cout << endl;
    }

    for (int k = 0; k < allVars.size(); k++) {
        if (allVars[k]->printed != true) {
            if (allVars[k]->varType == INT) {
                cout << allVars[k]->varName << ": int #" << endl;
                allVars[k]->printed = true;
            } else if (allVars[k]->varType == REAL) {
                cout << allVars[k]->varName << ": real #" << endl;
                allVars[k]->printed = true;
            } else if (allVars[k]->varType == BOOL) {
                cout << allVars[k]->varName << ": bool #" << endl;
                allVars[k]->printed = true;
            } else {
                cout << allVars[k]->varName;
                for (int i = 0; i < allVars[k]->eq.size(); i++) {
                    if (allVars[k]->eq[i]->printed == false) {
                        cout << ", " << allVars[k]->eq[i]->varName;
                        allVars[k]->eq[i]->printed = true;
                    }
                }
                cout << ": ? #" << endl;
            }
        }
    }
}

int main() {
    Parser parser;
    parser.parse_program();
    print();
}