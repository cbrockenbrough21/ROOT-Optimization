#include "VectorWrite.h"
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TMatrixD.h>
#include <fstream>
#include <ROOT/RDataFrame.hxx>


using namespace std;

std::uintmax_t getFileSize(const std::string &filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); // Returns the file size
}


void VectorWrite::OpenFile(const std::string &file_name, TFile *&m_file, TTree *&m_tree)
{
    // Open the ROOT file
    m_file = new TFile(file_name.c_str(), "RECREATE");
    if (m_file->IsZombie())
    {
        std::cerr << "Failed to open file: " << file_name << std::endl;
        return;
    }

    // Enable multithreading
    ROOT::EnableImplicitMT();

	m_file->SetCompressionAlgorithm(compression_algo);
	m_file->SetCompressionLevel(compression_level);

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
    m_tree->Branch("matrix", &new_matrix);

    m_tree->SetAutoFlush(m_auto_flush);
	m_tree->SetBasketSize("*", m_basket_size);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <compression_algo> <compression_level> <basket_size> <auto_flush>" << endl;
        return 1;
    }

    // Get compression parameters from command-line arguments
    int compression_algo = atoi(argv[1]);
    int compression_level = atoi(argv[2]);
    int basket_size = atoi(argv[3]);
    int auto_flush = atoi(argv[4]);


    VectorWrite writer;
    writer.compression_algo = compression_algo;
    writer.compression_level = compression_level;
    writer.m_basket_size = basket_size;
    writer.m_auto_flush = auto_flush;
    TFile *m_file = nullptr;
    TTree *m_tree = nullptr;

    writer.OpenFile("LinVector.root", m_file, m_tree);

    // Open the existing file with the tree to read from
    TFile *file = TFile::Open("../../gen/vector_sim20%.root");
    if (!file || file->IsZombie())
    {
        std::cerr << "Failed to open vector_sim.root" << std::endl;
        return 1; // Exit if the file can't be opened
    }
    TTree *tree = static_cast<TTree *>(file->Get("tree"));

    // Declare vectors for reading hit and track data
    std::vector<int> *hit_id = nullptr;
    std::vector<int> *detector_id = nullptr;
    std::vector<int> *element_id = nullptr;
    std::vector<double> *tdc_time = nullptr;
    std::vector<double> *drift_distance = nullptr;

    std::vector<int> *track_id = nullptr;
    std::vector<double> *x_vtx = nullptr;
    std::vector<double> *y_vtx = nullptr;
    std::vector<double> *z_vtx = nullptr;
    std::vector<double> *px_vtx = nullptr;
    std::vector<double> *py_vtx = nullptr;
    std::vector<double> *pz_vtx = nullptr;

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

    TMatrixD *matrix = nullptr;
    tree->SetBranchAddress("matrix", &matrix);

    Long64_t nentries = tree->GetEntries();

    TStopwatch timer;
	timer.Start();

    // Process each entry
    for (Long64_t i = 0; i < nentries; i++)
    {
        tree->GetEntry(i);
        
        writer.run_id = run_id;
        writer.spill_id = spill_id;
        writer.event_id = event_id;

        for (int ii = 0; ii < 5; ii++)
        {
            writer.fpga_bits[ii] = fpga_bits[ii];
            writer.nim_bits[ii] = nim_bits[ii];
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

        writer.new_matrix->Zero();

        if (matrix) {
            //std::cout << "Entering matrix processing block." << std::endl;
            for (int row = 0; row < writer.max_elements; row++) {
                for (int col = 0; col < writer.max_detectors; col++) {
                    double value = (*matrix)(row, col);
            
                    // Only write non-zero values to new_matrix
                    if (value != 0) {
                        (*writer.new_matrix)(row, col) = value;
                        //std::cout << "Non-zero value at (" << row << ", " << col << "): " << value << std::endl;
                    }
                }
            }
        }

        // Fill hit data
        for (size_t j = 0; j < tdc_time->size(); j++)
        {
            // std::cout << "Event " << i << ", Hit " << j << ", TDC Time: " << tdc_time->at(j) << std::endl;

            writer.hit_id.push_back(detector_id->at(j));
            writer.element_id.push_back(element_id->at(j));
            writer.tdc_time.push_back(tdc_time->at(j));
            writer.drift_distance.push_back(drift_distance->at(j));
        }

        // Fill track data
        for (size_t j = 0; j < track_id->size(); j++)
        {
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

    // Stop the timer
    timer.Stop();
    double real_time = timer.RealTime();

	// Get the file size
    std::uintmax_t file_size = getFileSize("LinVector.root");

    // Print out the file size and write time in the expected format
    cout << "WRITE_TIME=" << real_time << " FILE_SIZE=" << fixed << setprecision(3) 
         << static_cast<double>(file_size) / (1024 * 1024) << "MB" << endl;
    
    delete writer.new_matrix;
    writer.new_matrix = nullptr;

    m_file->Close();
    file->Close();
    delete m_file;
    delete file;
    return 0;
}
