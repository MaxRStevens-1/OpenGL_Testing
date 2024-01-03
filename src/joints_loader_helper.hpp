#ifndef JOINTS_LOADER_HELPER_HPP
#define JOINTS_LOADER_HELPER_HPP

#include <iostream>
#include <vector>
#include <cstring>
#include <cmath>
#include <chrono>
#include <fstream>
#include <regex>
#include <sstream>

const std::string JOINT_FILEPATH = "./joints_output/";

// largely taken from stack overflow
bool isFloat(std::string myString) {
    std::istringstream iss(myString);
    float f;
    iss >> std::noskipws >> f; // noskipws considers leading whitespace invalid
    // Check the entire string was consumed and if either failbit or badbit is set
    return iss.eof() && !iss.fail(); 
}
// largely taken form setack overflow
int nthOccurrence(const std::string& str, const std::string& findMe, int nth) {
    size_t  pos = -1;
    int     cnt = 0;
    while( cnt != nth ) {
        pos+=1;
        pos = str.find(findMe, pos);
        if (pos == std::string::npos) {
            return -1;
        }
        cnt++;
    }
    return pos;
}

inline bool is_valid_char(char ch) {
    return ('0' <= ch && ch <= '9') || ch == '.' || ch == ' ' || ch == ',';
}

inline bool is_valid_float(char ch) {
    return ('0' <= ch && ch <= '9') || ch == '.';
}
// base code largely taken from stackoverflow
std::string remove_invalid_chars(const std::string& input) {
    std::string result;
    std::copy_if(input.begin(), input.end(),
        std::back_inserter(result),
        is_valid_char);
    return result;
}

std::string remove_invalid_char_in_floats(const std::string& input) {
    std::string result;
    std::copy_if(input.begin(), input.end(),
        std::back_inserter(result),
        is_valid_float);
    return result;
}


std::vector<float> load_joints(std::string filename) {
    std::ifstream file;
    file.open(JOINT_FILEPATH + filename);
    std::string file_string;
    if (!file.is_open()) {
        std::cout << "ERROR: FAILED TO OPEN FILE" << std::endl;
        return {};
    }
    std::getline(file, file_string);    
    // now find location of second :
    int colon_pos = nthOccurrence(file_string, ":", 2);
    // removes frame #
    std::string preparsed_file = file_string.substr(colon_pos);
    // makes string a just numbers seperated by ,
    std::string parsed_line = remove_invalid_chars(preparsed_file);
    // parsed_line = remove_invalid_char_in_floats(parsed_line);
    std::regex reg("\\s+");
    std::sregex_token_iterator iter(parsed_line.begin(), parsed_line.end(), reg, -1);
    std::sregex_token_iterator end;
    std::vector<std::string> tokenized_pos(iter, end);
    


    std::vector<float> positions;
    for (std::string token : tokenized_pos) {
        std::string parsed_token = remove_invalid_char_in_floats(token);
        if (isFloat(parsed_token)) 
            positions.push_back(std::stof(parsed_token));
    }

    // tokenized_pos.
    file.close();
    return positions;
}

#endif