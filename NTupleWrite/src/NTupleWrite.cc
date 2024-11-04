#include <TFile.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <iostream>
#include <vector>

using namespace std;

int main()
{
    // Variables for compression and performance tuning
    int compression_algo = ROOT::kLZMA; // Compression algorithm (e.g., kLZMA)
    int compression_level = 1;          // Compression level (0 = no compression, 9 = maximum)
    int basket_size = 32000;            // Basket size for ROOT TTree (default is 32 KB)

    // Create pointers for the ROOT file and NTuples
    TFile *m_file = new TFile("../maker/ntuple_output.root", "RECREATE");
    TNtuple *event_ntuple = nullptr;
    TNtuple *hit_ntuple = nullptr;
    TNtuple *track_ntuple = nullptr;

    // Check if the file was created successfully
    if (!m_file || m_file->IsZombie()) {
        cerr << "Error: Could not create file ../maker/ntuple_output.root" << endl;
        return 1;
    } else {
        cout << "File " << m_file->GetName() << " opened successfully." << endl;
    }

    // Set the compression algorithm and level
    m_file->SetCompressionAlgorithm(compression_algo);
    m_file->SetCompressionLevel(compression_level);

    // Create the NTuples for event, hit, and track data
    event_ntuple = new TNtuple("event_ntuple", "Event Data", "event_id:run_id:spill_id:fpga_bits:nim_bits");
    hit_ntuple = new TNtuple("hit_ntuple", "Hit Data", "event_id:hit_id:detector_id:element_id:tdc_time:drift_distance:drift_time");
    track_ntuple = new TNtuple("track_ntuple", "Track Data", "event_id:track_id:charge:x_vtx:y_vtx:z_vtx:px_vtx:py_vtx:pz_vtx");

    if (!event_ntuple || !hit_ntuple || !track_ntuple) {
        cerr << "Error: Could not create NTuples" << endl;
        return 1;
    }

    // Set AutoFlush and BasketSize for better performance
    event_ntuple->SetAutoFlush(1000);
    hit_ntuple->SetAutoFlush(1000);
    track_ntuple->SetAutoFlush(1000);

    event_ntuple->SetBasketSize("*", basket_size);
    hit_ntuple->SetBasketSize("*", basket_size);
    track_ntuple->SetBasketSize("*", basket_size);

    cout << "NTuples created successfully with compression level " << compression_level 
         << ", compression algorithm " << compression_algo << ", and basket size " << basket_size << "." << endl;

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
    vector<double> *tdc_time = nullptr, *drift_distance = nullptr;
    vector<double> *drift_time = nullptr;
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

    // Measure the time for filling the NTuples
    TStopwatch timer;
    timer.Start();

    // Loop over entries and fill the NTuples
    Long64_t nentries = tree->GetEntries();
    for (Long64_t i = 0; i < nentries; i++) {
        tree->GetEntry(i);

        // Fill event data
        event_ntuple->Fill(event_id, run_id, spill_id, fpga_bits[0], nim_bits[0]);

        // Fill hit data
        for (size_t j = 0; j < hit_id->size(); j++) {
            hit_ntuple->Fill(event_id, hit_id->at(j), detector_id->at(j), element_id->at(j),
                             tdc_time->at(j), drift_distance->at(j), drift_time->at(j));
        }

        // Fill track data
        for (size_t j = 0; j < track_id->size(); j++) {
            track_ntuple->Fill(event_id, track_id->at(j), charge->at(j), x_vtx->at(j), y_vtx->at(j),
                               z_vtx->at(j), px_vtx->at(j), py_vtx->at(j), pz_vtx->at(j));
        }
    }

    // Stop the timer and calculate the elapsed time
    timer.Stop();
    double compression_time = timer.RealTime();
    cout << "Data written to the file in " << compression_time << " seconds." << endl;

      // Write and close the output file
    m_file->Write("", TObject::kOverwrite);
    cout << "Output file size: " << m_file->GetSize() / (1024.0 * 1024.0) << " MB" << endl;
    m_file->Close();
    file->Close();
    delete m_file;
    return 0;
}


/* 



#include <TFile.h>
#include <TNtuple.h>
#include <TTree.h>
#include <iostream>
#include <vector>

using namespace std;

// Function to open the ROOT file and create the NTuples for event, hit, and track data
void OpenFile(const std::string& file_name, TFile*& m_file, TNtuple*& event_ntuple, TNtuple*& hit_ntuple, TNtuple*& track_ntuple) {
    // Check if the file and ntuples are already open
    if (m_file && event_ntuple && hit_ntuple && track_ntuple) {
        cout << "File and ntuples already opened, skipping." << endl;
        return;
    }

    // Attempt to open the file
    cout << "Attempting to open file: " << file_name << endl;
    m_file = new TFile(file_name.c_str(), "RECREATE");
    if (!m_file || m_file->IsZombie()) {
        cerr << "Error: Could not create file " << file_name << endl;
        exit(1);
    } else {
        cout << "File " << m_file->GetName() << " opened successfully." << endl;
    }

    // Create the NTuples for event, hit, and track data
    event_ntuple = new TNtuple("event_ntuple", "Event Data", "event_id:run_id:spill_id:fpga_bits:nim_bits");
    hit_ntuple = new TNtuple("hit_ntuple", "Hit Data", "event_id:hit_id:detector_id:element_id:tdc_time:drift_distance:drift_time");
    track_ntuple = new TNtuple("track_ntuple", "Track Data", "event_id:track_id:charge:x_vtx:y_vtx:z_vtx:px_vtx:py_vtx:pz_vtx");

    if (!event_ntuple || !hit_ntuple || !track_ntuple) {
        cerr << "Error: Could not create NTuples" << endl;
        exit(1);
    } else {
        cout << "NTuples created successfully." << endl;
    }
}

// Example of filling data and writing to file
int main()
{
    TFile *m_file = nullptr;
    TNtuple *event_ntuple = nullptr;
    TNtuple *hit_ntuple = nullptr;
    TNtuple *track_ntuple = nullptr;

    // Open the output file and create the NTuples
    OpenFile("../maker/ntuple_output.root", m_file, event_ntuple, hit_ntuple, track_ntuple);

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
    vector<double> *tdc_time = nullptr, *drift_distance = nullptr;
    vector<double> *drift_time = nullptr;
    vector<int> *track_id = nullptr, *charge = nullptr;
    vector<double> *x_vtx = nullptr, *y_vtx = nullptr, *z_vtx = nullptr;
    vector<double> *px_vtx = nullptr, *py_vtx = nullptr, *pz_vtx = nullptr;
    int fpga_bits[5], nim_bits[5];

    // Set branch addresses
    //Event variables
    tree->SetBranchAddress("event_id", &event_id);
    tree->SetBranchAddress("run_id", &run_id);
    tree->SetBranchAddress("spill_id", &spill_id);
    tree->SetBranchAddress("fpga_bits", fpga_bits);
    tree->SetBranchAddress("nim_bits", nim_bits);

    //Hit variables
    tree->SetBranchAddress("hit_id", &hit_id);
    tree->SetBranchAddress("detector_id", &detector_id);
    tree->SetBranchAddress("element_id", &element_id);
    tree->SetBranchAddress("tdc_time", &tdc_time);
    tree->SetBranchAddress("drift_time", &drift_time);
    tree->SetBranchAddress("drift_distance", &drift_distance);

    //Track variables
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

        // Fill event data
        event_ntuple->Fill(event_id, run_id, spill_id, fpga_bits[0], nim_bits[0]);  // Assuming 1st element for simplification

        // Fill hit data
        for (size_t j = 0; j < hit_id->size(); j++) {
            hit_ntuple->Fill(event_id, hit_id->at(j), detector_id->at(j), element_id->at(j),
                 tdc_time->at(j), drift_distance->at(j), drift_time->at(j));
        }

        // Fill track data
        for (size_t j = 0; j < track_id->size(); j++) {
            track_ntuple->Fill(event_id, track_id->at(j), charge->at(j), x_vtx->at(j), y_vtx->at(j),
                               z_vtx->at(j), px_vtx->at(j), py_vtx->at(j), pz_vtx->at(j));
        }
    }

    // Write and close the output file
    m_file->Write("", TObject::kOverwrite);
    cout << "Data written to the file." << endl;
    m_file->Close();
    file->Close();
    delete m_file;
    delete file;

    return 0;
} */