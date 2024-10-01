#include "VectorWrite.h"
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TMatrixD.h>

using namespace std;

void VectorWrite::OpenFile(const std::string& file_name, TFile*& m_file, TTree*& m_tree) {
// Open the ROOT file
m_file = new TFile(file_name.c_str(), "RECREATE");
if (m_file->IsZombie()) {
    std::cerr << "Failed to open file: " << file_name << std::endl;
    return;
}

// Initialize the tree before branching
m_tree = new TTree("tree", "Tree for storing events");

// Now create branches
m_tree->Branch("run_id", &run_id, "run_id/I");
m_tree->Branch("spill_id", &spill_id, "spill_id/I");
m_tree->Branch("event_id", &event_id, "event_id/I");
m_tree->Branch("fpga_bits", fpga_bits, "fpga_bits[5]/I");
m_tree->Branch("nim_bits", nim_bits, "nim_bits[5]/I");

m_tree->Branch("hit_id", &hit_id);
m_tree->Branch("detector_id", &detector_id);
m_tree->Branch("element_id", &element_id);
m_tree->Branch("tdc_time", &tdc_time);
m_tree->Branch("drift_distance", &drift_distance);

m_tree->Branch("track_id", &track_id);
m_tree->Branch("charge", &charge);
m_tree->Branch("x_vtx", &x_vtx);
m_tree->Branch("y_vtx", &y_vtx);
m_tree->Branch("z_vtx", &z_vtx);
m_tree->Branch("px_vtx", &px_vtx);
m_tree->Branch("py_vtx", &py_vtx);
m_tree->Branch("pz_vtx", &pz_vtx);
   //m_tree->Branch("matrix", &new_matrix);
   
}

int main() {
    VectorWrite writer;
    TFile* m_file = nullptr;
    TTree* m_tree = nullptr;

    writer.OpenFile("LinVector.root", m_file, m_tree);

    // Open the existing file with the tree to read from
    TFile* file = TFile::Open("/seaquest/users/mhossain/Universal/gen/vector_sim.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Failed to open vector_sim.root" << std::endl;
        return 1; // Exit if the file can't be opened
    }
    TTree* tree = static_cast<TTree*>(file->Get("tree"));

    // Declare vectors for reading hit and track data
    std::vector<int>* hit_id = nullptr;
    std::vector<int>* detector_id = nullptr;
    std::vector<int>* element_id = nullptr;
    std::vector<double>* tdc_time = nullptr;
    std::vector<double>* drift_distance = nullptr;

    std::vector<int>* track_id = nullptr;
    std::vector<double>* x_vtx = nullptr;
    std::vector<double>* y_vtx = nullptr;
    std::vector<double>* z_vtx = nullptr;
    std::vector<double>* px_vtx = nullptr;
    std::vector<double>* py_vtx = nullptr;
    std::vector<double>* pz_vtx = nullptr;

    // Declare additional variables
    int fpga_bits[5];
    int nim_bits[5];
    int run_id, spill_id, event_id;

    // Set branch addresses
    tree->SetBranchAddress("run_id", &run_id);
    tree->SetBranchAddress("spill_id", &spill_id);
    tree->SetBranchAddress("event_id", &event_id);
    tree->SetBranchAddress("fpga_bits", fpga_bits);
    tree->SetBranchAddress("nim_bits", nim_bits);

    tree->SetBranchAddress("hit_id", &hit_id);
    tree->SetBranchAddress("detector_id", &detector_id);
    tree->SetBranchAddress("element_id", &element_id);
    tree->SetBranchAddress("tdc_time", &tdc_time);
    tree->SetBranchAddress("drift_distance", &drift_distance);

    tree->SetBranchAddress("track_id", &track_id);
    tree->SetBranchAddress("x_vtx", &x_vtx);
    tree->SetBranchAddress("y_vtx", &y_vtx);
    tree->SetBranchAddress("z_vtx", &z_vtx);
    tree->SetBranchAddress("px_vtx", &px_vtx);
    tree->SetBranchAddress("py_vtx", &py_vtx);
    tree->SetBranchAddress("pz_vtx", &pz_vtx);

    TMatrixD* matrix = nullptr;
    tree->SetBranchAddress("matrix", &matrix);

    Long64_t nentries = tree->GetEntries();

    // Process each entry
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);


	                //writer.evt.matrix = matrix; 
                writer.run_id = run_id;
                writer.spill_id = spill_id;
                writer.event_id = event_id;

                  for (int ii = 0; ii < 5; ii++) {
                writer.fpga_bits[ii] = fpga_bits[ii] ;
                writer.nim_bits[ii] =  nim_bits[ii];
	  }

        // Clear vectors for new event
        writer.hit_id.clear();
        writer.detector_id.clear();
        writer.element_id.clear();
        writer.tdc_time.clear();
        writer.drift_distance.clear();

        writer.track_id.clear();
        writer.x_vtx.clear();
        writer.y_vtx.clear();
        writer.z_vtx.clear();
        writer.px_vtx.clear();
        writer.py_vtx.clear();
        writer.pz_vtx.clear();



	



        // Fill hit data
        for (size_t j = 0; j < tdc_time->size(); j++) {
            //std::cout << "Event " << i << ", Hit " << j << ", TDC Time: " << tdc_time->at(j) << std::endl;

            writer.hit_id.push_back(detector_id->at(j));
            writer.element_id.push_back(element_id->at(j));
            writer.tdc_time.push_back(tdc_time->at(j));
            writer.drift_distance.push_back(drift_distance->at(j));
        }

        // Fill track data
        for (size_t j = 0; j < track_id->size(); j++) {
            writer.track_id.push_back(track_id->at(j));
            writer.x_vtx.push_back(x_vtx->at(j));
            writer.y_vtx.push_back(y_vtx->at(j));
            writer.z_vtx.push_back(z_vtx->at(j));
            writer.px_vtx.push_back(px_vtx->at(j));
            writer.py_vtx.push_back(py_vtx->at(j));
            writer.pz_vtx.push_back(pz_vtx->at(j));
        }
        m_tree->Fill();
    }
    m_file->Write("", TObject::kOverwrite);
    std::cout << "Data written to the file." << std::endl;
    m_file->Close();
    file->Close();
    delete m_file;
    delete file;
    return 0;
}

