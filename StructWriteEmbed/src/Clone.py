import ROOT

# Declare the custom structs in the script to eliminate the need for the shared library
ROOT.gInterpreter.Declare('''
struct EventData {
    int run_id;
    int spill_id;
    int event_id;
    std::vector<int> fpga_bits;
    std::vector<int> nim_bits;
    EventData() {}
    virtual ~EventData() {}
    ClassDef(EventData, 1);
};
''')

ROOT.gInterpreter.Declare('''
struct HitData {
    int detector_id;
    int element_id;
    double tdc_time;
    double drift_distance;
    HitData() {}
    virtual ~HitData() {}
    ClassDef(HitData, 1);
};
''')

ROOT.gInterpreter.Declare('''
struct TrackData {
    int track_id;
    int charge;
    double x_vtx;
    double y_vtx;
    double z_vtx;
    double px_vtx;
    double py_vtx;
    double pz_vtx;
    TrackData() {}
    virtual ~TrackData() {}
    ClassDef(TrackData, 1);
};
''')

# Open the input ROOT file
input_file = ROOT.TFile.Open("../maker/struct_sim.root")
if not input_file or input_file.IsZombie():
    print("Error: Could not open input file.")
    exit(1)

# Get the input tree
input_tree = input_file.Get("tree")
if not input_tree:
    print("Error: Could not find 'tree' in input file.")
    input_file.Close()
    exit(1)

# Get the number of entries in the input tree
n_entries = input_tree.GetEntries()
print(f"Total entries in the input file: {n_entries}")

# Create a new ROOT file
output_file = ROOT.TFile("new_struct_sim.root", "RECREATE")
if not output_file or output_file.IsZombie():
    print("Error: Could not create output file.")
    input_file.Close()
    exit(1)

# Set compression algorithm and level (optional)
output_file.SetCompressionAlgorithm(ROOT.kLZMA)  # Options: kZLIB, kLZMA, kLZ4, etc.
output_file.SetCompressionLevel(4)  # Level between 0 (no compression) and 9 (maximum compression)

# Clone the input tree structure and copy the data
output_file.cd()
output_tree = input_tree.CloneTree(0)  # Create an empty clone of the input tree

# Optionally, set AutoFlush and BasketSize for optimization
output_tree.SetAutoFlush(30000000)  # Flush after ~30 MB of data
# output_tree.SetBasketSize("*", 32000)  # Set basket size for all branches (adjust as needed)

# Copy all entries from the input tree to the output tree
for i in range(n_entries):
    if input_tree.GetEntry(i) <= 0:
        print(f"Warning: Could not read entry {i}")
        continue
    output_tree.Fill()

# Write the class definitions into the output file to make it self-contained
ROOT.gROOT.GetClass("EventData").Write()
ROOT.gROOT.GetClass("HitData").Write()
ROOT.gROOT.GetClass("TrackData").Write()

# Write the output tree to the new file
output_tree.Write()
output_file.Close()
input_file.Close()

print("Data successfully copied to the new ROOT file.")
