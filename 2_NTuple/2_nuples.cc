#include <TFile.h>
#include <TNtuple.h>
#include <TTree.h>
#include <iostream>
#include <vector>

using namespace std;

// Function to open the ROOT file and create the NTuples for event-hit and event-track data
void NTupleWrite::OpenFile(const std::string &file_name, TFile *&m_file, TNtuple *&event_hit_ntuple, TNtuple *&event_track_ntuple) {
    if (m_file && event_hit_ntuple && event_track_ntuple) {
        std::cout << "File and ntuples already opened, skipping." << std::endl;
        return;
    }

    m_file = new TFile(file_name.c_str(), "RECREATE");
    if (!m_file || m_file->IsZombie()) {
        std::cerr << "Error: Could not create file " << file_name << std::endl;
        exit(1);
    } else {
        std::cout << "File " << m_file->GetName() << " opened successfully." << std::endl;
    }

    m_file->SetCompressionAlgorithm(ROOT::kLZMA);
    m_file->SetCompressionLevel(m_compression_level);

    event_hit_ntuple = new TNtuple("event_hit_ntuple", "Event and Hit Data", "event_id:run_id:spill_id:hit_id:detector_id:element_id:tdc_time:drift_distance");
    event_track_ntuple = new TNtuple("event_track_ntuple", "Event and Track Data", "event_id:run_id:spill_id:track_id:charge:x_vtx:y_vtx:z_vtx:px_vtx:py_vtx:pz_vtx");

    if (!event_hit_ntuple || !event_track_ntuple) {
        std::cerr << "Error: Could not create NTuples" << std::endl;
        exit(1);
    }

    event_hit_ntuple->SetAutoFlush(m_auto_flush);
    event_track_ntuple->SetAutoFlush(m_auto_flush);
    event_hit_ntuple->SetBasketSize("*", m_basket_size);
    event_track_ntuple->SetBasketSize("*", m_basket_size);


    std::cout << "NTuples created successfully with compression level " << m_compression_level << " and basket size " << m_basket_size << "." << std::endl;
}


// Example of filling data and writing to file
int main()
{
    TFile *m_file = nullptr;
    TNtuple *event_hit_ntuple = nullptr;
    TNtuple *event_track_ntuple = nullptr;

    // Open the output file and create the NTuples
    OpenFile("../maker/ntuple_output.root", m_file, event_hit_ntuple, event_track_ntuple);

    // Open the input ROOT file and get the tree
    TFile *file = TFile::Open("../../gen/vector_sim.root");
    if (!file || file->IsZombie()) {
        cerr << "Error: Could not open input file!" << endl;
        return 1;
    }
    TTree *tree = (TTree *)file->Get("tree");

    // Declare variables for branches
    int run_id, spill_id, event_id;
    vector<int> *hit_id = nullptr, *detector_id = nullptr, *element_id = nullptr;
    vector<double> *tdc_time = nullptr, *drift_distance = nullptr, *drift_time = nullptr;
    vector<int> *track_id = nullptr, *charge = nullptr;
    vector<double> *x_vtx = nullptr, *y_vtx = nullptr, *z_vtx = nullptr;
    vector<double> *px_vtx = nullptr, *py_vtx = nullptr, *pz_vtx = nullptr;
    int fpga_bits[5], nim_bits[5];

    // Set branch addresses for event-level variables
    tree->SetBranchAddress("event_id", &event_id);
    tree->SetBranchAddress("run_id", &run_id);
    tree->SetBranchAddress("spill_id", &spill_id);
    tree->SetBranchAddress("fpga_bits", fpga_bits);
    tree->SetBranchAddress("nim_bits", nim_bits);

    // Set branch addresses for hit-level variables
    tree->SetBranchAddress("hit_id", &hit_id);
    tree->SetBranchAddress("detector_id", &detector_id);
    tree->SetBranchAddress("element_id", &element_id);
    tree->SetBranchAddress("tdc_time", &tdc_time);
    tree->SetBranchAddress("drift_time", &drift_time);
    tree->SetBranchAddress("drift_distance", &drift_distance);

    // Set branch addresses for track-level variables
    tree->SetBranchAddress("track_id", &track_id);
    tree->SetBranchAddress("charge", &charge);
    tree->SetBranchAddress("x_vtx", &x_vtx);
    tree->SetBranchAddress("y_vtx", &y_vtx);
    tree->SetBranchAddress("z_vtx", &z_vtx);
    tree->SetBranchAddress("px_vtx", &px_vtx);
    tree->SetBranchAddress("py_vtx", &py_vtx);
    tree->SetBranchAddress("pz_vtx", &pz_vtx);

    // Loop over entries and fill the NTuples
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        // Fill hit data with event-level info
        for (size_t j = 0; j < hit_id->size(); j++) {
            event_hit_ntuple->Fill(event_id, run_id, spill_id, fpga_bits[0], nim_bits[0], 
                                   hit_id->at(j), detector_id->at(j), element_id->at(j), 
                                   tdc_time->at(j), drift_distance->at(j), drift_time->at(j));
        }

        // Fill track data with event-level info
        for (size_t j = 0; j < track_id->size(); j++) {
            event_track_ntuple->Fill(event_id, run_id, spill_id, fpga_bits[0], nim_bits[0], 
                                     track_id->at(j), charge->at(j), x_vtx->at(j), y_vtx->at(j), 
                                     z_vtx->at(j), px_vtx->at(j), py_vtx->at(j), pz_vtx->at(j));
        }
    }

    // Write and close the output file
    m_file->Write("", TObject::kOverwrite);
    cout << "Data written to the file." << endl;
    m_file->Close();
    file->Close();
    delete m_file;
    return 0;
}
