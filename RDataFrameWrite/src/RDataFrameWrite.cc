#include <ROOT/RDataFrame.hxx>
#include <TFile.h>
#include <TStopwatch.h>
#include <iostream>
#include <vector>
#include <fstream> 

using namespace std;

std::uintmax_t getFileSize(const std::string &filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); // Returns the file size
}

int main(int argc, char *argv[]) {
    // Check if we received the correct number of arguments
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <compression_algo> <compression_level> <autoflush>" << endl;
        return 1;
    }

    // Get the compression algorithm, level, and autoflush from the command-line arguments
    int compression_algo = atoi(argv[1]);    // e.g., 1 = kZLIB, 2 = kLZMA, 4 = kLZ4, 5 = kZSTD
    int compression_level = atoi(argv[2]);   // Compression level (0-9)
    Long64_t autoflush = atoll(argv[3]);     // AutoFlush setting (0 means "Off")

    // Open the input ROOT file
    TFile *input_file = TFile::Open("../../gen/vector_sim100k.root");
    if (!input_file || input_file->IsZombie()) {
        cerr << "Error: Could not open input file!" << endl;
        return 1;
    }

    // Enable multithreading
    ROOT::EnableImplicitMT();

    // Initialize RDataFrame to process the input tree
    ROOT::RDataFrame input_df("tree", input_file);

    // Prepare the output options for compression and snapshot
    ROOT::RDF::RSnapshotOptions snapshot_options;
    snapshot_options.fCompressionAlgorithm = static_cast<ROOT::ECompressionAlgorithm>(compression_algo);
    snapshot_options.fCompressionLevel = compression_level;
    snapshot_options.fAutoFlush = autoflush;  // Set AutoFlush

    // Output file path
    std::string output_file_path = "../maker/rdataframe_output.root";

    // Define columns to save in the new tree (matching the input tree structure)
    std::vector<std::string> columns_to_save = {
        "run_id", "spill_id", "event_id", "fpga_bits", "nim_bits",
        "hit_id", "detector_id", "element_id", "tdc_time", "drift_distance", "drift_time",
        "track_id", "charge", "x_vtx", "y_vtx", "z_vtx", "px_vtx", "py_vtx", "pz_vtx"
    };

    // Start timing the writing process
    TStopwatch timer;
    timer.Start();

    // Use RDataFrame's Snapshot to write the output file efficiently
    input_df.Snapshot("tree", output_file_path, columns_to_save, snapshot_options);

    // Stop timing and output the time taken
    timer.Stop();
    double real_time = timer.RealTime();

    // Get the file size
    std::uintmax_t file_size = getFileSize(output_file_path);

    // Output simplified, easier-to-parse result
    cout << "WRITE_TIME=" << real_time 
        << " FILE_SIZE=" << fixed << setprecision(3) << static_cast<double>(file_size) / (1024 * 1024) << "MB" << endl;

    input_file->Close();
    delete input_file;

    return 0;
}
