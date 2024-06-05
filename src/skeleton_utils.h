#ifndef SKELETON_UTILS_H
#define SKELETON_UTILS_H

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <deque>
#include <cmath>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dancingVampireUtils.hpp"


struct position {
    float x;
    float y;
    float z;

    /**
     * @brief Construct a new position object, empty constructor
     * 
     */
    position () {

    }

    /**
     * @brief Construct a new position object, given xyz
     * 
     * @param ix 
     * @param iy 
     * @param iz 
     */
    position (float ix, float iy, float iz) {
        x = ix;
        y = iy;
        z = iz;
    }

    std::string toString () {
        return "[" + std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + "]"; 
    }

    position add (position p) {
        return position (x + p.x, y + p.y, z + p.z);
    } 

    position subtract (position p) {
        return position (x - p.x, y - p.y, z - p.z);
    }

    position scale (float f) {
        return position (x*f, y*f, z*f);
    }

    float dot (position p) {
        return x*p.x + y*p.y + z*p.z;
    }


    position cross (position p) {
        float cross_x = y*p.z - z*p.y;
        float cross_y = z*p.x - x*p.z;
        float cross_z = x*p.y - y*p.x;

        return position (cross_x, cross_y, cross_z);
    }

    float magnitude () {
        return std::sqrt(x*x + y*y + z*z);
    }

    position normalize() {
        float mag = magnitude();
        if (mag == 0) 
            return position(0,0,0);
        
        return position (
            x/mag, y/mag, z/mag
        );
    }

    glm::vec4 to_vec4() {
        float temp[4] = {
            x, y, z, 1
        };
        return glm::make_vec4(temp);
    }

    std::vector<float> to_vector() {
        return {x, y, z};
    }

    glm::mat4 convert_to_translation_mat() {
        glm::mat4 trans =  glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        return trans;
    }
};

struct matrix {
    std::vector<std::vector<float>> mat;
    matrix(std::vector<std::vector<float>> imat) {
        mat = imat;
    }

    matrix() {

    }

    matrix dot (matrix in) {
        // mat a
        float a = mat[0][0];
        float b = mat[0][1];
        float c = mat[0][2];
        float d = mat[1][0];
        float e = mat[1][1];
        float f = mat[1][2];
        float g = mat[2][0];
        float h = mat[2][1];
        float i = mat[2][2];

        // mat b
        float j = in.mat[0][0];
        float k = in.mat[0][1];
        float l = in.mat[0][2];
        float m = in.mat[1][0];
        float n = in.mat[1][1];
        float o = in.mat[1][2];
        float p = in.mat[2][0];
        float q = in.mat[2][1];
        float r = in.mat[2][2];


        std::vector<float> row_0 = {
            a*j + b*m + c*p,
            a*k + b*n + c*q,
            a*l + b*o + c*r
        };

        std::vector<float> row_1 = {
            d*j + e*m + f*p,
            d*k + e*n + f*q,
            d*l + e*o + f*r,
        };

        std::vector<float> row_2 = {
            g*j + h*m + i*p,
            g*k + h*n + i*q,
            g*l + h*o + i*r
        };

        std::vector<std::vector<float>> result;
        result.push_back(row_0);
        result.push_back(row_1);
        result.push_back(row_2);

        return matrix(result);
    }

    position dot(position pos) {
        // mat a
        float a = mat[0][0];
        float b = mat[0][1];
        float c = mat[0][2];
        float d = mat[1][0];
        float e = mat[1][1];
        float f = mat[1][2];
        float g = mat[2][0];
        float h = mat[2][1];
        float i = mat[2][2];

        position result(
            pos.x*a + pos.y*b + pos.z*c, // x coord
            pos.x*d + pos.y*e + pos.z*f, // y coord
            pos.x*g + pos.y*h + pos.z*i  // z coord 
        );

        return result;
    }

    matrix add (matrix in) {
        matrix temp_mat(mat);
        temp_mat.mat[0][0] = temp_mat.mat[0][0] + in.mat[0][0];
        temp_mat.mat[0][1] = temp_mat.mat[0][1] + in.mat[0][1];
        temp_mat.mat[0][2] = temp_mat.mat[0][2] + in.mat[0][2];

        temp_mat.mat[1][0] = temp_mat.mat[1][0] + in.mat[1][0];
        temp_mat.mat[1][1] = temp_mat.mat[1][1] + in.mat[1][1];
        temp_mat.mat[1][2] = temp_mat.mat[1][2] + in.mat[1][2];

        temp_mat.mat[2][0] = temp_mat.mat[2][0] + in.mat[2][0];
        temp_mat.mat[2][1] = temp_mat.mat[2][1] + in.mat[2][1];
        temp_mat.mat[2][2] = temp_mat.mat[2][2] + in.mat[2][2];
        return temp_mat;
    }

    matrix scale(float f) {
        matrix temp_mat(mat);
        temp_mat.mat[0][0] = temp_mat.mat[0][0] * f;
        temp_mat.mat[0][1] = temp_mat.mat[0][1] * f;
        temp_mat.mat[0][2] = temp_mat.mat[0][2] * f;

        temp_mat.mat[1][0] = temp_mat.mat[1][0] * f;
        temp_mat.mat[1][1] = temp_mat.mat[1][1] * f;
        temp_mat.mat[1][2] = temp_mat.mat[1][2] * f;

        temp_mat.mat[2][0] = temp_mat.mat[2][0] * f;
        temp_mat.mat[2][1] = temp_mat.mat[2][1] * f;
        temp_mat.mat[2][2] = temp_mat.mat[2][2] * f;

        return temp_mat;
    }

    matrix transpose() {
        std::vector<std::vector<float>> temp_mat;

        std::vector<float> row_1 = { 
            mat[0][0], mat[1][0], mat[2][0]
        };

        std::vector<float> row_2 = { 
            mat[0][1], mat[1][1], mat[2][1]
        };

        std::vector<float> row_3 = { 
            mat[0][2], mat[1][2], mat[2][2]
        };

        return matrix(temp_mat);
    }


    std::string to_string() {
        std::string result = "";

        for (std::vector<float> row : mat) {
            result += "[";

            for (float elem : row) {
                result += " " + std::to_string(elem) + ",";
            }

            result += " ]\n";
        }

        return result;
    }

    std::string to_single_line_string() {
        std::string result = "";

        unsigned int count = 0;

        for (std::vector<float> row : mat) {
            result += "[";

            for (float elem : row) {
                result += std::to_string(elem) + ",";
            }

            count++;
            if (mat.size() != count) {
                result += "],";
            } else {
                result += "]";
            }
        }
        return result;
    }

    glm::mat4 convert_to_mat4() {
        float a = mat[0][0];
        float b = mat[0][1];
        float c = mat[0][2];
        float d = mat[1][0];
        float e = mat[1][1];
        float f = mat[1][2];
        float g = mat[2][0];
        float h = mat[2][1];
        float i = mat[2][2];

        // TODO does this actually convert to col major format??
        float temp[16] = {
            a, b, c, 0,
            d, e, f, 0,
            g, h, i, 0,
            0, 0, 0, 1
        };

        return glm::make_mat4(temp);
    }
};

matrix identity() {
    std::vector<std::vector<float>> identity;
    identity.push_back(
        {1, 0, 0}
    );
    identity.push_back(
        {0, 1, 0}
    );
    identity.push_back(
        {0, 0, 1}
    );

    return matrix(identity);
}

matrix skew_symmetric(position cross) {
    std::vector<std::vector<float>> skew;
    skew.push_back(
        {0,        -cross.z,  cross.y}
    );
    skew.push_back(
        {cross.z,   0,       -cross.x}
    );
    skew.push_back(
        {-cross.y,  cross.x, 0       }
    );
    return matrix(skew);
}


struct position_pair {
    position parent;
    position child;
    position_pair(position a, position b) {
        parent = a;
        child = b;
    }
};


struct bone {
    // index in position list
    int parent_index;
    int child_index;
    // whether or continue to children
    bool single_shot;
    std::string name;

    bone() {

    }


    bone(int p_ind, int c_ind, std::string given_name, bool is_single_shot=false) {
        parent_index = p_ind;
        child_index = c_ind;
        name = given_name;
        single_shot = is_single_shot;
    }

    std::string toString () {
        return "(" + std::to_string(parent_index) + ", " + std::to_string(child_index) + ")";
    }

    bool operator==(const bone &j) const {
        return parent_index == j.parent_index && child_index == j.child_index && single_shot == j.single_shot;
    }
};

struct BoneHasher {

        std::size_t operator()(const bone& k) const
        {
            using std::size_t;
            using std::hash;
            using std::string;

            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:
            std::size_t res = 17;
            res = res * 31 * hash<int>()(k.parent_index);
            res = res * 31 * hash<int>()(k.child_index);
            res = res * 31 * hash<bool>()(k.single_shot);

            return res;
        }
};


matrix rodrigues(position base_pos, position new_pos) {
    position new_norm = new_pos.normalize();
    position base_norm = base_pos.normalize();
        
    position cross = base_norm.cross(new_norm);
    
    // // arbitary axis determination when mag is 0
    if (cross.magnitude() == 0)  {
        cross = position(0, 1, 0);
        std::cout << "CROSS PRODUCT HAS MAGNITUTDE OF 0" << std::endl;
        // NOTE !! This is an error edge case. if this is ever seen the resulting rotation is WRONG
    }

    float cosin = base_norm.dot(new_norm);
    float sin = cross.magnitude();

    matrix skew_symmetrix = skew_symmetric(cross.normalize());
    matrix skew_sq = skew_symmetrix.dot(skew_symmetrix);
    
    matrix I = identity();

    matrix rotation = identity().add(skew_symmetrix.scale(sin)).add(skew_symmetrix.dot(skew_symmetrix).scale(1-cosin));
    return rotation;
}

struct bodymodel {
   std::vector<bone> bones;
   std::unordered_map<bone,std::vector<bone>, BoneHasher> bones_flow;
   std::vector<position> positions;
    // base set of joints, starting points of model
   std::vector<bone> base_bones;

    std::unordered_map <std::string, bone> name_bone_hash;

    // blank model
    bodymodel () {

    }

    bodymodel(std::vector<bone> incoming_joints, int base_index) {
        bones = incoming_joints;
        create_flow_fromm_bones(base_index);
        create_name_joint_hash();
       std::vector<position> positions;
    }

    void create_name_joint_hash() {
        for (bone j : bones) {
            name_bone_hash[j.name] = j;
        }
    }


    std::vector<std::vector<float>> vectorify_positions() {
        std::vector<std::vector<float>> pos;

        for (position p : positions) {
            pos.push_back(p.to_vector());
        }

        return pos;
    }

    std::vector<std::vector<float>> vectorify_positions_in_order() {
        std::vector<std::vector<float>> pos;

        for (bone base : base_bones) {
            // first step case
            position p_pos = positions[base.parent_index];
            position c_pos = positions[base.child_index];
            pos.push_back(p_pos.to_vector());
            pos.push_back(c_pos.to_vector());
            for (bone j : bones_flow[base]) { 
                p_pos = positions[j.parent_index];
                c_pos = positions[j.child_index];
                pos.push_back(p_pos.to_vector());
                pos.push_back(c_pos.to_vector());
            }
        }

        return pos;
    }


    std::unordered_map<std::string, matrix> construct_rotations(bodymodel base) {
        std::unordered_map<std::string, matrix> bone_name_to_rotation;
        for (bone j : base_bones) {
            position parent = positions[j.parent_index];
            position child = positions[j.child_index];
            //now adjust with parent as origin
            child = child.subtract(parent);

            bone base_bone = base.name_bone_hash[j.name];
            // base parent as origin
            position base_parent = base.positions[base_bone.parent_index];
            position base_child = base.positions[base_bone.child_index];
            base_child = base_child.subtract(base_parent);            
            // std::cout << "child " << child.toString() << " base child " << base_child.toString() << std::endl;
            matrix rotation = rodrigues(base_child , child);
            // std::cout << "constructed matrix is: \n" << rotation.to_string() << std::endl;
            // NOTE !! To rotate point A to point B, you will need to normalize point A, and then multiple
            // by the bones base distance. This is to prevent bone lengths changing due to model instability
            bone_name_to_rotation[j.name] = rotation;
            for (bone next_j : bones_flow[j]) {
                parent = positions[next_j.parent_index];
                child = positions[next_j.child_index];
                //now adjust with parent as origin
                child = child.subtract(parent);
                base_bone = base.name_bone_hash[next_j.name];
                // base parent as origin
                base_parent = base.positions[base_bone.parent_index];
                base_child = base.positions[base_bone.child_index];

                base_child = base_child.subtract(base_parent); 
                // std::cout << "child " << child.toString() << " base child " << base_child.toString() << std::endl;
                rotation = rodrigues(base_child, child);
                // std::cout << "constructed matrix is: \n" << rotation.to_string() << std::endl;

                bone_name_to_rotation[next_j.name] = rotation;
            }
        }
        return bone_name_to_rotation;
    }

    bodymodel rotate_self_by_rotations(std::unordered_map<std::string, matrix> rotations, bodymodel new_model) {
        std::vector<position> local_positions = positions;

        for (bone j : base_bones) {
            matrix current_matrix = rotations[j.name];
            local_positions = rotate_single_bone(j, local_positions, current_matrix);
            for (bone next_j : bones_flow[j]) {
                if (next_j.name == "rl_should")
                    continue;
                current_matrix = rotations[next_j.name];
                local_positions = rotate_single_bone(next_j, local_positions, current_matrix);
            }
        }

        new_model.set_positions(local_positions);
        return new_model;

    }

    std::vector<position> rotate_single_bone(bone j, std::vector<position> local_positions, matrix current_rot) {
        position parent = local_positions[j.parent_index];
        position child = local_positions[j.child_index];

        position local_pos = child.subtract(parent);
        position rotated_pos = current_rot.dot(local_pos).add(parent);

        local_positions[j.child_index] = rotated_pos;
        // now apply translation downstream
        position position_diff = rotated_pos.subtract(child);
        for (bone dj : bones_flow[j]) {                    
            local_positions[dj.child_index] = local_positions[dj.child_index].add(position_diff);
        }
        // std::cout << std::endl;
        return local_positions;
    }

    void rotate_single_bone_with_translation_map(
        bone j, 
        matrix current_rot, 
        std::vector<position>& local_positions, 
        std::unordered_map<std::string, position>& map) 
    {
        position parent = local_positions[j.parent_index];
        position child = local_positions[j.child_index];

        position local_pos = child.subtract(parent);
        position rotated_pos = current_rot.dot(local_pos).add(parent);

        local_positions[j.child_index] = rotated_pos;
        // now apply translation downstream
        position position_diff = rotated_pos.subtract(child);
        for (bone dj : bones_flow[j]) {                    
            local_positions[dj.child_index] = local_positions[dj.child_index].add(position_diff);
            // add tranlsation to map
            if (map.find(dj.name) == map.end())
                map[dj.name] = position_diff;
            else 
                map[dj.name] = map[dj.name].add(position_diff);
            
        }
    }

    std::string toString() {
       std::vector <position> local_pos = positions;
        // fail case
        if (positions.size() == 0){
            return "";
        }
        std::string return_string = "";
        // iterate through all bones in stream
        
        for (bone base : base_bones) {
            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            return_string += base.name + ": {" + p_pos.toString() +", "+ c_pos.toString() + " }, ";
            for (bone j : bones_flow[base]) { 
                p_pos = local_pos[j.parent_index];
                c_pos = local_pos[j.child_index];
                return_string += j.name + ": {" + p_pos.toString() +", "+ c_pos.toString() + " }, ";
            }
        }
        return return_string;
    }

    std::string toFlatString() {
       std::vector <position> local_pos = positions;
        // fail case
        if (positions.size() == 0){
            return "";
        }
        std::string return_string = "";
        // iterate through all bones in stream
        for (bone base : base_bones) {
            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            return_string += p_pos.toString();
            for (bone j : bones_flow[base]) { 
                p_pos = local_pos[j.parent_index];
                c_pos = local_pos[j.child_index];
                return_string += c_pos.toString();
            }
        }
        return return_string;
    }


    void set_positions (std::vector<position> new_positions) {
        positions = new_positions;
    }

    position_pair get_bones_positions (bone chosen_bone) {
        return position_pair {positions[chosen_bone.parent_index], positions[chosen_bone.child_index]};
    }

    void create_flow_fromm_bones (int base_index) {
       std::vector<bone> immediate_children;
        for (int i = 0; i < bones.size(); i++) {
            if (bones[i].parent_index == base_index && !bones[i].single_shot) {
                immediate_children.push_back(bones[i]);
            }
        }
        for (bone child: immediate_children) {
            create_flow_helper(child);
        }
        base_bones = immediate_children;
    }

    /**
     * @brief Recursive helper for constructing model stream flow
     * 
     * Finds each immediate downstream base of the parent base,
     *  recursively calls this method on each child and childs downstream bones to its own,
     *  and sets the bones_flow hashmap to all of its downstream bones in order of the 
     *  furthest upstream bones first
     * @param parent 
     * @return std::vector<bone> 
     */
   std::vector<bone> create_flow_helper (bone parent) {
        // find children of parent
       std::vector<bone> downstream;
        if (parent.single_shot) 
            return downstream;
        for (int i = 0; i < bones.size(); i++) {
            bone current_bone = bones[i];
            // if is a child of parent, and is a parent itself,
            //  bones is downstream
            if (parent.child_index == current_bone.parent_index) {
                downstream.push_back(current_bone);
               std::vector<bone> child_downstream = create_flow_helper(current_bone);
                downstream.insert(downstream.end(), child_downstream.begin(), child_downstream.end());
            }
        }
        bones_flow[parent] = downstream;
        return downstream;
    }


    /**
     * @brief gets the relative xyz position of bones in positions and returns a hashmap which 
     * 
     * @return unordered_map<bone, position, BaseHasher> 
     */
    std::unordered_map<bone, position, BoneHasher> get_bone_relative_positions () {
       std::vector <position> local_pos = positions;
        std::unordered_map<bone, position, BoneHasher> relative_pos;
        // fail case
        if (positions.size() == 0){
            return relative_pos;
        }
        // iterate through all bones in stream
        for (bone base : base_bones) {

            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            position difference = p_pos.subtract(c_pos);
            relative_pos[base] = difference;
            // for each child of base bones in order, find local difference
            for (bone j : bones_flow[base]) {
                position p_pos = local_pos[j.parent_index];
                position c_pos = local_pos[j.child_index];
                position difference = p_pos.subtract(c_pos);
                relative_pos[j] = difference;
            }
        }
        return relative_pos;
    }

    /**
     * @brief returns a flat position array of position differences
     * 
     * @returnstd::vector<position> 
     */
   std::vector<position> get_bone_relative_positions_flat() {
       std::vector <position> local_pos = positions;
       std::vector <position> relative_pos;

        if (positions.size() == 0){
            return relative_pos;
        }
        // iterate through all bones in stream
        for (bone base : base_bones) {

            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            position difference = p_pos.subtract(c_pos);
            relative_pos.push_back(difference);
            // for each child of base base in order, find local difference
            for (bone j : bones_flow[base]) {
                position p_pos = local_pos[j.parent_index];
                position c_pos = local_pos[j.child_index];
                position difference = p_pos.subtract(c_pos);
                relative_pos.push_back(difference);
            }
        }
        return relative_pos;
    }

    std::vector<bone> get_parent_bones_of_pos_index(int index) {
        std::vector<bone> local_bones;
        for (bone j : bones) {
            if (j.parent_index == index) {
                local_bones.push_back(j);
            }
        }
        return local_bones;
    }

    bone get_first_parent_bone_instance(int index) {
        for (bone j : bones) {
            if (j.parent_index == index) {
                return j;
            }
        }
        throw std::invalid_argument("get_first_parent_bone_instance: Failed to find input index as a parent...");
    }

    bone get_first_bone_that_has_child_in_flow(int parent, int child) {
        std::vector<bone> prospects;
        for (bone j : bones) {
            if (j.parent_index == parent && j.child_index == child)
                return j;
            else if (j.parent_index == parent)
                prospects.push_back(j);
            
        }
        for (bone j : prospects) {
            for (bone c : bones_flow[j]) {
                if (c.child_index == child || c.parent_index == child)
                    return j;
            }
        }
        throw std::invalid_argument("get_first_bone_that_has_child_in_flow: Failed to find input index as a parent...");
    }

    // gets all bones between bone parent and child in bone heirachy
    std::vector<bone> get_all_bone_between_parent_and_child(int parent, int child) {
        // I am assuming that parent and child are in one singular path, and have no branches
        std::vector<bone> parent_bone;
        bool has_found_parent = false;
        for (bone j : bones) {
            if (j.parent_index == parent  && j.child_index == child) {
                return {j};
            }
            else if (j.parent_index == parent) {
                parent_bone.push_back(j);
            }
        }
        for (bone pj : parent_bone) {
            std::vector<bone> inbetween_bones = {pj};
            bool has_child_in_path = false;
            for (bone j : bones_flow[pj]) {
                if (j.child_index == child) {
                    inbetween_bones.push_back(j);
                    has_child_in_path = true;
                    break;
                }

                inbetween_bones.push_back(j);
            }
            if (has_child_in_path) 
                return inbetween_bones;
        }
        throw std::invalid_argument("get_all_bone_between_parent_and_child: Failed to find proper input index as a parent...");

    }
};

/**
 * @brief Create a blaepose body model object
 * right hip is the center of body 
 *  R_HIP INDEX is 23
 * 
 * @param positions 
 * @returnstd::vector<bone> 
 */
bodymodel create_blaze_body_model () {

    int BLAZE_BASE_BONE = 23;

   std::vector<bone> blaze_bones;
    // right arm
    bone r_should_eblow (11, 13, "r_should_elbow");
    bone r_elbow_wrist  (13, 15, "r_elbow_wrist");
    bone r_wrist_thumb  (15, 21, "r_wrist_thumb");
    bone r_wrist_pinky  (15, 17, "r_wrist_pinky");
    bone r_wrist_index  (15, 19, "r_wrist_index");
    // probably don't need
    //joint r_index_pinky  (19, 17, true);

    // left arms
    bone l_should_elbow (12, 14, "l_should_elbow");
    bone l_elbow_wrist  (14, 16, "l_elbow_wrist");
    bone l_wrist_thumb  (16, 22, "l_wrist_thumb");
    bone l_wrist_index  (16, 20, "l_wrist_index");
    bone l_wrist_pinky  (16, 18, "l_wrist_pinky");
    // probably don't need
    //joint l_index_pinky  (20, 18, true);

    // right leg
    bone r_hip_knee   (23, 25, "r_hip_knee");
    bone r_knee_ankle (25, 27, "r_knee_ankle");
    bone r_ankle_heel (27, 29, "r_ankle_heel");
    bone r_ankle_foot (27, 31, "r_ankle_foot");
    // probably don't need
    //joint r_heel_foot  (29, 31, true);

    // left leg
    bone l_hip_knee   (24, 26, "l_hip_knee");
    bone l_knee_ankle (26, 28, "l_knee_ankle");
    bone l_ankle_heel (28, 30, "l_ankle_heel");
    bone l_ankle_foot (28, 32, "l_ankle_foot");
    //joint l_heel_foot  (30, 32, true);

    // center body
    bone lr_hip       (23, 24, "lr_hip");
    bone l_hip_should (23, 11, "l_hip_should");
    bone r_hip_should (24, 12, "r_hip_should");
    bone rl_should    (12, 11, "rl_should", true);    


    blaze_bones.push_back(r_should_eblow);
    blaze_bones.push_back(r_elbow_wrist);
    blaze_bones.push_back(r_wrist_thumb);
    blaze_bones.push_back(r_wrist_pinky);
    blaze_bones.push_back(r_wrist_index);
    //blaze_joints.push_back(r_index_pinky);

    blaze_bones.push_back(l_should_elbow);
    blaze_bones.push_back(l_elbow_wrist);
    blaze_bones.push_back(l_wrist_thumb);
    blaze_bones.push_back(l_wrist_pinky);
    blaze_bones.push_back(l_wrist_index);
    //blaze_joints.push_back(l_index_pinky);
    
    blaze_bones.push_back(r_hip_knee);
    blaze_bones.push_back(r_knee_ankle);
    blaze_bones.push_back(r_ankle_heel);
    blaze_bones.push_back(r_ankle_foot);
    //blaze_joints.push_back(r_heel_foot);

    blaze_bones.push_back(l_hip_knee);
    blaze_bones.push_back(l_knee_ankle);
    blaze_bones.push_back(l_ankle_heel);
    blaze_bones.push_back(l_ankle_foot);
    //blaze_joints.push_back(l_heel_foot);

    blaze_bones.push_back(lr_hip);
    blaze_bones.push_back(l_hip_should);
    blaze_bones.push_back(r_hip_should);
    blaze_bones.push_back(rl_should);

    // now piece the model togther
    bodymodel blaze_model (blaze_bones, BLAZE_BASE_BONE);

    return blaze_model;
} 

/**
 * @brief Create a blazepose body model object
 * BUT instead of using an 'sqaure' torso, 
 * this uses an 'spine', constructed out of midpoints from 
 * left/right shoulders and hip markers.
 * 
 * @param positions 
 * @returnstd::vector<bone> 
 */
bodymodel create_adjusted_blaze_model() {
    int ADJUSTED_BASE_BONE = 0;

    const int HIP = 0;
    const int SPINE = 1;
    
    const int RIGHT_SHOULDER = 2;
    const int RIGHT_ELBOW = 3;
    const int RIGHT_HAND = 4;

    const int LEFT_SHOULDER = 5;
    const int LEFT_ELBOW = 6;
    const int LEFT_HAND = 7;

    const int RIGHT_KNEE = 8;
    const int RIGHT_FOOT = 9;

    const int LEFT_KNEE = 10;
    const int LEFT_FOOT = 11;
    const int HEAD = 12;


    // base positions
    bone hip_spine (HIP, SPINE, "hip_spine");

    // right arm
    bone r_spine_should (SPINE, RIGHT_SHOULDER, "r_spine_should");
    bone r_should_elbow (RIGHT_SHOULDER, RIGHT_ELBOW, "r_should_elbow");
    bone r_elbow_hand (RIGHT_ELBOW, RIGHT_HAND, "r_elbow_hand");

    // left arm 
    bone l_spine_should (SPINE, LEFT_SHOULDER, "l_spine_should");
    bone l_should_elbow (LEFT_SHOULDER, LEFT_ELBOW, "l_should_elbow");
    bone l_elbow_hand (LEFT_ELBOW, LEFT_HAND, "l_elbow_hand");

    // right leg
    bone r_hip_knee (HIP, RIGHT_KNEE, "r_hip_knee");
    bone r_knee_foot (RIGHT_KNEE, RIGHT_FOOT, "r_knee_foot");

    // left leg
    bone l_hip_knee (HIP, LEFT_KNEE, "l_hip_knee");
    bone l_knee_foot (LEFT_KNEE, LEFT_FOOT, "l_knee_foot");

    // head
    bone spine_head (SPINE, HEAD, "spine_head");

    std::vector<bone> adjusted_bones = {
        hip_spine, 
        r_spine_should, r_should_elbow, r_elbow_hand,
        l_spine_should, l_should_elbow, l_elbow_hand,
        r_hip_knee, r_knee_foot,
        l_hip_knee, l_knee_foot,
        spine_head
    };

    // now piece the model togther
    return bodymodel(adjusted_bones, ADJUSTED_BASE_BONE);
} 

/**
 * Creates the adjusted model positions from an input blazepose model.
 * This is done to create a spin torso based model vs. an box torso model.
 * Most 3D models use the spin torso model, so this simply makes it easier to apply
 * rotations calculated from human pose to model pose.
 * 
 * returns the new positions for the adjusted model, given an old adjusted model and new blaze model.
*/
bodymodel set_adjust_points_from_blaze (bodymodel adjusted_model, bodymodel blaze_model) {
    // what I need to do, find midpoint between l_hip and r_hip, this is hip
    // fine midpoint between l_should and right_should, this is spine
    // recreate positions array using adjusted indexing
    bodymodel local_adjusted_model = adjusted_model;

    std::vector<position> blaze_positions = blaze_model.positions;

    const int BLAZE_LEFT_HIP = 23;
    const int BLAZE_RIGHT_HIP = 24;

    const int BLAZE_LEFT_SHOULDER = 11;
    const int BLAZE_RIGHT_SHOULDER = 12;

    // calculating hip and shoulder midpoints
    position blaze_hip_midpoint = blaze_positions[BLAZE_LEFT_HIP]
        .add(blaze_positions[BLAZE_RIGHT_HIP]).scale(0.5);
    position blaze_shoulder_midpoint = blaze_positions[BLAZE_LEFT_SHOULDER]
        .add(blaze_positions[BLAZE_RIGHT_SHOULDER]).scale(0.5);

    std::vector adjusted_positions = {
        blaze_hip_midpoint,      // hip
        blaze_shoulder_midpoint, // spine
        blaze_positions[11],     // right shoulder
        blaze_positions[13],     // right elbow
        blaze_positions[15],     // right hand
        blaze_positions[12],     // left shoulder
        blaze_positions[14],     // left elbow
        blaze_positions[16],     // left hand
        blaze_positions[25],     // right knee
        blaze_positions[27],     // right foot
        blaze_positions[26],     // left knee
        blaze_positions[28],     // left foot
        blaze_positions[0]       // nose (standin for head)

    };
    local_adjusted_model.set_positions(adjusted_positions);

    // now that I have the positions of the local model, I have to calculate rotations using this 
    // I'll keep same file format, so i'll have to reconvert blaze model postitions to adjust model in graphics.
    return local_adjusted_model;
}

/**
 * constructs a dancing vampire model. Manually done to insure correctness.
*/
bodymodel create_local_dancing_vampire_model() {

    //torso
    bone hips_spine(HIPS, SPINE, "hips_spine");
    bone spine_spine_1(SPINE, SPINE_1, "spine_spine_1");
    bone spine_1_spine_2(SPINE_1, SPINE_2, "spine_1_spine_2");


    // left arm
    bone spine_2_left_shoulder(SPINE_2, LEFT_SHOULDER, "spine_2_left_shoulder");
    bone left_shoulder_left_arm(LEFT_SHOULDER, LEFT_ARM, "left_shoulder_left_arm");
    bone left_arm_left_forearm(LEFT_ARM, LEFT_FOREARM, "left_arm_left_forearm");
    bone left_forearm_left_hand(LEFT_FOREARM, LEFT_HAND, "left_forearm_left_hand");

    // left hand
    bone left_hand_left_hand_thumb_1(LEFT_HAND, LEFT_HAND_THUMB_1, "left_hand_left_hand_thumb");
    bone left_hand_pink_1_left_hand_thumb_2(LEFT_HAND_THUMB_1, LEFT_HAND_THUMB_2, "left_hand_thumb_1_left_hand_thumb_2");
    bone left_hand_pink_2_left_hand_thumb_3(LEFT_HAND_THUMB_2, LEFT_HAND_THUMB_3, "left_hand_thumb_2_left_hand_thumb_3");
    bone left_hand_pink_3_left_hand_thumb_4(LEFT_HAND_THUMB_3, LEFT_HAND_THUMB_4, "left_hand_thumb_3_left_hand_thumb_4");

    bone left_hand_left_hand_index_1(LEFT_HAND, LEFT_HAND_INDEX_1, "left_hand_left_hand_index");
    bone left_hand_pink_1_left_hand_index_2(LEFT_HAND_INDEX_1, LEFT_HAND_INDEX_2, "left_hand_index_1_left_hand_index_2");
    bone left_hand_pink_2_left_hand_index_3(LEFT_HAND_INDEX_2, LEFT_HAND_INDEX_3, "left_hand_index_2_left_hand_index_3");
    bone left_hand_pink_3_left_hand_index_4(LEFT_HAND_INDEX_3, LEFT_HAND_INDEX_4, "left_hand_index_3_left_hand_index_4");

    bone left_hand_left_hand_middle_1(LEFT_HAND, LEFT_HAND_MIDDLE_1, "left_hand_left_hand_middle");
    bone left_hand_pink_1_left_hand_middle_2(LEFT_HAND_MIDDLE_1, LEFT_HAND_MIDDLE_2, "left_hand_middle_1_left_hand_middle_2");
    bone left_hand_pink_2_left_hand_middle_3(LEFT_HAND_MIDDLE_2, LEFT_HAND_MIDDLE_3, "left_hand_middle_2_left_hand_middle_3");
    bone left_hand_pink_3_left_hand_middle_4(LEFT_HAND_MIDDLE_3, LEFT_HAND_MIDDLE_4, "left_hand_middle_3_left_hand_middle_4");

    bone left_hand_left_hand_ring_1(LEFT_HAND, LEFT_HAND_RING_1, "left_hand_left_hand_ring");
    bone left_hand_pink_1_left_hand_ring_2(LEFT_HAND_RING_1, LEFT_HAND_RING_2, "left_hand_ring_1_left_hand_ring_2");
    bone left_hand_pink_2_left_hand_ring_3(LEFT_HAND_RING_2, LEFT_HAND_RING_3, "left_hand_ring_2_left_hand_ring_3");
    bone left_hand_pink_3_left_hand_ring_4(LEFT_HAND_RING_3, LEFT_HAND_RING_4, "left_hand_ring_3_left_hand_ring_4");

    bone left_hand_left_hand_pinky_1(LEFT_HAND, LEFT_HAND_PINKY_1, "left_hand_left_hand_pinky");
    bone left_hand_pink_1_left_hand_pinky_2(LEFT_HAND_PINKY_1, LEFT_HAND_PINKY_2, "left_hand_pinky_1_left_hand_pinky_2");
    bone left_hand_pink_2_left_hand_pinky_3(LEFT_HAND_PINKY_2, LEFT_HAND_PINKY_3, "left_hand_pinky_2_left_hand_pinky_3");
    bone left_hand_pink_3_left_hand_pinky_4(LEFT_HAND_PINKY_3, LEFT_HAND_PINKY_4, "left_hand_pinky_3_left_hand_pinky_4");


    // right arm
    bone spine_2_right_shoulder(SPINE_2, RIGHT_SHOULDER, "spine_2_right_shoulder");
    bone right_shoulder_right_arm(RIGHT_SHOULDER, RIGHT_ARM, "right_shoulder_right_arm");
    bone right_arm_right_forearm(RIGHT_ARM, RIGHT_FOREARM, "right_arm_right_forearm");
    bone right_forearm_right_hand(RIGHT_FOREARM, RIGHT_HAND, "right_forearm_right_hand");

    // right hand
    bone right_hand_right_hand_thumb_1(RIGHT_HAND, RIGHT_HAND_THUMB_1, "right_hand_right_hand_thumb");
    bone right_hand_pink_1_right_hand_thumb_2(RIGHT_HAND_THUMB_1, RIGHT_HAND_THUMB_2, "right_hand_thumb_1_right_hand_thumb_2");
    bone right_hand_pink_2_right_hand_thumb_3(RIGHT_HAND_THUMB_2, RIGHT_HAND_THUMB_3, "right_hand_thumb_2_right_hand_thumb_3");
    bone right_hand_pink_3_right_hand_thumb_4(RIGHT_HAND_THUMB_3, RIGHT_HAND_THUMB_4, "right_hand_thumb_3_right_hand_thumb_4");

    bone right_hand_right_hand_index_1(RIGHT_HAND, RIGHT_HAND_INDEX_1, "right_hand_right_hand_index");
    bone right_hand_pink_1_right_hand_index_2(RIGHT_HAND_INDEX_1, RIGHT_HAND_INDEX_2, "right_hand_index_1_right_hand_index_2");
    bone right_hand_pink_2_right_hand_index_3(RIGHT_HAND_INDEX_2, RIGHT_HAND_INDEX_3, "right_hand_index_2_right_hand_index_3");
    bone right_hand_pink_3_right_hand_index_4(RIGHT_HAND_INDEX_3, RIGHT_HAND_INDEX_4, "right_hand_index_3_right_hand_index_4");

    bone right_hand_right_hand_middle_1(RIGHT_HAND, RIGHT_HAND_MIDDLE_1, "right_hand_right_hand_middle");
    bone right_hand_pink_1_right_hand_middle_2(RIGHT_HAND_MIDDLE_1, RIGHT_HAND_MIDDLE_2, "right_hand_middle_1_right_hand_middle_2");
    bone right_hand_pink_2_right_hand_middle_3(RIGHT_HAND_MIDDLE_2, RIGHT_HAND_MIDDLE_3, "right_hand_middle_2_right_hand_middle_3");
    bone right_hand_pink_3_right_hand_middle_4(RIGHT_HAND_MIDDLE_3, RIGHT_HAND_MIDDLE_4, "right_hand_middle_3_right_hand_middle_4");

    bone right_hand_right_hand_ring_1(RIGHT_HAND, RIGHT_HAND_RING_1, "right_hand_right_hand_ring");
    bone right_hand_pink_1_right_hand_ring_2(RIGHT_HAND_RING_1, RIGHT_HAND_RING_2, "right_hand_ring_1_right_hand_ring_2");
    bone right_hand_pink_2_right_hand_ring_3(RIGHT_HAND_RING_2, RIGHT_HAND_RING_3, "right_hand_ring_2_right_hand_ring_3");
    bone right_hand_pink_3_right_hand_ring_4(RIGHT_HAND_RING_3, RIGHT_HAND_RING_4, "right_hand_ring_3_right_hand_ring_4");

    bone right_hand_right_hand_pinky_1(RIGHT_HAND, RIGHT_HAND_PINKY_1, "right_hand_right_hand_pinky");
    bone right_hand_pink_1_right_hand_pinky_2(RIGHT_HAND_PINKY_1, RIGHT_HAND_PINKY_2, "right_hand_pinky_1_right_hand_pinky_2");
    bone right_hand_pink_2_right_hand_pinky_3(RIGHT_HAND_PINKY_2, RIGHT_HAND_PINKY_3, "right_hand_pinky_2_right_hand_pinky_3");
    bone right_hand_pink_3_right_hand_pinky_4(RIGHT_HAND_PINKY_3, RIGHT_HAND_PINKY_4, "right_hand_pinky_3_right_hand_pinky_4");

    // left leg
    bone hips_left_up_leg(HIPS, LEFT_UP_LEG, "hips_left_up_leg");
    bone left_up_leg_left_leg(LEFT_UP_LEG, LEFT_LEG, "left_up_leg_left_leg");
    bone left_leg_left_toe_base(LEFT_LEG, LEFT_TOE_BASE, "left_leg_left_toe_base");
    bone left_toe_base_left_toe_end(LEFT_TOE_BASE, LEFT_TOE_END, "left_toe_base_left_toe_end");

    // right leg
    bone hips_right_up_leg(HIPS, RIGHT_UP_LEG, "hips_right_up_leg");
    bone right_up_leg_right_leg(RIGHT_UP_LEG, RIGHT_LEG, "right_up_leg_right_leg");
    bone right_leg_right_toe_base(RIGHT_LEG, RIGHT_TOE_BASE, "right_leg_right_toe_base");
    bone right_toe_base_right_toe_end(RIGHT_TOE_BASE, RIGHT_TOE_END, "right_toe_base_right_toe_end");

    // HEAD AND NECK
    bone spine_2_neck(SPINE_2, NECK, "spine_2_neck");
    bone neck_head(NECK, HEAD, "neck_head");

    std::vector<bone> bones = {
        hips_spine, spine_spine_1, spine_1_spine_2, spine_2_left_shoulder, left_shoulder_left_arm, 
        left_arm_left_forearm, left_forearm_left_hand, left_hand_left_hand_thumb_1, left_hand_pink_1_left_hand_thumb_2, 
        left_hand_pink_2_left_hand_thumb_3, left_hand_pink_3_left_hand_thumb_4, left_hand_left_hand_index_1, 
        left_hand_pink_1_left_hand_index_2, left_hand_pink_2_left_hand_index_3, left_hand_pink_3_left_hand_index_4, 
        left_hand_left_hand_middle_1, left_hand_pink_1_left_hand_middle_2, left_hand_pink_2_left_hand_middle_3, 
        left_hand_pink_3_left_hand_middle_4, left_hand_left_hand_ring_1, left_hand_pink_1_left_hand_ring_2, 
        left_hand_pink_2_left_hand_ring_3, left_hand_pink_3_left_hand_ring_4, left_hand_left_hand_pinky_1, 
        left_hand_pink_1_left_hand_pinky_2, left_hand_pink_2_left_hand_pinky_3, left_hand_pink_3_left_hand_pinky_4, 
        spine_2_right_shoulder, right_shoulder_right_arm, right_arm_right_forearm, right_forearm_right_hand, 
        right_hand_right_hand_thumb_1, right_hand_pink_1_right_hand_thumb_2, right_hand_pink_2_right_hand_thumb_3, 
        right_hand_pink_3_right_hand_thumb_4, right_hand_right_hand_index_1, right_hand_pink_1_right_hand_index_2, 
        right_hand_pink_2_right_hand_index_3, right_hand_pink_3_right_hand_index_4, right_hand_right_hand_middle_1, 
        right_hand_pink_1_right_hand_middle_2, right_hand_pink_2_right_hand_middle_3, right_hand_pink_3_right_hand_middle_4, 
        right_hand_right_hand_ring_1, right_hand_pink_1_right_hand_ring_2, right_hand_pink_2_right_hand_ring_3, 
        right_hand_pink_3_right_hand_ring_4, right_hand_right_hand_pinky_1, right_hand_pink_1_right_hand_pinky_2, 
        right_hand_pink_2_right_hand_pinky_3, right_hand_pink_3_right_hand_pinky_4, hips_left_up_leg, left_up_leg_left_leg, 
        left_leg_left_toe_base, left_toe_base_left_toe_end, hips_right_up_leg, right_up_leg_right_leg, 
        right_leg_right_toe_base, right_toe_base_right_toe_end,
        spine_2_neck, neck_head
    };

    return bodymodel(bones, HIPS);
}




void print_map(std::unordered_map<bone, position, BoneHasher> m) {
    for (auto pair: m) {
        std::cout << "{(" << pair.first.parent_index << ", " << pair.first.child_index << "): " << pair.second.toString() << "}\n";
    }
}

position split_string_kp_into_arr (std::string keypoints, bool reverse_y=false) {
    std::string end_bracket = "]";
    std::string comma = ",";
    int start, end = -1*comma.size();
    float x;
    start = end + comma.size();
    end = keypoints.find(comma, start);    
    std::string current_num = keypoints.substr(start, end-start);
    // try catch for case of dangling ,
    try {
        x = stof(current_num);
    } catch (...) {
        start = end + comma.size();
        end = keypoints.find(comma, start);    
        std::string current_num = keypoints.substr(start, end-start);
        x = stof(current_num);
    }
    start = end + comma.size();
    end = keypoints.find(comma, start);
    current_num = keypoints.substr(start, end-start);
    float y = stof(current_num);
    if (reverse_y)
        y*=-1;

    start = end + comma.size();
    end = keypoints.find(comma, start);
    // now you have 
    current_num = keypoints.substr(start, end-start);
    float z = stof(current_num);

    return {x, y, z};
}

std::vector<position> split_blaze_keypoints (std::string kp, bool reverse_y=false) {
    std::string end_bracket = "]";
    std::string comma = ",";
    int start, end = -1*end_bracket.size();
    std::vector<position> positions; 
    do {
        start = end + end_bracket.size();
        end = kp.find(end_bracket, start);
        // now you have 
        std::string keypoints = kp.substr(start, end-start);
        keypoints.erase(remove(keypoints.begin(), keypoints.end(), '['), keypoints.end());
        // now I should have NUM, NUM, NUM
        //cout << "gotten " << keypoints << "\n";
        if (keypoints.find(",") == std::string::npos)
            break;
        positions.push_back(
            split_string_kp_into_arr(keypoints, reverse_y)
        );
    } while (end != -1);
    std::cout << "POSITIONS SIZE: "  << positions.size() << std::endl;
    return positions;
}

// blaze model does not need positions, it is just used to grab bone names
std::tuple<bodymodel, std::unordered_map<std::string, position>> apply_rotations_to_vamp_model(
    std::unordered_map<std::string, matrix> blaze_rotations, 
    bodymodel vamp, 
    bodymodel blaze
) {
    auto blaze_vamp_mapping = blaze_to_vampire_map();
    std::vector<position> local_positions = vamp.positions;

    auto new_vamp = vamp;
    std::unordered_map<std::string, position> translation_map;

    // iterate thru blaze model to get rotation and vamp bone
    for (auto base_bone : blaze.base_bones) {
        // get local rotations
        auto current_rot = blaze_rotations[base_bone.name];
        
        // get vamp pos index tuple
        auto vamp_bone_indexs = blaze_vamp_mapping[base_bone.name];
        for (auto bone_tuple : vamp_bone_indexs) {


            std::vector<bone> vamp_bones = vamp.get_all_bone_between_parent_and_child(
                std::get<0>(bone_tuple),
                std::get<1>(bone_tuple)
            );
            // apply rotation for each bone
            for (bone vamp_bone : vamp_bones) {
                // local_positions = vamp.rotate_single_bone_with_translation_map(vamp_bone, local_positions, current_rot, translation_map);
                vamp.rotate_single_bone_with_translation_map(vamp_bone, current_rot, local_positions, translation_map);
            }
        }

        for (auto local_bone : blaze.bones_flow[base_bone]) {
            // get local rotations
            auto current_rot = blaze_rotations[local_bone.name];
            
            // get vamp pos index tuple
            auto vamp_bone_indexs = blaze_vamp_mapping[local_bone.name];
            for (auto bone_tuple : vamp_bone_indexs) {
                std::vector<bone> vamp_bones = vamp.get_all_bone_between_parent_and_child(
                    std::get<0>(bone_tuple),
                    std::get<1>(bone_tuple)
                );
                // apply rotation for each bone
                for (bone vamp_bone : vamp_bones) {
                    // local_positions = vamp.rotate_single_bone_with_translation_map(vamp_bone, local_positions, current_rot, translation_map);
                    vamp.rotate_single_bone_with_translation_map(vamp_bone, current_rot, local_positions, translation_map);
                }
            }

        }
    }

    new_vamp.set_positions(local_positions);
    return {new_vamp, translation_map};
}

std::unordered_map<std::string, matrix> get_vampire_blaze_rotations(bodymodel blaze, bodymodel vampire) {
    auto blaze_vamp_mapping = blaze_to_vampire_map();
    std::unordered_map<std::string, matrix> bone_name_to_rotation;

    // iterate thru blaze model to get rotation and vamp bone
    for (auto base_bone : blaze.base_bones) {
        // get local blaze positions

        position curr_parent = blaze.positions[base_bone.parent_index];
        position curr_child = blaze.positions[base_bone.child_index];
        // now adjust with parent as origin
        position rel_curr_child = curr_child.subtract(curr_parent);

        // lets get vampire pos indicies
        // (grabs first position as only mutiple one is hands)
        int vamp_parent_ind = std::get<0>(blaze_vamp_mapping[base_bone.name][0]);
        int vamp_child_ind = std::get<1>(blaze_vamp_mapping[base_bone.name][0]);

        position vamp_parent = vampire.positions[vamp_parent_ind];
        position vamp_child = vampire.positions[vamp_child_ind]; 
        // now adjust with parent as origin
        position rel_vamp_child = vamp_child.subtract(vamp_parent);

        matrix rotation = rodrigues(rel_vamp_child, rel_curr_child);

        bone_name_to_rotation[base_bone.name] = rotation;

        for (auto local_bone : blaze.bones_flow[base_bone]) {
            position curr_parent = blaze.positions[local_bone.parent_index];
            position curr_child = blaze.positions[local_bone.child_index];
            // now adjust with parent as origin
            position rel_curr_child = curr_child.subtract(curr_parent);

            // lets get vampire pos indicies
        // (grabs first position as only mutiple one is hands)
            int vamp_parent_ind = std::get<0>(blaze_vamp_mapping[local_bone.name][0]);
            int vamp_child_ind = std::get<1>(blaze_vamp_mapping[local_bone.name][0]);


            position vamp_parent = vampire.positions[vamp_parent_ind];
            position vamp_child = vampire.positions[vamp_child_ind]; 
            // now adjust with parent as origin
            position rel_vamp_child = vamp_child.subtract(vamp_parent);

            matrix rotation = rodrigues(rel_vamp_child, rel_curr_child);

            bone_name_to_rotation[local_bone.name] = rotation;
        }
    }

    return bone_name_to_rotation;
}


#endif
