import ROOT

# Open the input ROOT file
input_file = ROOT.TFile.Open("../maker/struct_sim.root")
input_tree = input_file.Get("tree")

# Create a new ROOT file and tree
output_file = ROOT.TFile("new_struct_sim.root", "RECREATE")
output_tree = ROOT.TTree("tree", "Tree for storing events")

# Clone the tree structure from the input tree
output_tree = input_tree.CloneTree(0)  # Create an empty clone of the input tree

# Get the number of entries in the input tree
n_entries = input_tree.GetEntries()
print(f"Total entries in the input file: {n_entries}")

# Loop over the entries in the input tree and fill the output tree
for i in range(n_entries):
    # Read the entry from the input tree
    input_tree.GetEntry(i)
    
    # Access the data directly from the tree
    event_data = input_tree.evt
    hit_list = input_tree.list_hit
    track_list = input_tree.list_trk

    # Optionally, you can modify the data here if needed

    # Fill the output tree with the current entry
    output_tree.Fill()

# Write the output tree to the new file
output_file.Write()
output_file.Close()
input_file.Close()

print("Data successfully copied to the new ROOT file.")
