/*
 * Copyright (c) 2017 Daniel James Evans.  Licensed under the MIT License.
 */

#include <iostream>
#include <cstring>
#include <map>
#include <fstream>
#include <vector>
#include <math.h>


#include "tokens.h"
#include "exp_parse.h"
#include "ast_make.h"
#include "visitor.h"


using namespace std;


token_obj get_type(string token) {
    bool can_be_float = false;
    bool can_be_var = false;
    bool is_paren = false;
    bool token_is_binop = false;
    for (int i = 0; i < token.length(); i++) {
        if (isdigit(token[i]) || token[i] == '+' || token[i] == '-' || token[i] == '.') {
            can_be_float = true;
        }
        else {
            cout << "err" << token << endl;
            can_be_float = false;
            break;
        }
    }
    if (isalpha(token[0])) {
        for (int i = 0; i < token.length(); i++) {
            if (isalpha(token[i]) || token[i] == '_') {
                can_be_var = true;
            }
            else {
                can_be_var = false;
                break;
            }
        }
    }
    if (token.length() == 1) {
        if (token == "(" || token == ")") {
            is_paren = true;
        }
        if (is_binop(token) == true) {
            token_is_binop = true;
        }
        if (isalpha(token[0]) == true) {
            can_be_var = true;
        }
    }
    token_obj new_token;
    new_token.type = "error";
    if (can_be_float) {
        new_token.type = "float";
        new_token.float_val = stof(token);
    }
    if (can_be_var) {
        new_token.type = "var";
        new_token.string_val = token;
    }
    if (is_paren) {
        new_token.type = "paren";
        new_token.string_val = token;
    }
    if (token_is_binop) {
        new_token.type = "binop";
        new_token.string_val = token;
    }
    return new_token;
}


vector<token_obj> tokenize(string code, bool start, vector<token_obj> &tokens) {
    bool cont = true;
    bool var = false;
    bool binop = false;
    bool end_of_num_or_var_binop = false;
    bool paren_next = false;
    int i = 0;
    string next_token = "";
    while (cont == true) {
        char next_char = code[i];
        if (next_char != ' ') {
            //If the char can be part of a float, add it to the token.
            if (isdigit(next_char) || next_char == '.') {
                next_token += next_char;
                start = false;
            }
            //If the char is a + or - used as a unary operator, add it to the token.
            else if ((next_char == '+' || next_char == '-') &&
                     i < code.length() - 1 &&
                     (isdigit(code[i + 1]) || code[i + 1] == '.') && start == true) {
                next_token += next_char;
                start = false;
            }
            else if (next_char == '(' || next_char == ')') {
                next_token += next_char;
                token_obj new_paren;
                new_paren.string_val = string(1, next_char);
                new_paren.type = "paren";
                tokens.push_back(new_paren);
                if (next_char == '(') {
                    start = true;
                }
                var = false;
            }
            //If the char is part of a variable name, add it to the token.
            else if (isalpha(next_char) || (next_char == '_' && var == true)) {
                next_token += next_char;
                var = true;
            }
            //If none of the other conditions apply, assume char is binary operator.
            else {
                next_token += next_char;
                start = false;
                if (is_binop(next_token) || next_token == "&" || next_token == "|") {
                    binop = true;
                }
            }
            //If the char isn't a binary op or parenthesis and the next char can't be
            //float, end token.  The "" was added because of some weird behavior with " ("
            if ((next_char == '(' || next_char == ')')) {
                paren_next = true;
            }
            else {
                paren_next = false;
            }
            if (i == code.length() - 1 || (!isdigit(code[i + 1]) && code[i + 1] != '.' && var == false  && binop == false)
                || (var == true && !isalpha(code[i + 1]) && !isdigit(code[i + 1]) && code[i+1] != '_') ||
                (binop == true && !is_binop(next_token + code[i+1]))){
                end_of_num_or_var_binop = true;
            }
            else {
                end_of_num_or_var_binop = false;
            }
            if ((paren_next || end_of_num_or_var_binop) && next_token != "") {
                if (i < code.length() - 1) { //Because if at end, token added later.
                    if (var == true) {
                        token_obj new_token_obj;
                        new_token_obj.string_val = next_token;
                        new_token_obj.type = "var";
                        tokens.push_back(new_token_obj);
                        next_token = "";
                        var = false;
                    }
                    else if (binop == true) {
                        token_obj new_token_obj;
                        new_token_obj.string_val = next_token;
                        new_token_obj.type = "binop";
                        tokens.push_back(new_token_obj);
                        next_token = "";
                        binop = false;
                    }
                    else if (paren_next == true) {
                        next_token = "";
                        paren_next = false;
                    }
                    else {
                        token_obj new_token_obj;
                        try {
                            new_token_obj.float_val = stof(next_token);
                            new_token_obj.type = "float";
                            tokens.push_back(new_token_obj);
                            next_token = "";
                            var = false;
                        }
                        catch (std::invalid_argument) {
                            cerr << "ERROR: Invalid syntax in token " << next_token <<
                            " in expression " << code << endl;
                            exit(1);
                        }
                    }
                    
                }
            }
        }
        i++;
        if (i >= code.length()) {
            cont = false;
        }
    }
    if (var == true) {
        token_obj new_token_obj;
        new_token_obj.type = "var";
        new_token_obj.string_val = next_token;
        tokens.push_back(new_token_obj);
        return tokens;
    }
    else {
        token_obj new_token_obj;
        if (next_token == ")") {
            new_token_obj.string_val = next_token;
            new_token_obj.type = "paren";
            tokens.push_back(new_token_obj);
            return tokens;
        }
        //If token isn't ")" and var != true, token should be a float.
        try {
            new_token_obj.float_val = stof(next_token);
            new_token_obj.type = "float";
            tokens.push_back(new_token_obj);
            return tokens;
        }
        catch (std::invalid_argument) {
            cerr << "ERROR: Invalid syntax in token " << next_token <<
            " in expression " << code << endl;
            exit(1);
        }
        
    }
}