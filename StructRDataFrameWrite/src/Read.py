import ROOT

# Load the shared library containing the custom structs
ROOT.gSystem.Load("libstruct_rdataframe_write_example.so")

# Open the input ROOT file
input_file = ROOT.TFile("../maker/struct_rdataframe_output.root")
input_tree = input_file.Get("event_tree")

# Create a new ROOT file and tree
output_file = ROOT.TFile("../maker/new_struct_rdataframe_output.root", "RECREATE")
output_tree = ROOT.TTree("event_tree", "Event Data with Hits and Tracks")

# Create instances of the structs to hold data
event_data = ROOT.EventData()
hit_list = ROOT.std.vector("HitData")()
track_list = ROOT.std.vector("TrackData")()

# Set branches for the structs in the output tree
output_tree.Branch("event_data", event_data)
output_tree.Branch("hit_list", hit_list)
output_tree.Branch("track_list", track_list)

# Loop over the entries in the input tree and fill the output tree
for entry in input_tree:
    # Copy event data
    event_data.run_id = entry.event_data.run_id
    event_data.spill_id = entry.event_data.spill_id
    event_data.event_id = entry.event_data.event_id
    event_data.fpga_bits = entry.event_data.fpga_bits
    event_data.nim_bits = entry.event_data.nim_bits

    # Copy hit data
    hit_list.clear()
    for hit in entry.hit_list:
        hit_data = ROOT.HitData()
        hit_data.detector_id = hit.detector_id
        hit_data.element_id = hit.element_id
        hit_data.tdc_time = hit.tdc_time
        hit_data.drift_distance = hit.drift_distance
        hit_list.push_back(hit_data)

    # Copy track data
    track_list.clear()
    for track in entry.track_list:
        track_data = ROOT.TrackData()
        track_data.track_id = track.track_id
        track_data.charge = track.charge
        track_data.x_vtx = track.x_vtx
        track_data.y_vtx = track.y_vtx
        track_data.z_vtx = track.z_vtx
        track_data.px_vtx = track.px_vtx
        track_data.py_vtx = track.py_vtx
        track_data.pz_vtx = track.pz_vtx
        track_list.push_back(track_data)

    # Fill the output tree with the current entry
    output_tree.Fill()

# Write the output tree to the new file
output_tree.Write()
output_file.Close()
input_file.Close()

print("Data successfully copied to the new ROOT file.")
