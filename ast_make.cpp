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


string remove_indent(string line){
    int i = 0;
    string unindented_line;
    //Get first non-space char
    while (i < line.size()) {
        if (line[i] != ' ') {
            unindented_line += line[i];
            break;
        }
        else {
            i++;
        }
    }
    unindented_line += line.substr(i + 1, line.size() - i - 1);
    return unindented_line;
}

bool is_binop(string a_string) {
    if (a_string == "+" || a_string == "-" || a_string == "*" || a_string == "/"
        || a_string == "<=" || a_string == ">=" || a_string == "&&" ||
        a_string == "||" || a_string == "<" || a_string == ">") {
        return true;
    }
    else {
        return false;
    }
}

var_name_node find_var_name(string line, node_vectors &vectors) {
    string name = "";
    var_name_node new_node;
    new_node.own_address.type = "var";
    if (isalpha(line[0]) == false) {
        new_node.correct_node = false;
        cout << "ERROR: varname not found; first char is: " << line[0] << endl;
        return new_node;
    }
    for (int i = 0; i < line.length(); i++) {
        if (isalpha(line[i]) or  isdigit(line[i]) or line[i] == '_') {
            name += line[i];
        }
        else if (line[i] == ' ' || is_binop(string(1,line[i])) || line[i] == '=') {
            break;
        }
        else {
            cout << "ERROR: BAD VAR_NAME.  char is: " << line[i] << endl;
            new_node.correct_node = false;
            return new_node;
        }
    }
    new_node.val = name;
    return new_node;
}


assignment_node find_assignment(string line, node_vectors &vectors) {
    assignment_node new_node;
    new_node.own_address.type = "assignment";
    var_name_node new_name;
    new_name = find_var_name(line, vectors);
    if (new_name.correct_node == true) {
        new_name.own_address.loc = vectors.vars.size();
        new_name.own_address.type = "var";
        vectors.vars.push_back(new_name);
        new_node.addresses.push_back(new_name.own_address);
    }
    else {
        new_node.correct_node = false;
        cout << "ERROR: No var_name found in line: " << line << endl;
        exit(1);
    }
    
    
    
    //Parse assignment operator
    int i = 0;
    if (line.substr(0, 4) == "str " || line.substr(0, 4) == "flt ") {
        i += 4;
    }
    i += new_name.get_string_val().length();
    for (i; i < line.length(); i++) {
        if (line[i] == '=') {
            break;
        }
        else if (line[i] != ' ') {
            cout << "ERROR+ n- \"=\" found in right place.  other char is: " << line[i] << " in line " << line << endl;
            new_node.correct_node = false;
            break;
        }
    }
    new_node.own_address.loc = vectors.assignments.size();
    vector<token_obj> tokens;
    tokens = tokenize(line.substr(i + 1), true, tokens);
    int parse_i = 1;
    bool end = false;
    address exp_address;
    exp_address = parse(1, tokens, vectors, parse_i, end);
    new_node.addresses.push_back(exp_address);
    vectors.assignments.push_back(new_node);
    return new_node;
}


while_node find_while(string line, node_vectors &vectors) {
    while_node new_while;
    if (line.substr(0,6) != "while{" || line.substr(line.length()-2, 2) != "}{") {
        new_while.correct_node = false;
        return new_while;
    }
    else {
        string cond = line.substr(6, line.length() - 2 - 6);
        vector<token_obj> tokens;
        tokens = tokenize(cond, true, tokens);
        int parse_i = 1;
        bool parse_end =  false;
        address exp_address = parse(1, tokens, vectors, parse_i, parse_end);
        new_while.addresses.push_back(exp_address);
    }
    return new_while;
}

void find_statement(string line, node_vectors &vectors, address &parent_address) {
    if (line == "}") {
        node* parent_pointer = get_pointer(parent_address, vectors);
        parent_address = parent_pointer->get_parent();
        return;
    }
    else if (line.substr(0,2) == "//") {
        return;
    }
    if (line.substr(0, 6) == "print ") {
        print_node new_print_node;
        if (line[6] == '\"') {
            string_node new_string_node;
            new_string_node.val = line.substr(7, line.size() - 8);
            new_string_node.own_address.type = "string";
            new_string_node.own_address.loc = vectors.strings.size();
            vectors.strings.push_back(new_string_node);
            new_print_node.addresses.push_back(new_string_node.own_address);
            new_print_node.own_address.loc = vectors.prints.size();
            new_print_node.own_address.type = "print node";
            vectors.prints.push_back(new_print_node);
            node* parent_pointer = get_pointer(parent_address, vectors);
            parent_pointer->addresses.push_back(new_print_node.own_address);
        }
        else {
            string exp = line.substr(6, line.size() - 6);
            vector<token_obj> tokens;
            tokens = tokenize(exp, true, tokens);
            int i = 1;
            bool end = false;
            address exp_address = parse(1, tokens, vectors, i, end);
            new_print_node.addresses.push_back(exp_address);
            new_print_node.own_address.loc = vectors.prints.size();
            new_print_node.own_address.type = "print node";
            vectors.prints.push_back(new_print_node);
            node* parent_pointer = get_pointer(parent_address, vectors);
            parent_pointer->addresses.push_back(new_print_node.own_address);
        }
    }
    else if (line.substr(0, 4) != "for " && line.substr(0, 6) != "while{") {
        assignment_node new_assignment;
        new_assignment = find_assignment(line, vectors);
        node* parent_pointer = get_pointer(parent_address, vectors);
        parent_pointer->addresses.push_back(new_assignment.own_address);
        
    }
    else {
        while_node new_while = find_while(line, vectors);
        new_while.own_address.loc = vectors.whiles.size();
        new_while.own_address.type = "while";
        new_while.parent_address = parent_address;
        vectors.whiles.push_back(new_while);
        node* parent_pointer = get_pointer(parent_address, vectors);
        parent_pointer->addresses.push_back(new_while.own_address);
        parent_address = new_while.own_address;
        return;
    }
    
    node* parent_pointer = get_pointer(parent_address, vectors);
    return;
}







int main(int argc, char* argv[]) {
    ifstream code_file(argv[1]);
    string line;
    node_vectors vectors;
    program_node new_program;
    new_program.own_address.loc = 0;
    new_program.own_address.type = "program";
    vectors.programs.push_back(new_program);
    address parent_address = new_program.own_address;
    while (getline(code_file, line)) {
        line = remove_indent(line);
        find_statement(line, vectors, parent_address);
        //find_(line, vectors);
    };
    environment en;
    visitor(new_program.own_address, vectors, en);
    map<string, string> environment;
    return 0;
}