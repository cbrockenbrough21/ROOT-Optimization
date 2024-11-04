import ROOT

# Load the shared library with custom structs
ROOT.gSystem.Load("libstruct_write_example.so")

# Open the input ROOT file
input_file = ROOT.TFile.Open("../maker/struct_sim.root")
input_tree = input_file.Get("tree")

# Create a new ROOT file and tree
output_file = ROOT.TFile("new_struct_sim.root", "RECREATE")
output_tree = ROOT.TTree("tree", "Tree for storing events")

# Create instances for the output tree
output_event_data = ROOT.EventData()
output_hit_list = ROOT.std.vector("HitData")()
output_track_list = ROOT.std.vector("TrackData")()

# Set branches for the output tree
output_tree.Branch("evt", output_event_data)
output_tree.Branch("list_hit", output_hit_list)
output_tree.Branch("list_trk", output_track_list)

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

    # Copy event data to the output struct
    output_event_data.run_id = event_data.run_id
    output_event_data.spill_id = event_data.spill_id
    output_event_data.event_id = event_data.event_id
    output_event_data.fpga_bits = event_data.fpga_bits
    output_event_data.nim_bits = event_data.nim_bits

    # Copy hit data to the output vector
    output_hit_list.clear()  # Clear the vector before filling it
    for hit in hit_list:
        new_hit = ROOT.HitData()  # Create a new instance of HitData
        new_hit.detector_id = hit.detector_id
        new_hit.element_id = hit.element_id
        new_hit.tdc_time = hit.tdc_time
        new_hit.drift_distance = hit.drift_distance
        output_hit_list.push_back(new_hit)

    # Copy track data to the output vector
    output_track_list.clear()  # Clear the vector before filling it
    for track in track_list:
        new_track = ROOT.TrackData()  # Create a new instance of TrackData
        new_track.track_id = track.track_id
        new_track.charge = track.charge
        new_track.x_vtx = track.x_vtx
        new_track.y_vtx = track.y_vtx
        new_track.z_vtx = track.z_vtx
        new_track.px_vtx = track.px_vtx
        new_track.py_vtx = track.py_vtx
        new_track.pz_vtx = track.pz_vtx
        output_track_list.push_back(new_track)

    # Fill the output tree with the current entry
    output_tree.Fill()

# Write the output tree to the new file
output_file.Write()
output_file.Close()
input_file.Close()

print("Data successfully copied to the new ROOT file.")
