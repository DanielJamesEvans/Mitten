/*
 * Copyright (c) 2017 Daniel James Evans.  Licensed under the MIT License.
 */

#ifndef TOKENS_INCLUDE
#define TOKENS_INCLUDE


class token_obj {
public:
    std::string string_val;
    char char_val;
    float float_val;
    std::string type;
    bool last_token = false;
    int last_token_pos;
};


token_obj get_type(std::string token);
std::vector<token_obj> tokenize(std::string code, bool start, std::vector<token_obj> &tokens);

#endif