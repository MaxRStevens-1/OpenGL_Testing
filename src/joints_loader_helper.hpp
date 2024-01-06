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

// taken from github
template <typename T>
std::vector<T> flatten(const std::vector<std::vector<T>>& v) {
    std::size_t total_size = 0;
    for (const auto& sub : v)
        total_size += sub.size(); // I wish there was a transform_accumulate
    std::vector<T> result;
    result.reserve(total_size);
    for (const auto& sub : v)
        result.insert(result.end(), sub.begin(), sub.end());
    return result;
}


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
    return ('0' <= ch && ch <= '9') || ch == '.' || ch == ' ' || ch == ',' || ch == '-';
}

inline bool is_valid_float(char ch) {
    return ('0' <= ch && ch <= '9') || ch == '.' || ch == '-';
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


std::vector<float> load_joints_single_line(std::string filename) {
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
    // need to flip y values
    unsigned int index = 0;
    for (std::string token : tokenized_pos) {
        std::string parsed_token = remove_invalid_char_in_floats(token);
        if (isFloat(parsed_token)) {
            if (index % 3 == 1) {
                positions.push_back(std::stof(parsed_token)*-1);
                std::cout << "changing sign from " << std::stof(parsed_token) << " to " << std::stof(parsed_token)*-1 << std::endl;
            }
            else
                positions.push_back(std::stof(parsed_token));
            index++;
        } 
    }

    file.close();
    return positions;
}



std::vector<std::vector<float>> load_joints_all_lines(std::string filename) {
    std::ifstream file;
    file.open(JOINT_FILEPATH + filename);
    std::string file_string;
    if (!file.is_open()) {
        std::cout << "ERROR: FAILED TO OPEN FILE" << std::endl;
        return {};
    }
    std::vector<std::vector<float>> all_positions;

    while (std::getline(file, file_string)) {
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
        unsigned int index = 0;
        for (std::string token : tokenized_pos) {
            std::string parsed_token = remove_invalid_char_in_floats(token);
            if (isFloat(parsed_token)) {
                if (index % 3 == 1) {
                    positions.push_back(std::stof(parsed_token)*-1);
                } else
                    positions.push_back(std::stof(parsed_token));
                index++;
            }
        }
        all_positions.push_back(positions);

    }
    // tokenized_pos.
    file.close();

    return all_positions;
}

// largely taken to stack over flow
std::string replace_all(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::vector<std::vector<float>> load_joints_all_lines_flat(std::string filename) {
    std::ifstream file;
    file.open(JOINT_FILEPATH + filename);
    std::string file_string;
    if (!file.is_open()) {
        std::cout << "ERROR: FAILED TO OPEN FILE" << std::endl;
        return {};
    }
    std::vector<std::vector<float>> all_positions;

    while (std::getline(file, file_string)) {
        // now find location of second :
        int bracket_pos = nthOccurrence(file_string, "[", 1);
        // removes frame #
        std::string preparsed_file = file_string.substr(bracket_pos);
        std::cout << "file is: " << preparsed_file << std::endl;

        // makes string a just numbers seperated by ,
        std::string parsed_line = remove_invalid_chars(preparsed_file);
        std::regex reg("\\s+");
        std::sregex_token_iterator iter(parsed_line.begin(), parsed_line.end(), reg, -1);
        std::sregex_token_iterator end;
        std::vector<std::string> tokenized_pos(iter, end);
        std::vector<float> positions;
        unsigned int index = 0;
        for (std::string token : tokenized_pos) {
            std::string parsed_token = remove_invalid_char_in_floats(token);
            if (isFloat(parsed_token)) {
                if (index % 3 == 1) {
                    positions.push_back(std::stof(parsed_token)*-1);
                } else
                    positions.push_back(std::stof(parsed_token));
                index++;
            }
        }
        std::cout << "positions size is " << positions.size() << std::endl;
        all_positions.push_back(positions);

    }
    // tokenized_pos.
    file.close();

    return all_positions;
}

#endif