#include <ROOT/RDataFrame.hxx>
#include <TFile.h>
#include <TStopwatch.h>
#include <iostream>
#include <vector>
#include <fstream>
#include "TreeData.h"  // Include your structs for EventData, HitData, TrackData

using namespace std;

std::uintmax_t getFileSize(const std::string &filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); // Returns the file size
}

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

    // Open the input ROOT file
    TFile *input_file = TFile::Open("../../gen/vector_sim100k.root");
    if (!input_file || input_file->IsZombie()) {
        cerr << "Error: Could not open input file!" << endl;
        return 1;
    }

    // Enable multithreading
    ROOT::EnableImplicitMT(1);

    // Initialize RDataFrame to process the input tree
    ROOT::RDataFrame input_df("tree", input_file);

    // Output file path
    std::string output_file_path = "../maker/struct_rdataframe_output.root";
    
    // Open the output ROOT file
    TFile *output_file = TFile::Open(output_file_path.c_str(), "RECREATE");
    if (!output_file || output_file->IsZombie()) {
        cerr << "Error: Could not create output file!" << endl;
        return 1;
    }

    // Set compression settings for the output file
    output_file->SetCompressionAlgorithm(compression_algo);
    output_file->SetCompressionLevel(compression_level);

    // Create TTree for EventData, HitList, TrackList
    TTree *event_tree = new TTree("event_tree", "Event Data with Hits and Tracks");
    event_tree->SetAutoFlush(autoflush); // Set AutoFlush value

    EventData event_data;
    HitList hit_list;
    TrackList track_list;

    // Set branches for the structs
    TBranch *event_branch = event_tree->Branch("event_data", &event_data);
    event_branch->SetBasketSize(basket_size);
    
    TBranch *hit_branch = event_tree->Branch("hit_list", &hit_list);
    hit_branch->SetBasketSize(basket_size);
    
    TBranch *track_branch = event_tree->Branch("track_list", &track_list);
    track_branch->SetBasketSize(basket_size);

    // Define columns to save in the new tree (matching the input tree structure)
    std::vector<std::string> columns_to_save = {
        "run_id", "spill_id", "event_id", "fpga_bits", "nim_bits",
        "hit_id", "detector_id", "element_id", "tdc_time", "drift_distance",
        "track_id", "charge", "x_vtx", "y_vtx", "z_vtx", "px_vtx", "py_vtx", "pz_vtx"
    };

    // Start timing the writing process
    TStopwatch timer;
    timer.Start();

    // Define a lambda function to process each event and fill the struct-based data
    auto fill_struct = [&](int run_id, int spill_id, int event_id,
                           const ROOT::VecOps::RVec<int>& fpga_bits, 
                           const ROOT::VecOps::RVec<int>& nim_bits,
                           const std::vector<int>& hit_ids, const std::vector<int>& detector_ids, 
                           const std::vector<int>& element_ids,
                           const std::vector<double>& tdc_times, const std::vector<double>& drift_distances,
                           const std::vector<int>& track_ids, const std::vector<int>& charges,
                           const std::vector<double>& x_vtxs, const std::vector<double>& y_vtxs, 
                           const std::vector<double>& z_vtxs,
                           const std::vector<double>& px_vtxs, const std::vector<double>& py_vtxs, 
                           const std::vector<double>& pz_vtxs) {
        // Fill the EventData struct
        event_data.run_id = run_id;
        event_data.spill_id = spill_id;
        event_data.event_id = event_id;

        // Copy values from RVec to vector
        event_data.fpga_bits.assign(fpga_bits.begin(), fpga_bits.end());
        event_data.nim_bits.assign(nim_bits.begin(), nim_bits.end());

        // Fill the HitList struct
        hit_list.clear();
        for (size_t i = 0; i < hit_ids.size(); ++i) {
            HitData hit;
            hit.detector_id = detector_ids[i];
            hit.element_id = element_ids[i];
            hit.tdc_time = tdc_times[i];
            hit.drift_distance = drift_distances[i];
            hit_list.push_back(hit);
        }

        // Fill the TrackList struct
        track_list.clear();
        for (size_t i = 0; i < track_ids.size(); ++i) {
            TrackData track;
            track.track_id = track_ids[i];
            track.charge = charges[i];
            track.x_vtx = x_vtxs[i];
            track.y_vtx = y_vtxs[i];
            track.z_vtx = z_vtxs[i];
            track.px_vtx = px_vtxs[i];
            track.py_vtx = py_vtxs[i];
            track.pz_vtx = pz_vtxs[i];
            track_list.push_back(track);
        }

        // Fill the tree with the structs
        event_tree->Fill();
    };

    // Use RDataFrame's Foreach to process the data and fill the tree
    input_df.Foreach(fill_struct, columns_to_save);

    // Write the tree and close the output file
    event_tree->Write();
    output_file->Close();

    // Stop timing and output the time taken
    timer.Stop();
    double real_time = timer.RealTime();

    // Get the file size
    std::uintmax_t file_size = getFileSize(output_file_path);
    cout << "WRITE_TIME=" << real_time 
         << " FILE_SIZE=" << fixed << setprecision(3) 
         << static_cast<double>(file_size) / (1024 * 1024) << "MB" << endl;

    input_file->Close();
    delete input_file;

    return 0;
}
