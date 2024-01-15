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

#include "joint_utils.h"

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



std::unordered_map<std::string, matrix> matrices_from_line(std::string line) {
    // skip past frame info
    int colon_pos = nthOccurrence(line, ":", 1);
    std::string preparsed_file = line.substr(colon_pos);

    std::regex between_brackets("\\{([^\\}]*)\\}");
    std::sregex_iterator iterator(preparsed_file.begin(), preparsed_file.end(), between_brackets);
    std::sregex_iterator end;
    std::vector<std::string> martices;    
    while (iterator != end) {
        std::smatch match = *iterator;
        martices.push_back(match[1].str());
        ++iterator;
    }

    // now you have the stored positions, get the names
    std::vector<std::string> names;
    std::regex between_space_and_colon("\\s([^\\s]+):");
    iterator = std::sregex_iterator(preparsed_file.begin(), preparsed_file.end(), between_space_and_colon);
    while (iterator != end) {
        std::smatch match = *iterator;
        names.push_back(match[1].str());
        ++iterator;
    }



    std::regex between_square_paren("\\[([^\\]]*)\\]");
    // for each matrix, split into rows, then split into cells
    std::vector<matrix> real_matrices;
    for (std::string mat : martices) {
        std::sregex_iterator row_iter  = std::sregex_iterator(mat.begin(), mat.end(), between_square_paren);
        std::vector<std::vector<float>> num_mat;
        while (row_iter != end) {
            std::smatch match = *row_iter;
            std::string row = match[1].str();
            std::stringstream floats(row);
            std::vector<float> num_row;
            float i;
            while (floats >> i) {
                num_row.push_back(i);
                if (floats.peek() == ',') 
                    floats.ignore();
            }
            num_mat.push_back(num_row);
            ++row_iter;
        }
        matrix local_matrix(num_mat);
        real_matrices.push_back(local_matrix);
    }


    std::unordered_map<std::string, matrix> joint_matrix_map;
    for (unsigned int i = 0; i < names.size(); i++) {
        std::string name = names[i];
        matrix matrix = real_matrices[i];
        joint_matrix_map[name] = matrix;
    }

    return joint_matrix_map;
}


std::tuple<bodymodel, std::vector<std::unordered_map<std::string, matrix>>> load_blaze_model_from_file(std::string filename) {
    std::ifstream file;
    std::string file_string;
    file.open(JOINT_FILEPATH + filename);
    if (!file.is_open()) {
        std::cout << JOINT_FILEPATH + filename << std::endl;
        std::cout << "ERROR: FAILED TO OPEN FILE" << std::endl;
    }

    std::getline(file, file_string);
    std::vector<position> positions = split_blaze_keypoints(file_string, true);
    bodymodel model = create_blaze_body_model();
    model.set_positions(positions);
    std::vector<std::unordered_map<std::string, matrix>> matrix_hash_list;
    while (std::getline(file, file_string)) {
        matrix_hash_list.push_back(matrices_from_line(file_string));
    }


    file.close();
    
    return {model, matrix_hash_list};
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