#ifndef JOINT_UTILS_H
#define JOINT_UTILS_H

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


struct joint {
    // index in position list
    int parent_index;
    int child_index;
    // whether or continue to children
    bool single_shot;
    std::string name;

    joint() {

    }


    joint(int p_ind, int c_ind, std::string given_name, bool is_single_shot=false) {
        parent_index = p_ind;
        child_index = c_ind;
        name = given_name;
        single_shot = is_single_shot;
    }

    std::string toString () {
        return "(" + std::to_string(parent_index) + ", " + std::to_string(child_index) + ")";
    }

    bool operator==(const joint &j) const {
        return parent_index == j.parent_index && child_index == j.child_index && single_shot == j.single_shot;
    }
};

struct JointHasher {

        std::size_t operator()(const joint& k) const
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



struct position_over_time {
   std::vector<std::vector<position>> positions;

   std::vector<std::unordered_map<joint, position, JointHasher>> joint_maps;
    
    /**
     * @brief adds positions to local arrays
     * 
     * @param p 
     */
    void add_position(std::vector<position> p) {
        positions.push_back(p);
    }

    void add_map(std::unordered_map<joint, position, JointHasher> jm) {
        joint_maps.push_back(jm);
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
   std::vector<joint> joints;
   std::unordered_map<joint,std::vector<joint>, JointHasher> joints_flow;
   std::vector<position> positions;
    // base set of joints, starting points of model
   std::vector<joint> base_joints;

    std::unordered_map <std::string, joint> name_joint_hash;

    // blank model
    bodymodel () {

    }

    bodymodel(std::vector<joint> incoming_joints, int base_index) {
        joints = incoming_joints;
        create_flow_fromm_joints(base_index);
        create_name_joint_hash();
       std::vector<position> positions;
    }

    void create_name_joint_hash() {
        for (joint j : joints) {
            name_joint_hash[j.name] = j;
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

        for (joint base : base_joints) {
            // first step case
            position p_pos = positions[base.parent_index];
            position c_pos = positions[base.child_index];
            pos.push_back(p_pos.to_vector());
            pos.push_back(c_pos.to_vector());
            for (joint j : joints_flow[base]) { 
                p_pos = positions[j.parent_index];
                c_pos = positions[j.child_index];
                pos.push_back(p_pos.to_vector());
                pos.push_back(c_pos.to_vector());
            }
        }

        return pos;
    }


    std::unordered_map<std::string, matrix> construct_rotations(bodymodel base) {
        std::unordered_map<std::string, matrix> joint_name_to_rotation;
        for (joint j : base_joints) {
            position parent = positions[j.parent_index];
            position child = positions[j.child_index];
            //now adjust with parent as origin
            child = child.subtract(parent);

            joint base_joint = base.name_joint_hash[j.name];
            // base parent as origin
            position base_parent = base.positions[base_joint.parent_index];
            position base_child = base.positions[base_joint.child_index];
            base_child = base_child.subtract(base_parent);            
            std::cout << "child " << child.toString() << " base child " << base_child.toString() << std::endl;
            matrix rotation = rodrigues(base_child , child);
            std::cout << "constructed matrix is: \n" << rotation.to_string() << std::endl;
            // NOTE !! To rotate point A to point B, you will need to normalize point A, and then multiple
            // by the joints base distance. This is to prevent bone lengths changing due to model instability
            joint_name_to_rotation[j.name] = rotation;
            for (joint next_j : joints_flow[j]) {
                parent = positions[next_j.parent_index];
                child = positions[next_j.child_index];
                //now adjust with parent as origin
                child = child.subtract(parent);
                base_joint = base.name_joint_hash[next_j.name];
                // base parent as origin
                base_parent = base.positions[base_joint.parent_index];
                base_child = base.positions[base_joint.child_index];

                base_child = base_child.subtract(base_parent); 
                std::cout << "child " << child.toString() << " base child " << base_child.toString() << std::endl;
                rotation = rodrigues(base_child, child);
                std::cout << "constructed matrix is: \n" << rotation.to_string() << std::endl;

                joint_name_to_rotation[next_j.name] = rotation;
            }
        }
        return joint_name_to_rotation;
    }

    bodymodel rotate_self_by_rotations(std::unordered_map<std::string, matrix> rotations, bodymodel new_model) {
        std::vector<position> local_positions = positions;

        for (joint j : base_joints) {
            matrix current_matrix = rotations[j.name];
            local_positions = rotate_single_joint(j, local_positions, current_matrix);
            for (joint next_j : joints_flow[j]) {
                if (next_j.name == "rl_should")
                    continue;
                // current_matrix = current_matrix.dot(rotations[next_j.name]);
                current_matrix = rotations[next_j.name];
                local_positions = rotate_single_joint(next_j, local_positions, current_matrix);
            }
        }

        new_model.set_positions(local_positions);
        return new_model;

    }

    std::vector<position> rotate_single_joint(joint j, std::vector<position> local_positions, matrix current_rot) {
        position parent = local_positions[j.parent_index];
        position child = local_positions[j.child_index];

        position local_pos = child.subtract(parent);
        position rotated_pos = current_rot.dot(local_pos).add(parent);

        local_positions[j.child_index] = rotated_pos;
        // now apply translation downstream
        position position_diff = rotated_pos.subtract(child);
        for (joint dj : joints_flow[j]) {                    
            local_positions[dj.child_index] = local_positions[dj.child_index].add(position_diff);
        }
        // std::cout << std::endl;
        return local_positions;
    }

    std::string toString() {
       std::vector <position> local_pos = positions;
        // fail case
        if (positions.size() == 0){
            return "";
        }
        std::string return_string = "";
        // iterate through all joints in stream
        
        for (joint base : base_joints) {
            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            return_string += base.name + ": {" + p_pos.toString() +", "+ c_pos.toString() + " }, ";
            for (joint j : joints_flow[base]) { 
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
        // iterate through all joints in stream
        for (joint base : base_joints) {
            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            return_string += p_pos.toString();
            for (joint j : joints_flow[base]) { 
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

    position_pair get_joints_positions (joint chosen_joint) {
        return position_pair {positions[chosen_joint.parent_index], positions[chosen_joint.child_index]};

    }

    void create_flow_fromm_joints (int base_index) {
       std::vector<joint> immediate_children;
        for (int i = 0; i < joints.size(); i++) {
            if (joints[i].parent_index == base_index && !joints[i].single_shot) {
                immediate_children.push_back(joints[i]);
            }
        }
        for (joint child: immediate_children) {
            create_flow_helper(child);
        }
        base_joints = immediate_children;
    }

    /**
     * @brief Recursive helper for constructing model stream flow
     * 
     * Finds each immediate downstream joint of the parent joint,
     *  recursively calls this method on each child and childs downstream joints to its own,
     *  and sets the joints_flow hashmap to all of its downstream joints in order of the 
     *  furthest upstream joint first
     * @param parent 
     * @returnstd::vector<joint> 
     */
   std::vector<joint> create_flow_helper (joint parent) {
        // find children of parent
       std::vector<joint> downstream;
        if (parent.single_shot) 
            return downstream;
        for (int i = 0; i < joints.size(); i++) {
            joint current_joint = joints[i];
            // if is a child of parent, and is a parent itself,
            //  joint is downstream
            if (parent.child_index == current_joint.parent_index) {
                downstream.push_back(current_joint);
               std::vector<joint> child_downstream = create_flow_helper(current_joint);
                downstream.insert(downstream.end(), child_downstream.begin(), child_downstream.end());
            }
        }
        joints_flow[parent] = downstream;
        return downstream;
    }


    /**
     * @brief gets the relative xyz position of joints in positions and returns a hashmap which 
     * 
     * @return unordered_map<joint, position, JointHasher> 
     */
    std::unordered_map<joint, position, JointHasher> get_joint_relative_positions () {
       std::vector <position> local_pos = positions;
        std::unordered_map<joint, position, JointHasher> relative_pos;
        // fail case
        if (positions.size() == 0){
            return relative_pos;
        }
        // iterate through all joints in stream
        for (joint base : base_joints) {

            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            position difference = p_pos.subtract(c_pos);
            relative_pos[base] = difference;
            // for each child of base joint in order, find local difference
            for (joint j : joints_flow[base]) {
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
   std::vector<position> getJointRelativePositionsFlat() {
       std::vector <position> local_pos = positions;
       std::vector <position> relative_pos;

        if (positions.size() == 0){
            return relative_pos;
        }
        // iterate through all joints in stream
        for (joint base : base_joints) {

            // first step case
            position p_pos = local_pos[base.parent_index];
            position c_pos = local_pos[base.child_index];
            position difference = p_pos.subtract(c_pos);
            relative_pos.push_back(difference);
            // for each child of base joint in order, find local difference
            for (joint j : joints_flow[base]) {
                position p_pos = local_pos[j.parent_index];
                position c_pos = local_pos[j.child_index];
                position difference = p_pos.subtract(c_pos);
                relative_pos.push_back(difference);
            }
        }
        return relative_pos;
    }
};

/**
 * @brief Create a blaepose body model object
 * right hip is the center of body 
 *  R_HIP INDEX is 23
 * 
 * @param positions 
 * @returnstd::vector<joint> 
 */
bodymodel create_blaze_body_model () {

    int BLAZE_BASE_JOINT = 23;

   std::vector<joint> blaze_joints;
    // right arm
    joint r_should_eblow (11, 13, "r_should_elbow");
    joint r_elbow_wrist  (13, 15, "r_elbow_wrist");
    joint r_wrist_thumb  (15, 21, "r_wrist_thumb");
    joint r_wrist_pinky  (15, 17, "r_wrist_pinky");
    joint r_wrist_index  (15, 19, "r_wrist_index");
    // probably don't need
    //joint r_index_pinky  (19, 17, true);

    // left arms
    joint l_should_elbow (12, 14, "l_should_elbow");
    joint l_elbow_wrist  (14, 16, "l_elbow_wrist");
    joint l_wrist_thumb  (16, 22, "l_wrist_thumb");
    joint l_wrist_index  (16, 20, "l_wrist_index");
    joint l_wrist_pinky  (16, 18, "l_wrist_pinky");
    // probably don't need
    //joint l_index_pinky  (20, 18, true);

    // right leg
    joint r_hip_knee   (23, 25, "r_hip_knee");
    joint r_knee_ankle (25, 27, "r_knee_ankle");
    joint r_ankle_heel (27, 29, "r_ankle_heel");
    joint r_ankle_foot (27, 31, "r_ankle_foot");
    // probably don't need
    //joint r_heel_foot  (29, 31, true);

    // left leg
    joint l_hip_knee   (24, 26, "l_hip_knee");
    joint l_knee_ankle (26, 28, "l_knee_ankle");
    joint l_ankle_heel (28, 30, "l_ankle_heel");
    joint l_ankle_foot (28, 32, "l_ankle_foot");
    //joint l_heel_foot  (30, 32, true);

    // center body
    joint lr_hip       (23, 24, "lr_hip");
    joint l_hip_should (23, 11, "l_hip_should");
    joint r_hip_should (24, 12, "r_hip_should");
    joint rl_should    (12, 11, "rl_should", true);



    blaze_joints.push_back(r_should_eblow);
    blaze_joints.push_back(r_elbow_wrist);
    blaze_joints.push_back(r_wrist_thumb);
    blaze_joints.push_back(r_wrist_pinky);
    blaze_joints.push_back(r_wrist_index);
    //blaze_joints.push_back(r_index_pinky);

    blaze_joints.push_back(l_should_elbow);
    blaze_joints.push_back(l_elbow_wrist);
    blaze_joints.push_back(l_wrist_thumb);
    blaze_joints.push_back(l_wrist_pinky);
    blaze_joints.push_back(l_wrist_index);
    //blaze_joints.push_back(l_index_pinky);
    
    blaze_joints.push_back(r_hip_knee);
    blaze_joints.push_back(r_knee_ankle);
    blaze_joints.push_back(r_ankle_heel);
    blaze_joints.push_back(r_ankle_foot);
    //blaze_joints.push_back(r_heel_foot);

    blaze_joints.push_back(l_hip_knee);
    blaze_joints.push_back(l_knee_ankle);
    blaze_joints.push_back(l_ankle_heel);
    blaze_joints.push_back(l_ankle_foot);
    //blaze_joints.push_back(l_heel_foot);

    blaze_joints.push_back(lr_hip);
    blaze_joints.push_back(l_hip_should);
    blaze_joints.push_back(r_hip_should);
    blaze_joints.push_back(rl_should);

    // now piece the model togther
    bodymodel blaze_model (blaze_joints, BLAZE_BASE_JOINT);

    return blaze_model;
} 


void print_map(std::unordered_map<joint, position, JointHasher> m) {
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
    return positions;
}




#endif
