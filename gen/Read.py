import ROOT

# Try opening the file
input_file = ROOT.TFile.Open("vector_sim.root")
if not input_file or input_file.IsZombie():
    print("Error: Could not open file vector_sim.root")
else:
    tree = input_file.Get("tree")
    if not tree:
        print("Error: Could not access the tree")
    else:
        # Print the structure of the tree (shows branches and data types)
        tree.Print()

        # Loop over the entries and read data
        nentries = tree.GetEntries()
        for i in range(nentries):
            tree.GetEntry(i)
            
            # Accessing event-level data
            print(f"Event ID: {tree.event_id}, Run ID: {tree.run_id}")
            
            # Accessing hit data
            for j in range(len(tree.hit_id)):
                print(f"Hit {j}: Detector ID = {tree.detector_id[j]}, TDC Time = {tree.tdc_time[j]}, Drift Distance = {tree.drift_distance[j]}")
            
            # Accessing track data
            for j in range(len(tree.track_id)):
                print(f"Track {j}: X VTX = {tree.x_vtx[j]}, Y VTX = {tree.y_vtx[j]}")

        # Close the ROOT file
        input_file.Close()
