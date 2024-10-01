#ifndef VECTORWRITE_H
#define VECTORWRITE_H

#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <TMatrixD.h>
#include <vector>

class VectorWrite {
public:
    VectorWrite() = default;
    void OpenFile(const std::string& file_name, TFile*& m_file, TTree*& m_tree);
    
    std::vector<int> hit_id;
    std::vector<int> detector_id;
    std::vector<int> element_id;
    std::vector<double> tdc_time;
    std::vector<double> drift_distance;

    std::vector<int> track_id;
    std::vector<int> charge;
    std::vector<double> x_vtx;
    std::vector<double> y_vtx;
    std::vector<double> z_vtx;
    std::vector<double> px_vtx;
    std::vector<double> py_vtx;
    std::vector<double> pz_vtx;
    //TMatrixD *new_matrix;
int fpga_bits[5];  // Array for FPGA bits
int nim_bits[5];   // Array for NIM bits
int run_id, spill_id, event_id;

};
#endif // VECTORWRITE_H

