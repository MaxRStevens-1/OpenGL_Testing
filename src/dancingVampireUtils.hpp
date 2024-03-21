#ifndef DANCING_VAMPIRE_UTILS_HPP
#define DANCING_VAMPIRE_UTILS_HPP

#include <iostream>
#include <unordered_map>
#include <tuple> // for tuple

const unsigned int HIPS = 0;
const unsigned int SPINE = 1;
const unsigned int LEFT_UP_LEG = 2;
const unsigned int RIGHT_UP_LEG = 3;
const unsigned int SPINE_1 = 4;
const unsigned int SPINE_2 = 5;
const unsigned int LEFT_SHOULDER = 6;
const unsigned int RIGHT_SHOULDER= 7;

// LEFT ARM
const unsigned int LEFT_ARM = 8;
const unsigned int LEFT_FOREARM = 9;
const unsigned int LEFT_HAND = 10;

const unsigned int LEFT_HAND_THUMB_1 = 11;
const unsigned int LEFT_HAND_THUMB_2 = 12;
const unsigned int LEFT_HAND_THUMB_3 = 13;
const unsigned int LEFT_HAND_THUMB_4 = 14;

const unsigned int LEFT_HAND_INDEX_1 = 15;
const unsigned int LEFT_HAND_INDEX_2 = 16;
const unsigned int LEFT_HAND_INDEX_3 = 17;
const unsigned int LEFT_HAND_INDEX_4 = 18;

const unsigned int LEFT_HAND_MIDDLE_1 = 19;
const unsigned int LEFT_HAND_MIDDLE_2 = 20;
const unsigned int LEFT_HAND_MIDDLE_3 = 21;
const unsigned int LEFT_HAND_MIDDLE_4 = 22;

const unsigned int LEFT_HAND_RING_1 = 23;
const unsigned int LEFT_HAND_RING_2 = 24;
const unsigned int LEFT_HAND_RING_3 = 25;
const unsigned int LEFT_HAND_RING_4 = 26;

const unsigned int LEFT_HAND_PINKY_1 = 27;
const unsigned int LEFT_HAND_PINKY_2 = 28;
const unsigned int LEFT_HAND_PINKY_3 = 29;
const unsigned int LEFT_HAND_PINKY_4 = 30;

// RIGHT ARM
const unsigned int RIGHT_ARM = 31;
const unsigned int RIGHT_FOREARM = 32;
const unsigned int RIGHT_HAND = 33;

const unsigned int RIGHT_HAND_THUMB_1 = 34;
const unsigned int RIGHT_HAND_THUMB_2 = 35;
const unsigned int RIGHT_HAND_THUMB_3 = 36;
const unsigned int RIGHT_HAND_THUMB_4 = 37;

const unsigned int RIGHT_HAND_INDEX_1 = 38;
const unsigned int RIGHT_HAND_INDEX_2 = 39;
const unsigned int RIGHT_HAND_INDEX_3 = 40;
const unsigned int RIGHT_HAND_INDEX_4 = 41;

const unsigned int RIGHT_HAND_MIDDLE_1 = 42;
const unsigned int RIGHT_HAND_MIDDLE_2 = 43;
const unsigned int RIGHT_HAND_MIDDLE_3 = 44;
const unsigned int RIGHT_HAND_MIDDLE_4 = 45;

const unsigned int RIGHT_HAND_RING_1 = 46;
const unsigned int RIGHT_HAND_RING_2 = 47;
const unsigned int RIGHT_HAND_RING_3 = 48;
const unsigned int RIGHT_HAND_RING_4 = 49;

const unsigned int RIGHT_HAND_PINKY_1 = 50;
const unsigned int RIGHT_HAND_PINKY_2 = 51;
const unsigned int RIGHT_HAND_PINKY_3 = 52;
const unsigned int RIGHT_HAND_PINKY_4 = 53;

// left leg
const unsigned int LEFT_LEG = 54;
const unsigned int LEFT_FOOT = 55;
const unsigned int LEFT_TOE_BASE = 56;
const unsigned int LEFT_TOE_END = 57;

// right left
const unsigned int RIGHT_LEG = 58;
const unsigned int RIGHT_FOOT = 59;
const unsigned int RIGHT_TOE_BASE = 60;
const unsigned int RIGHT_TOE_END = 61;


std::unordered_map<unsigned int, std::string> hashtable_from_const() {
    std::unordered_map<unsigned int, std::string> hashtable;
    hashtable[HIPS] = "HIPS";
    hashtable[SPINE] = "SPINE";
    hashtable[LEFT_UP_LEG] = "LEFTUPLEG";
    hashtable[RIGHT_UP_LEG] = "RIGHTUPLEG";
    hashtable[SPINE_1] = "SPINE1";
    hashtable[SPINE_2] = "SPINE2";
    hashtable[LEFT_SHOULDER] = "LEFTSHOULDER";
    hashtable[RIGHT_SHOULDER] = "RIGHTSHOULDER";
    hashtable[LEFT_ARM] = "LEFTARM";
    hashtable[LEFT_FOREARM] = "LEFTFOREARM";
    hashtable[LEFT_HAND] = "LEFTHAND";
    hashtable[LEFT_HAND_THUMB_1] = "LEFTHANDTHUMB1";
    hashtable[LEFT_HAND_THUMB_2] = "LEFTHANDTHUMB2";
    hashtable[LEFT_HAND_THUMB_3] = "LEFTHANDTHUMB3";
    hashtable[LEFT_HAND_THUMB_4] = "LEFTHANDTHUMB4";
    hashtable[LEFT_HAND_INDEX_1] = "LEFTHANDINDEX1";
    hashtable[LEFT_HAND_INDEX_2] = "LEFTHANDINDEX2";
    hashtable[LEFT_HAND_INDEX_3] = "LEFTHANDINDEX3";
    hashtable[LEFT_HAND_INDEX_4] = "LEFTHANDINDEX4";
    hashtable[LEFT_HAND_MIDDLE_1] = "LEFTHANDMIDDLE1";
    hashtable[LEFT_HAND_MIDDLE_2] = "LEFTHANDMIDDLE2";
    hashtable[LEFT_HAND_MIDDLE_3] = "LEFTHANDMIDDLE3";
    hashtable[LEFT_HAND_MIDDLE_4] = "LEFTHANDMIDDLE4";
    hashtable[LEFT_HAND_RING_1] = "LEFTHANDRING1";
    hashtable[LEFT_HAND_RING_2] = "LEFTHANDRING2";
    hashtable[LEFT_HAND_RING_3] = "LEFTHANDRING3";
    hashtable[LEFT_HAND_RING_4] = "LEFTHANDRING4";
    hashtable[LEFT_HAND_PINKY_1] = "LEFTHANDPINKY1";
    hashtable[LEFT_HAND_PINKY_2] = "LEFTHANDPINKY2";
    hashtable[LEFT_HAND_PINKY_3] = "LEFTHANDPINKY3";
    hashtable[LEFT_HAND_PINKY_4] = "LEFTHANDPINKY4";
    hashtable[RIGHT_ARM] = "RIGHTARM";
    hashtable[RIGHT_FOREARM] = "RIGHTFOREARM";
    hashtable[RIGHT_HAND] = "RIGHTHAND";
    hashtable[RIGHT_HAND_THUMB_1] = "RIGHTHANDTHUMB1";
    hashtable[RIGHT_HAND_THUMB_2] = "RIGHTHANDTHUMB2";
    hashtable[RIGHT_HAND_THUMB_3] = "RIGHTHANDTHUMB3";
    hashtable[RIGHT_HAND_THUMB_4] = "RIGHTHANDTHUMB4";
    hashtable[RIGHT_HAND_INDEX_1] = "RIGHTHANDINDEX1";
    hashtable[RIGHT_HAND_INDEX_2] = "RIGHTHANDINDEX2";
    hashtable[RIGHT_HAND_INDEX_3] = "RIGHTHANDINDEX3";
    hashtable[RIGHT_HAND_INDEX_4] = "RIGHTHANDINDEX4";
    hashtable[RIGHT_HAND_MIDDLE_1] = "RIGHTHANDMIDDLE1";
    hashtable[RIGHT_HAND_MIDDLE_2] = "RIGHTHANDMIDDLE2";
    hashtable[RIGHT_HAND_MIDDLE_3] = "RIGHTHANDMIDDLE3";
    hashtable[RIGHT_HAND_MIDDLE_4] = "RIGHTHANDMIDDLE4";
    hashtable[RIGHT_HAND_RING_1] = "RIGHTHANDRING1";
    hashtable[RIGHT_HAND_RING_2] = "RIGHTHANDRING2";
    hashtable[RIGHT_HAND_RING_3] = "RIGHTHANDRING3";
    hashtable[RIGHT_HAND_RING_4] = "RIGHTHANDRING4";
    hashtable[RIGHT_HAND_PINKY_1] = "RIGHTHANDPINKY1";
    hashtable[RIGHT_HAND_PINKY_2] = "RIGHTHANDPINKY2";
    hashtable[RIGHT_HAND_PINKY_3] = "RIGHTHANDPINKY3";
    hashtable[RIGHT_HAND_PINKY_4] = "RIGHTHANDPINKY4";
    hashtable[LEFT_LEG] = "LEFTLEG";
    hashtable[LEFT_FOOT] = "LEFTFOOT";
    hashtable[LEFT_TOE_BASE] = "LEFTTOEBASE";
    hashtable[LEFT_TOE_END] = "LEFTTOE_END";
    hashtable[RIGHT_LEG] = "RIGHTLEG";
    hashtable[RIGHT_FOOT] = "RIGHTFOOT";
    hashtable[RIGHT_TOE_BASE] = "RIGHTTOEBASE";
    hashtable[RIGHT_TOE_END] = "RIGHTTOE_END";
    
    return hashtable;    
}

std::unordered_map<std::string, unsigned int> name_to_index() {
    std::unordered_map<std::string, unsigned int> reverse;
    auto hashmap = hashtable_from_const();
    for (const auto& pair : hashmap) {
        reverse[pair.second] = pair.first;
    }

    return reverse;
}

// now I have a map of blaze bones, to vamp model joint tuples
std::unordered_map<std::string, std::vector<std::tuple<int, int>>> blaze_to_vampire_map() {
    std::unordered_map<std::string, std::vector<std::tuple<int, int>>> hash;
    hash["hip_spine"] = {std::make_tuple(HIPS, SPINE_2)};

    // the left and right of the blazepose model and the 3d model are swapped :( ...
    hash["l_spine_should"] = {std::make_tuple(SPINE_2, RIGHT_ARM)};
    hash["l_should_elbow"] = {std::make_tuple(RIGHT_ARM, RIGHT_FOREARM)};
    hash["l_elbow_hand"] = {
        std::make_tuple(RIGHT_FOREARM, RIGHT_HAND),
        std::make_tuple(RIGHT_HAND, RIGHT_HAND_THUMB_4),
        std::make_tuple(RIGHT_HAND, RIGHT_HAND_INDEX_4),  
        std::make_tuple(RIGHT_HAND, RIGHT_HAND_MIDDLE_4),
        std::make_tuple(RIGHT_HAND, RIGHT_HAND_RING_4),
        std::make_tuple(RIGHT_HAND, RIGHT_HAND_PINKY_4)
    };

    hash["r_spine_should"] = {std::make_tuple(SPINE_2, LEFT_ARM)};
    hash["r_should_elbow"] = {std::make_tuple(LEFT_ARM, LEFT_FOREARM)};
    hash["r_elbow_hand"] = {
        std::make_tuple(LEFT_FOREARM, LEFT_HAND),
        std::make_tuple(LEFT_HAND, LEFT_HAND_THUMB_4),
        std::make_tuple(LEFT_HAND, LEFT_HAND_INDEX_4),  
        std::make_tuple(LEFT_HAND, LEFT_HAND_MIDDLE_4),
        std::make_tuple(LEFT_HAND, LEFT_HAND_RING_4),
        std::make_tuple(LEFT_HAND, LEFT_HAND_PINKY_4)
    };

    hash["l_hip_knee"] = {std::make_tuple(HIPS, RIGHT_UP_LEG)};
    hash["l_knee_foot"] = {std::make_tuple(RIGHT_UP_LEG, RIGHT_LEG)};

    hash["r_hip_knee"] = {std::make_tuple(HIPS, LEFT_UP_LEG)};
    hash["r_knee_foot"] = {std::make_tuple(LEFT_UP_LEG, LEFT_LEG)};

    return hash;
}



#endif