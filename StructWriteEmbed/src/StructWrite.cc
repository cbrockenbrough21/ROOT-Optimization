#include <TStreamerInfo.h>
#include "StructWrite.h"
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <TMatrixD.h>
#include <fstream>
#include <ROOT/RDataFrame.hxx>
#include <TROOT.h>  // For gROOT

using namespace std;

// Include ROOT implementation macro
ClassImp(StructWrite)

std::uintmax_t getFileSize(const std::string &filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); // Returns the file size
}

void StructWrite::OpenFile(const std::string &file_name, TFile *&m_file, TTree *&m_tree)
{
    if (m_file && m_tree) {
        std::cout << "File and tree already opened, skipping." << std::endl;
        return;
    }

    std::cout << "Attempting to open file: " << file_name << std::endl;

    m_file = new TFile(file_name.c_str(), "RECREATE");
    if (!m_file || m_file->IsZombie()) {
        std::cerr << "Error: Could not create file " << file_name << std::endl;
        exit(1);
    } else {
        std::cout << "File " << m_file->GetName() << " opened successfully." << std::endl;
    }

    // Enable multithreading
    ROOT::EnableImplicitMT();

    m_file->SetCompressionAlgorithm(compression_algo);
    m_file->SetCompressionLevel(compression_level);

    m_tree = new TTree("tree", "Tree for storing events");

    if (!m_tree) {
        std::cerr << "Error: Could not create tree" << std::endl;
        exit(1);
    } else {
        std::cout << m_tree->GetName() << " created successfully." << std::endl;
    }

    m_tree->Branch("evt", &evt);
    m_tree->Branch("list_hit", &list_hit);
    m_tree->Branch("list_trk", &list_trk);

    m_tree->SetAutoFlush(m_auto_flush);
    m_tree->SetBasketSize("*", m_basket_size);
}

// Example of filling data and writing to file
int main(int argc, char *argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <compression_algo> <compression_level> <basket_size> <autoflush>" << endl;
        return 1;
    }

    // Get compression parameters from command-line arguments
    int compression_algo = atoi(argv[1]);
    int compression_level = atoi(argv[2]);
    int basket_size = atoi(argv[3]);
    Long64_t autoflush = atoll(argv[4]); // AutoFlush setting (0 means disabled)

    StructWrite writer;
    writer.compression_algo = compression_algo;
    writer.compression_level = compression_level;
    writer.m_basket_size = basket_size;
    writer.m_auto_flush = autoflush;

    TFile *m_file = nullptr;
    TTree *m_tree = nullptr;

    writer.OpenFile("struct_sim.root", m_file, m_tree);


    //TStreamerInfo::Optimize(false);  // Disable optimization for full info
    // Embed the streamer information for custom classes
    gROOT->GetClass("EventData")->GetStreamerInfo();
    gROOT->GetClass("HitData")->GetStreamerInfo();
    gROOT->GetClass("TrackData")->GetStreamerInfo();

    // Open the input file and retrieve the tree
    TFile *file = TFile::Open("../../gen/vector_sim.root");
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open input file!" << std::endl;
        return 1;
    }

    TTree *tree = (TTree *)file->Get("tree");
    if (!tree) {
        std::cerr << "Error: TTree 'tree' not found in the input file!" << std::endl;
        return 1;
    }


    std::vector<int> *hit_id = nullptr;
    std::vector<int> *detector_id = nullptr;
    std::vector<int> *element_id = nullptr;
    std::vector<double> *tdc_time = nullptr;
    std::vector<double> *drift_distance = nullptr;

    std::vector<double> *track_id = nullptr;
    std::vector<double> *charge = nullptr;
    std::vector<double> *x_vtx = nullptr;
    std::vector<double> *y_vtx = nullptr;
    std::vector<double> *z_vtx = nullptr;
    std::vector<double> *px_vtx = nullptr;
    std::vector<double> *py_vtx = nullptr;
    std::vector<double> *pz_vtx = nullptr;

    int fpga_bits[5]; // assuming the arrays have 5 elements
    int nim_bits[5];
    int run_id;
    int spill_id;
    int event_id;

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

    tree->SetBranchAddress("charge", &charge);
    tree->SetBranchAddress("track_id", &track_id);
    tree->SetBranchAddress("x_vtx", &x_vtx);
    tree->SetBranchAddress("y_vtx", &y_vtx);
    tree->SetBranchAddress("z_vtx", &z_vtx);
    tree->SetBranchAddress("px_vtx", &px_vtx);
    tree->SetBranchAddress("py_vtx", &py_vtx);
    tree->SetBranchAddress("pz_vtx", &pz_vtx);

    Long64_t nentries = tree->GetEntries();

    TStopwatch timer;
    timer.Start();

    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        writer.evt.run_id = run_id;
        writer.evt.spill_id = spill_id;
        writer.evt.event_id = event_id;

        for (int ii = 0; ii < 5; ii++) {
            writer.evt.fpga_bits[ii] = fpga_bits[ii];
            writer.evt.nim_bits[ii] = nim_bits[ii];
        }

        writer.list_hit.clear();
        for (size_t j = 0; j < tdc_time->size(); j++) {
            HitData hit;
            hit.detector_id = detector_id->at(j);
            hit.element_id = element_id->at(j);
            hit.tdc_time = tdc_time->at(j);
            hit.drift_distance = drift_distance->at(j);
            writer.list_hit.push_back(hit);
        }

        writer.list_trk.clear();
        for (size_t j = 0; j < track_id->size(); j++) {
            TrackData trk;
            trk.charge = charge->at(j);
            trk.track_id = track_id->at(j);
            trk.x_vtx = x_vtx->at(j);
            trk.y_vtx = y_vtx->at(j);
            trk.z_vtx = z_vtx->at(j);
            trk.px_vtx = px_vtx->at(j);
            trk.py_vtx = py_vtx->at(j);
            trk.pz_vtx = pz_vtx->at(j);
            writer.list_trk.push_back(trk);
        }

        m_tree->Fill();
    }

    m_file->Write("", TObject::kOverwrite);
    m_file->WriteObject(gROOT->GetClass("EventData")->GetStreamerInfo(), "EventData");
    m_file->WriteObject(gROOT->GetClass("HitData")->GetStreamerInfo(), "HitData");
    m_file->WriteObject(gROOT->GetClass("TrackData")->GetStreamerInfo(), "TrackData");  

    m_file->Close();
    file->Close();
    delete file;

    // Stop the timer
    timer.Stop();
    double real_time = timer.RealTime();

    // Get the file size
    std::uintmax_t file_size = getFileSize("struct_sim.root");

    // Print out the file size and write time in the expected format
    cout << "WRITE_TIME=" << real_time << " FILE_SIZE=" << fixed << setprecision(3) 
         << static_cast<double>(file_size) / (1024 * 1024) << "MB" << endl;

    delete m_file;
    return 0;
}
