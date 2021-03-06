
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

//int a = 1;


int get_prec(token_obj token) {
    map <string, int> prec_map;
    prec_map["||"] = 1;
    prec_map["&&"] = 2;
    prec_map["<="] = 3;
    prec_map["<"] = 3;
    prec_map[">="] = 3;
    prec_map[">"] = 3;
    prec_map["+"] = 4;
    prec_map["-"] = 4;
    prec_map["*"] = 5;
    prec_map["/"] = 5;
    prec_map["^"] = 6;
    return prec_map[token.string_val];
}


char get_assoc(token_obj token) {
    map <string, int> assoc_map;
    assoc_map["+"] = 'l';
    assoc_map["-"] = 'l';
    assoc_map["*"] = 'l';
    assoc_map["/"] = 'l';
    assoc_map["||"] = 'l';
    assoc_map["&&"] = 'l';
    assoc_map["<="] = 'l';
    assoc_map["<"] = 'l';
    assoc_map[">="] = 'l';
    assoc_map[">"] = 'l';
    assoc_map["^"] = 'r';
    return assoc_map[token.string_val];
}



node* get_pointer(address an_address, node_vectors &vectors) {
    if (an_address.type == "string") {
        return &vectors.strings[an_address.loc];
    }
    else if (an_address.type == "float") {
        return &vectors.flts[an_address.loc];
    }
    else if (an_address.type == "var") {
        return &vectors.vars[an_address.loc];
    }
    else if (an_address.type == "binop") {
        return &vectors.binops[an_address.loc];
    }
    else if (an_address.type == "program") {
        return &vectors.programs[an_address.loc];
    }
    else if (an_address.type == "while") {
        return &vectors.whiles[an_address.loc];
    }
    else if (an_address.type == "assignment") {
        return &vectors.assignments[an_address.loc];
    }
    else if (an_address.type == "print node") {
        return &vectors.prints[an_address.loc];
    }
    else {
        cout << "ERROR IN GET POINTER " << an_address.type << endl;
        exit(1);
    }
}

address compute_op(address op_address, address left_address, address right_address, node_vectors &vectors) {
    
    node* op_ptr = get_pointer(op_address, vectors);
    
    op_ptr->addresses.push_back(left_address);
    op_ptr->addresses.push_back(right_address);
    return op_address;
}


//Given token, adds node to correct type_node vector.
address add_node(token_obj token, node_vectors &vectors) {
    if (token.type == "var") {
        var_name_node new_node;
        new_node.val = token.string_val;
        new_node.own_address.type = "var";
        new_node.own_address.loc=vectors.vars.size();
        vectors.vars.push_back(new_node);
        return new_node.own_address;
    }
    else if (token.type == "float") {
        flt_node new_node;
        new_node.val = token.float_val;
        new_node.own_address.type = "float";
        new_node.own_address.loc = vectors.flts.size();
        vectors.flts.push_back(new_node);
        return new_node.own_address;
    }
    else if (token.type == "binop") {
        binop_node new_node;
        new_node.val = token.string_val;
        new_node.own_address.type = "binop";
        new_node.own_address.loc = vectors.binops.size();
        vectors.binops.push_back(new_node);
        return new_node.own_address;
    }
    else {
        cout << "ERROR: Node type " << token.type << " not supported." << endl;
        exit(1);
    }
}


int get_closing_paren(vector<token_obj> &tokens, node_vectors &vectors, int i) {
    int j = i + 1;
    int paren_count = 1;
    while (j < tokens.size()) {
        j++;
        token_obj next_token = tokens[j];
        if (next_token.type == "paren") {
            if (next_token.string_val == "(") {
                paren_count++;
            }
            else {
                paren_count--;
            }
            if (paren_count == 0) {
                break;
            }
        }
    }
    j = j - i;
    return j;
}



address parse(int min_prec, vector<token_obj> &tokens, node_vectors &vectors, int &i, bool &end) {
    // Returns address of top node of expression.  Returned address hasn't been
    // added to a vector yet, but all nodes (including top node) that are created by
    //this function are added to vectors.
    token_obj result_token = tokens[i - 1];
    int result;
    
    
    //Check that first token (result) is of correct type
    address result_node_address;
    if (result_token.type == "var" || result_token.type == "float") {
        result_node_address = add_node(result_token, vectors);
    }
    else if (result_token.type == "paren") {
        int paren_size = get_closing_paren(tokens, vectors, i);
        vector<token_obj>::const_iterator first = tokens.begin() + i;
        vector<token_obj>::const_iterator last = tokens.begin() + i + paren_size;
        vector<token_obj> paren_exp(first, last);
        bool paren_end = false;
        int paren_i = 1;
        result_node_address = parse(1, paren_exp, vectors, paren_i, paren_end);
        i = i + paren_size + 1;
        if (i >= tokens.size()) {
            return result_node_address;
        }
    }
    else {
        cout << "ERROR!!!!!!!  Invalid token in expression: type " << result_token.type << " and val " << result_token.string_val << endl;
        exit(1);
    }
    if (tokens.size() == 1) {
        return result_node_address;
    }
    token_obj next_token = tokens[i];
    
    
    
    //Check that first operator (next_token) is of correct type
    address next_token_address;
    if (next_token.type == "binop") {
        next_token_address = add_node(next_token, vectors);
    }
    else {
        cout << "ERROR in expression parsing: Expected binop, got type " << next_token.type << endl;
        exit(1);
    }
    
    int prec = get_prec(next_token);
    
    while (prec >= min_prec and end == false) {
        char next_assoc = get_assoc(next_token);
        if (next_assoc == 'l') {
            prec++;
        }
        i += 2;
        address right_hand_address;
        if (i < tokens.size()) {
            right_hand_address = parse(prec, tokens, vectors, i, end);
        }
        else {
            if (tokens[tokens.size() - 1].last_token == false) {
                node* last_node_ptr;
                if (tokens[tokens.size() - 1].type == "var") {
                    var_name_node last_node;
                    last_node_ptr = &last_node;
                }
                else if (tokens[tokens.size() - 1].type == "float") {
                    flt_node last_node;
                    last_node_ptr = &last_node;
                }
                else {
                    node last_node;
                    last_node.correct_node = false;
                    last_node_ptr = &last_node;
                }
                
                tokens[tokens.size() - 1].last_token = true;
                right_hand_address = add_node(tokens[tokens.size() - 1], vectors);
            }
        }
        result_node_address = compute_op(next_token_address, result_node_address, right_hand_address, vectors);
        if (i >= tokens.size()) {
            return result_node_address;
        }
        next_token = tokens[i];
        prec = get_prec(next_token);
        if (next_token.type == "binop") {
            next_token_address = add_node(next_token, vectors);
        }
        
        else {
            cout << "ERROR!!!!!!!  Invalid token at start of expression: type " << result_token.type << endl;
            exit(1);
        }
    }
    
    return result_node_address;
}