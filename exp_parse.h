/*
 * Copyright (c) 2017 Daniel James Evans.  Licensed under the MIT License.
 */

#ifndef EXP_PARSE_INCLUDE
#define EXP_PARSE_INCLUDE


class address {
public:
    std::string type = "base type";
    int loc;
};


class node {
public:
    address own_address;
    std::vector<address> addresses;
    bool correct_node = true;
    virtual std::string get_string_val() {
        return "base_object has no val!";
    }
    virtual float get_float_val() {
        return 0;
    }
    virtual std::string get_type() {
        return "base_object has no type";
    }
    virtual address get_parent() {
        address empty_address;
        empty_address.type = "base_object has no type";
        empty_address.loc = -1;
        return empty_address;
    }
};

class str_node: public node {
public:
    std::string val;
    std::string get_string_val() {
        return val;
    }
    std::string get_type() {
        return "string";
    }
};
class flt_node: public node {
public:
    float val;
    float get_float_val() {
        return val;
    }
    std::string get_type() {
        return "float";
    }
};

class binop_node: public node {
public:
    std::string val;
    std::string get_string_val() {
        return val;
    }
    std::string get_type() {
        return "binary operator";
    }
};

class var_name_node: public node {
public:
    std::string val;
    std::string get_string_val() {
        return val;
    }
    std::string get_type() {
        return "variable";
    }
};

class assignment_node: public node {
public:
    address parent_address;
    std::string get_val() {
        return "=";
    }
    std::string get_type() {
        return "assignment";
    }
    address get_parent() {
        return parent_address;
    }
};



class program_node: public node {
public:
    std::string get_string_val() {
        return "program";
    }
    std::string get_type() {
        return "program";
    }
};


class while_node: public node {
public:
    address parent_address;
    std::string get_string_val() {
        return "while loop";
    }
    std::string get_type() {
        return "while loop";
    }
    address get_parent() {
        return parent_address;
    }
};

class print_node: public node {
public:
    address parent_address;
    std::string get_string_val() {
        return "print node";
    }
    std::string get_type() {
        return "print node";
    }
    address get_parent() {
        return parent_address;
    }
};

class string_node: public node {
public:
    address parent_address;
    std::string val;
    std::string get_string_val() {
        return val;
    }
    std::string get_type() {
        return "string";
    }
    address get_parent() {
        return parent_address;
    }
};


class node_vectors {
public:
    std::vector<flt_node> flts;
    std::vector<binop_node> binops;
    std::vector<var_name_node> vars;
    std::vector<assignment_node> assignments;
    std::vector<while_node> whiles;
    std::vector<program_node> programs;
    std::vector<print_node> prints;
    std::vector<string_node> strings;
};

node* get_pointer(address an_address, node_vectors &vectors);
address parse(int min_prec, std::vector<token_obj> &tokens, node_vectors &vectors, int &i, bool &end);



#endif