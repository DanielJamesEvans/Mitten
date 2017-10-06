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


float visitor(address parent_address, node_vectors vectors, environment &en) {
    node* parent_pointer = get_pointer(parent_address, vectors);
    string parent_type = parent_pointer->own_address.type;
    //cout << "Parent type: " << parent_type << endl;
    if (parent_type == "assignment") {
        address var_name_address = parent_pointer->addresses[0];
        node* var_name_ptr = get_pointer(var_name_address, vectors);
        string var_name_val = var_name_ptr->get_string_val();
        float right_side = visitor(parent_pointer->addresses[1], vectors, en);
        en.update_environment(var_name_val, right_side);
    }
    else if(parent_type == "while") {
        address cond_address = parent_pointer->addresses[0];
        while (visitor(cond_address, vectors, en)) {
            for (int i = 0; i < parent_pointer->addresses.size(); i++) {
                address next_address = parent_pointer->addresses[i];
                node* in_loop_ptr = get_pointer(next_address, vectors);
                visitor(parent_pointer->addresses[i], vectors, en);
            }
        }
    }
    else if(parent_type == "binop") {
        address left_address = parent_pointer->addresses[0];
        node* left_node_ptr = get_pointer(left_address, vectors);
        string left_type = left_node_ptr->get_type();
        address right_address = parent_pointer->addresses[1];
        node* right_node_ptr = get_pointer(right_address, vectors);
        string right_type = right_node_ptr->get_type();
        if ((left_type != "binary operator" && left_type != "float" && left_type != "variable") || (right_type != "binary operator" && right_type != "float" && right_type != "variable")) {
            cout << "ERROR: types " << left_type << " and " << right_type << " cannot be passed to binary operator." << endl;
            exit(1);
        }
        float left_val = visitor(left_address, vectors, en);
        float right_val = visitor(right_address, vectors, en);
        string binop = parent_pointer->get_string_val();
        
        if (binop == "+"){
            return left_val + right_val;
        }
        else if (binop == "-"){
            return left_val - right_val;
        }
        else if (binop == "*"){
            return left_val * right_val;
        }
        else if (binop == "/"){
            return left_val / right_val;
        }
        else if (binop == "^"){
            return pow(left_val, right_val);
        }
        else if (binop == "<="){
            return left_val <= right_val;
        }
        else if (binop == ">="){
            return left_val >= right_val;
        }
        else if (binop == "&&"){
            return left_val && right_val;
        }
        else if (binop == "||"){
            return left_val || right_val;
        }
        else if (binop == "<"){
            return left_val < right_val;
        }
        else if (binop == "<="){
            return left_val > right_val;
        }
        else {
            cout << "ERROR: unsupported binary operator " << binop << endl;
            exit(1);
        }
    }
    else if(parent_type == "float") {
        return parent_pointer->get_float_val();
    }
    else if(parent_type == "program") {
        for (int i = 0; i < parent_pointer->addresses.size(); i++) {
            visitor(parent_pointer->addresses[i], vectors, en);
        }
    }
    else if(parent_type == "print node") {
        string print_type = parent_pointer->addresses[0].type;
        if (print_type == "string") {
            node* printed_node_ptr = get_pointer(parent_pointer->addresses[0], vectors);
            string print_this = printed_node_ptr->get_string_val();
            cout << print_this << endl;
        }
        else {
            
            cout << visitor(parent_pointer->addresses[0], vectors, en) << endl;
        }
    }
    else if(parent_type == "var") {
        return en.get_val(parent_pointer->get_string_val());
    }
    return 0;
}