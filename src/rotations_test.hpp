#ifndef ROTATIONS_TEST_HPP
#define ROTATIONS_TEST_HPP

#include "joint_utils.h"

bodymodel construct_test_model() {
    // im doing a 4, 3 connection point model test
    joint a_b(0, 1, "a_b");
    joint b_c(1, 2, "b_c");
    joint c_d(2, 3, "c_d");
    bodymodel test({a_b, b_c, c_d}, 0);

    // now lets set base positions
    // a (0,0,0) b (1,0,0) c (2,0,0) d (3,0,0)
    position a({0,0,0});
    position b({1,0,0});
    position c({2,0,0});
    position d({3,0,0});
    test.set_positions({a,b,c,d});
    return test;
}

bodymodel rotate_test_with_set_rotation(bodymodel test) {
    // matrix a;
    // 90 degree rotation around y axis
    std::vector<float> row_1 = {0.0,   0.0000000,  1.0};
    std::vector<float> row_2 = {0.0000000,   1.0000000,  0.0000000};
    std::vector<float> row_3 = {-1.0,  0.0000000,  0.0};

    matrix y90({row_1, row_2, row_3});
    bodymodel new_model = test;
    matrix curr_matrix = y90;
    for (unsigned int i = 1; i < new_model.positions.size(); i++) {
        position p = new_model.positions[i-1];
        position c = new_model.positions[i];

        position rel = c.subtract(p);
        position new_cur = curr_matrix.dot(rel).add(p);



        new_model.positions[i] = new_cur;
        // now add the relative difference downstream
        position new_rel = new_cur.subtract(c);
        for (unsigned int j = i+1; j < new_model.positions.size(); j++) {
            new_model.positions[j] = new_model.positions[j].add(new_rel);
        }
        std::cout << "MATRIX: " << i << "\n" << curr_matrix.to_string() << std::endl;
        curr_matrix = curr_matrix.dot(y90);
        
        std::cout << "JOINT: " << i << " " << new_model.toString() << std::endl;
    }

    return new_model;
}

void test_basic_rotations() {
    bodymodel test_model = construct_test_model();
    bodymodel rotated_model = rotate_test_with_set_rotation(test_model);
    std::cout << test_model.toString() << std::endl;
    std::cout << "rotated" << std::endl << rotated_model.toString() << std::endl;
    std::cout << "_________________________" << std::endl;

    auto map = rotated_model.construct_rotations(test_model);
    // for (auto mat : map) {
    //     std::cout << mat.second.to_string() << std::endl;
    // }
    auto new_model = test_model.rotate_self_by_rotations(map, rotated_model);
    
    std::cout << new_model.toString() << std::endl;
}


#endif