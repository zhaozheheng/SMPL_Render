#include <iostream>
#include <fstream>
#include <random>
#include "opencv2/opencv.hpp"
#include "Posemapper.h"
#include "lbs.h"

using namespace cv;

struct smpl_model
{
    Mat model;
//    Mat J_regressor_prior;
    Mat pose;
    Mat hands_components;
    Mat f;
    Mat J_regressor;
    Mat betas;
    Mat kintree_table;
    Mat J;
    Mat v_shaped;
//    Mat weights_prior;
    Mat trans;
    Mat v_posed;
    Mat hands_coeffs;
    Mat weights;
//    Mat vert_sym_idxs;
    Mat posedirs;
    Mat hands_mean;
    Mat v_template;
    Mat shapedirs;
};

struct handmodel
{
    Mat hands_components;
    Mat updated_hands_components;
    Mat hands_coeffs;
    Mat hands_mean;
};

void loadModel(smpl_model &smpl, handmodel &hand_l, handmodel &hand_r, double pose_set[78]) {
    //std::cout << "Hello, World!" << std::endl;
    FileStorage fs("data/SMPLH_female.yml", cv::FileStorage::READ);
    Mat J_regressor_prior, f, J_regressor, kintree_table, J, weights_prior, weights, vert_sym_idxs, posedirs,
            v_template, shapedirs, trans, pose, betas, full_pose;

    Mat selected_components, hands_mean, pose_coeffs, full_hand_pose;
    bool flat_hand_mean = false;
    int ncomps = 12, body_pose_dofs = 66;
//    fs["J_regressor_prior"] >> J_regressor_prior;
//    fs["hands_components"] >> hands_components;
    fs["f"] >> f;
    fs["J_regressor"] >> J_regressor;
    fs["kintree_table"] >> kintree_table;
    fs["J"] >> J;
//    fs["weights_prior"] >> weights_prior;
//    fs["hands_coeffs"] >> hands_coeffs;
    fs["weights"] >> weights;
//    fs["vert_sym_idxs"] >> vert_sym_idxs;
    fs["posedirs"] >> posedirs;
    fs["v_template"] >> v_template;
    fs["shape"] >> shapedirs;

    fs.release();
//    std::cout << "J_regressor_prior: " << J_regressor_prior.size << " and channels: " << J_regressor_prior.channels() << std::endl;
//    std::cout << "f: " << f.size << " and channels: " << f.channels() << std::endl;
//    std::cout << "J_regressor: " << J_regressor.size << " and channels: " << J_regressor.channels() << std::endl;
//    std::cout << "kintree_table: " << kintree_table.size << " and channels: " << kintree_table.channels() << std::endl;
//    std::cout << "J: " << J.size << " and channels: " << J.channels() << std::endl;
//    std::cout << "weights_prior: " << weights_prior.size << " and channels: " << weights_prior.channels() << std::endl;
//    std::cout << "weights: " << weights.size << " and channels: " << weights.channels() << std::endl;
//    std::cout << "vert_sym_idxs: " << vert_sym_idxs.size << " and channels: " << vert_sym_idxs.channels() << std::endl;
//    std::cout << "posedirs: " << posedirs.size << " and channels: " << posedirs.channels() << std::endl;
//    std::cout << "v_template: " << v_template.size << " and channels: " << v_template.channels() << std::endl;
//    std::cout << "shape: " << shapedirs.size << " and channels: " << shapedirs.channels() << std::endl;

//    std::cout << "hands_components: " << hands_components.size << " and channels: " << hands_components.channels() << std::endl;
//    std::cout << "hands_coeffs: " << hands_coeffs.size << " and channels: " << hands_coeffs.channels() << std::endl;
//    std::cout << "hands_mean: " << hands_mean.size << " and channels: " << hands_mean.channels() << std::endl;

    FileStorage lhand("data/MANO_LEFT.yml", cv::FileStorage::READ);
    FileStorage rhand("data/MANO_RIGHT.yml", cv::FileStorage::READ);

    lhand["hands_components"] >> hand_l.hands_components;
    rhand["hands_components"] >> hand_r.hands_components;

    lhand["hands_coeffs"] >> hand_l.hands_coeffs;
    rhand["hands_coeffs"] >> hand_r.hands_coeffs;

    if (flat_hand_mean){
        hand_l.hands_mean = Mat::zeros(hand_l.hands_components.rows, 1, CV_64F);
        hand_r.hands_mean = Mat::zeros(hand_r.hands_components.rows, 1, CV_64F);
    } else{
        lhand["hands_mean"] >> hand_l.hands_mean;
        rhand["hands_mean"] >> hand_r.hands_mean;
    }

    lhand.release();
    rhand.release();

    hand_l.hands_coeffs = hand_l.hands_coeffs.colRange(0, ncomps/2);
    hand_r.hands_coeffs = hand_r.hands_coeffs.colRange(0, ncomps/2);
    hconcat(hand_l.hands_components.rowRange(0, ncomps/2), Mat::zeros(ncomps/2, hand_l.hands_components.cols, CV_64F), hand_l.updated_hands_components);
    hconcat(Mat::zeros(ncomps/2, hand_r.hands_components.cols, CV_64F), hand_r.hands_components.rowRange(0, ncomps/2), hand_r.updated_hands_components);
    vconcat(hand_l.updated_hands_components, hand_r.updated_hands_components, selected_components);
//    std::cout << selected_components.size << std::endl;
//    std::cout << selected_components << std::endl;

    vconcat(hand_l.hands_mean, hand_r.hands_mean, hands_mean);
//    std::cout << hands_mean.size << std::endl;
//    std::cout << hands_mean << std::endl;
    //    selected_components = hands_components.rowRange(0, ncomps);

//    std::cout << selected_components.size << std::endl;
//    std::cout << selected_components << std::endl;

//    double corr_global_rot[3] = { 0.0, 0.0, 0.0 };
//    double next_ncomps_to_hand_pose[6] = { -0.42671473, -0.85829819, -0.50662164, +1.97374622, -0.84298473, -1.29958491 };

//    double pose_set[78] = { -0.17192541, +0.36310464, +0.05572387, -0.42836206, -0.00707548, +0.03556427,
//                      +0.18696896, -0.22704364, -0.39019834, +0.20273526, +0.07125099, +0.07105988,
//                      +0.71328310, -0.29426986, -0.18284189, +0.72134655, +0.07865227, +0.08342645,
//                      +0.00934835, +0.12881420, -0.02610217, -0.15579594, +0.25352553, -0.26097519,
//                      -0.04529948, -0.14718626, +0.52724564, -0.07638319, +0.03324086, +0.05886086,
//                      -0.05683995, -0.04069042, +0.68593617, -0.75870686, -0.08579930, -0.55086359,
//                      -0.02401033, -0.46217096, -0.03665799, +0.12397343, +0.10974685, -0.41607569,
//                      -0.26874970, +0.40249335, +0.21223768, +0.03365140, -0.05243080, +0.16074013,
//                      +0.13433811, +0.10414972, -0.98688595, -0.17270103, +0.29374368, +0.61868383,
//                      +0.00458329, -0.15357027, +0.09531648, -0.10624117, +0.94679869, -0.26851003,
//                      +0.58547889, -0.13735695, -0.39952280, -0.16598853, -0.14982575, -0.27937399,
//                      +0.12354536, -0.55101035, -0.41938681, +0.52238684, -0.23376718, -0.29814804,
//                      -0.42671473, -0.85829819, -0.50662164, +1.97374622, -0.84298473, -1.29958491 };

    pose_coeffs = Mat::zeros(body_pose_dofs + selected_components.rows, 1, CV_64F);
    // update pose coeffs
    pose_coeffs = Mat(body_pose_dofs + selected_components.rows, 1, CV_64F, pose_set);
//    vconcat(Mat(body_pose_dofs, 1, CV_64F, corr_global_rot), Mat(ncomps, 1, CV_64F, next_ncomps_to_hand_pose), pose_coeffs);

    full_hand_pose = (pose_coeffs.rowRange(body_pose_dofs, body_pose_dofs + ncomps).t() * selected_components).t();
    vconcat(pose_coeffs.rowRange(0, body_pose_dofs), hands_mean + full_hand_pose, full_pose);

//    std::cout << full_pose.size << std::endl;
//    std::cout << full_pose << std::endl;

//    pose = pose_coeffs;
//    std::cout << full_pose.size << std::endl;
//    std::cout << full_pose << std::endl;
//    std::cout << new_hands_coeffs.size << std::endl;

    bool want_shapemodel = !shapedirs.empty();
    int nposeprams = kintree_table.cols;

    if (trans.empty()) {
        trans = Mat::zeros(3, 1, CV_64F);
    }
    if (pose.empty()) {
        pose = Mat::zeros(nposeprams * 3, 1, CV_64F);
    }
    if (!shapedirs.empty() && betas.empty()) {
        betas = Mat::zeros(shapedirs.channels(), 1, CV_64F);
    }

    //random pose and betas
    /*std::cout << "----------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "prior_model.pose: " << model.pose << std::endl;
    std::cout << "proir_model.betas: " << model.betas << std::endl;*/

    //normal distribution random
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator (seed);

    //(0,1) normal distribution
    std::normal_distribution<double> distribution (0.0,1.0);

//    for (size_t i = 0; i < pose.rows; i++)
//    {
//        pose.at<double>(i, 0) = distribution(generator) * 0.05;
////        pose.at<double>(i, 0) = ((double)rand() / (RAND_MAX)) * 0.05;
////        pose.at<double>(i, 0) = 0.05;
//    }
//    std::cout << pose << std::endl;
    for (size_t i = 0; i < betas.rows; i++)
    {
        betas.at<double>(i, 0) = distribution(generator) * 0.03;
//        betas.at<double>(i, 0) = ((double)rand() / (RAND_MAX)) * 1.0;
//        betas.at<double>(i, 0) = 1.0;
    }

    /*std::cout << "after_model.pose: " << model.pose << std::endl;
    std::cout << "after_model.betas: " << model.betas << std::endl;*/

    Mat v_shaped, v_posed;

    posedirs = posedirs.reshape(1, posedirs.rows * posedirs.cols);
    //std::cout << posedirs.size << std::endl;
    //std::cout << beta << std::endl;

    v_template = v_template.reshape(1, v_template.rows * v_template.cols);
    //std::cout << v_template.size << std::endl;
    shapedirs = shapedirs.reshape(1, shapedirs.rows * shapedirs.cols);
    //std::cout << shapedirs.size << std::endl;

    if (want_shapemodel) {
//        std::cout << "!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        v_shaped = v_template + shapedirs * betas;

        v_shaped = v_shaped.reshape(1, v_shaped.rows / 3);

        Mat Jtmpx, Jtmpy, Jtmpz, tmp;

        //std::cout << v_shaped.size << std::endl;

        Jtmpx = J_regressor * v_shaped.col(0);
        //std::cout << Jtmpx << std::endl;
        //std::cout << Jtmpx.size << std::endl;
        Jtmpy = J_regressor * v_shaped.col(1);
        Jtmpz = J_regressor * v_shaped.col(2);
        hconcat(Jtmpx, Jtmpy, tmp);
        hconcat(tmp, Jtmpz, tmp);
        //std::cout << tmp.size << std::endl;
        J = tmp;    //update J matrix
        //todo update v_shape by using pose mapper
        v_posed = posedirs * posemap(full_pose); //20670x207 * 207x1
        v_posed = v_posed.reshape(1, v_posed.rows / 3) + v_shaped;  //reshape to 6890x3 + 6890x3
    }
    else {
        //todo update v_shape by using pose mapper
        v_posed = posedirs * posemap(full_pose); //20670x207 * 207x1
        v_posed = v_posed.reshape(1, v_posed.rows / 3) + v_template.reshape(1, v_template.rows / 3);  //reshape to 6890x3 + 6890x3
    }

//    std::cout << "weight: " << weights.size << " and channels: " << weights.channels() << std::endl;

//    std::cout << v_posed.row(0) << std::endl;

    v_and_J vj = verts_core(full_pose, v_posed, J, weights, kintree_table, true);

//    std::cout << vj.v.row(0) << std::endl;
//    std::cout << "prior_v: " << vj.v.size << "      prior_Jtr: " << vj.Jtr.size << std::endl;

//    std::cout << "trans: " << trans << std::endl;

    for (size_t i = 0; i < vj.v.rows; i++)
    {
        vj.v.row(i) += trans.t();
    }

    for (size_t i = 0; i < vj.Jtr.rows; i++)
    {
        vj.Jtr.row(i) += trans.t();
    }
//    std::cout << "after_v: " << vj.v.size << "      after_Jtr: " << vj.Jtr.size << std::endl;
    //vj.v += trans;

    /*std::cout << "J_regressor_prior: " << J_regressor_prior.size << std::endl;
    std::cout << "f: " << f.size << std::endl;
    std::cout << "J_regressor: " << J_regressor.size << std::endl;
    std::cout << "kintree_table: " << kintree_table.size << std::endl;
    std::cout << "J: " << J.size << std::endl;
    std::cout << "weights_prior: " << weights_prior.size << std::endl;
    std::cout << "weights: " << weights.size << std::endl;
    std::cout << "vert_sym_idxs: " << vert_sym_idxs.size << std::endl;
    std::cout << "posedirs: " << posedirs.size << std::endl;
    std::cout << "v_template: " << v_template.size << std::endl;
    std::cout << "shape: " << shapedirs.size << " with " << shapedirs.channels() << " channels." << std::endl;*/

    smpl.model = vj.v;
//    model.J_regressor_prior = J_regressor_prior;
    smpl.pose = pose_coeffs;
    smpl.f = f;
    smpl.J_regressor = J_regressor;
    smpl.betas = betas;
    smpl.kintree_table = kintree_table;
    smpl.J = vj.Jtr;
    smpl.v_shaped = v_shaped;
//    model.weights_prior = weights_prior;
    smpl.trans = trans;
    smpl.v_posed = v_posed;
    smpl.weights = weights;
//    model.vert_sym_idxs = vert_sym_idxs;
    smpl.posedirs = posedirs;
    smpl.v_template = v_template;
    smpl.shapedirs = shapedirs;

//    std::cout << pose << std::endl;
//    std::cout << "----------------------------------------------------------------------------------------------------------" << std::endl;
//    std::cout << "model: " << model.model << std::endl;
//    std::cout << "J_regressor_prior: " << model.J_regressor_prior.size << std::endl;
//    std::cout << "pose: " << model.pose.size << std::endl;
//    std::cout << "f: " << model.f.size << std::endl;
//    std::cout << "J_regressor: " << model.J_regressor.size << std::endl;
//    std::cout << "betas: " << model.betas.size << std::endl;
//    std::cout << "kintree_table: " << model.kintree_table.size << std::endl;
//    std::cout << "J: " << model.J.size << std::endl;
//    std::cout << "v_shaped: " << model.v_shaped.size << std::endl;
//    std::cout << "weights_prior: " << model.weights_prior.size << std::endl;
//    std::cout << "trans: " << model.trans.size << std::endl;
//    std::cout << "v_posed: " << model.v_posed.size << std::endl;
//    std::cout << "weights: " << model.weights.size << std::endl;
//    std::cout << "vert_sym_idxs: " << model.vert_sym_idxs.size << std::endl;
//    std::cout << "posedirs: " << model.posedirs.size << std::endl;
//    std::cout << "v_template: " << model.v_template.size << std::endl;
//    std::cout << "shapedirs: " << model.shapedirs.size << std::endl;

//    std::cout << "----------------------------------------------------------------------------------------------------------" << std::endl;
//
//    std::ofstream meshfile ("SMPL+H_Mesh.obj");
//    if (meshfile.is_open())
//    {
//        for (size_t i = 0; i < model.model.rows; i++)
//        {
//            meshfile << "v " << model.model.at<double>(i, 0) << " " << model.model.at<double>(i, 1) << " " << model.model.at<double>(i, 2) << "\n";
//        }
//        for (size_t i = 0; i < model.f.rows; i++)
//        {
//            meshfile << "f " << model.f.at<double>(i, 0) + 1 << " " << model.f.at<double>(i, 1) + 1 << " " << model.f.at<double>(i, 2) + 1 << "\n";
//        }
//        std::cout << "Finish writing SMPL+H Mesh!" << std::endl;
//    }
//    else std::cout << "Unable to open file";
}