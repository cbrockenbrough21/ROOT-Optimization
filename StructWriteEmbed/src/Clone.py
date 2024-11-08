import ROOT

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
output_file.SetCompressionAlgorithm(1)  # Options: kZLIB, kLZMA, kLZ4, etc.
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
