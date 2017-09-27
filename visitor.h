/*
 * Copyright (c) 2017 Daniel James Evans.  Licensed under the MIT License.
 */

#ifndef VISITOR_INCLUDE
#define VISITOR_INCLUDE


class environment{
public:
    std::map<std::string, std::string> types;
    std::map<std::string, float> floats;
    address parent_address;
    
    void update_environment(std::string var_name, float var_val) {
        if (types.find(var_name) == types.end()) {
            types.insert(std::pair<std::string, std::string>(var_name, "float"));
            floats.insert(std::pair<std::string, float>(var_name, var_val));
        }
        else if (types[var_name] != "float") {
            std::cout << "ERROR: var_name is not supported type!" << std::endl;
            exit(1);
        }
        else {
            floats[var_name] = var_val;
            return;

        }
    }
    
    int get_val(std::string var_name) {
        if (types.find(var_name) == types.end()) {
            std::cout << "ERROR: var_name " << var_name << " doesn't exist!";
            exit(1);
        }
        else if (types[var_name] != "float") {
            std::cout << "ERROR: var_name is not supported type!" << std::endl;
            exit(1);
        }
        else {
            return floats[var_name];
        }
    }

    std::string get_type(std::string var_name) {
        if (types.find(var_name) == types.end()) {
            std::cout << "ERROR: var_name " << var_name << " doesn't exist!";
            exit(1);
        }
        else {
            return types[var_name];
        }
    }
    
    std::string get_type() {
        return "while loop";
    }
    address get_parent() {
        return parent_address;
    }
};

float visitor(address parent_address, node_vectors vectors, environment &en);


#endif