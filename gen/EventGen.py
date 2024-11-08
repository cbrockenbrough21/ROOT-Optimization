import numpy as np
import ROOT

def create_hit_data(n_hits):
    hit_id = np.arange(1, n_hits + 1)
    detector_id = np.random.randint(1, 31, size=n_hits)
    element_id = np.random.randint(1, 101, size=n_hits)
    tdc_time = np.random.normal(800, 200, size=n_hits)
    drift_time = 1000 - tdc_time
    drift_distance = drift_time / 200 
    return hit_id, detector_id, element_id, tdc_time, drift_time, drift_distance

def create_track_data(n_tracks):
    track_id = np.arange(1, n_tracks + 1)
    charge = np.random.choice([-1, 1], size=n_tracks)
    x_vtx = np.random.normal(0.0, 0.1, size=n_tracks)
    y_vtx = np.random.normal(0.0, 0.1, size=n_tracks)
    z_vtx = np.random.normal(-300.0, 4.0, size=n_tracks)

    px_vtx = np.random.normal(0.0, 0.1, size=n_tracks)
    py_vtx = np.random.normal(0.0, 0.1, size=n_tracks)
    pz_vtx = np.random.normal(50.0, 10.0, size=n_tracks)

    return track_id, charge, x_vtx, y_vtx, z_vtx, px_vtx, py_vtx, pz_vtx

def create_event_data(n_events):
    run_id = np.ones(n_events, dtype=int)
    spill_id = np.full(n_events, 10, dtype=int)
    event_id = np.arange(1, n_events + 1)
    fpga_bits = np.random.choice([0, 1], size=(n_events, 5), p=[0.8, 0.2])  
    nim_bits  = np.random.choice([0, 1], size=(n_events, 5), p=[0.8, 0.2])   
    return run_id, spill_id, event_id, fpga_bits, nim_bits

def create_root_file(filename, n_events):
    # Open a ROOT file
    file = ROOT.TFile(filename, "RECREATE")
    
    # Create a TTree

    n_hits = np.random.randint(10, 100)
    max_elements = 201 
    max_detectors = 55
    tree = ROOT.TTree("tree", "Tree with event, track, and hit data")

    # Define branches for event data
    run_id = np.zeros(1, dtype=int)
    spill_id = np.zeros(1, dtype=int)
    event_id = np.zeros(1, dtype=int)
    fpga_bits = np.zeros(5, dtype=int)
    nim_bits = np.zeros(5, dtype=int)
    
    tree.Branch("run_id", run_id, "run_id/I")
    tree.Branch("spill_id", spill_id, "spill_id/I")
    tree.Branch("event_id", event_id, "event_id/I")
    tree.Branch("fpga_bits", fpga_bits, "fpga_bits[5]/I")
    tree.Branch("nim_bits", nim_bits, "nim_bits[5]/I")

    matrix = ROOT.TMatrixD(max_elements, max_detectors)
    tree.Branch("matrix", matrix)

    # Create branches for hit data
    hit_data = { 
        'hit_id': ROOT.std.vector('int')(),
        'detector_id': ROOT.std.vector('int')(),
        'element_id': ROOT.std.vector('int')(),
        'tdc_time': ROOT.std.vector('double')(),
        'drift_time': ROOT.std.vector('double')(),
        'drift_distance': ROOT.std.vector('double')()
    }   

    for key in hit_data:
        tree.Branch(key, hit_data[key])

    # Create branches for track data
    track_data = { 
        'track_id': ROOT.std.vector('int')(),
        'charge': ROOT.std.vector('int')(),
        'x_vtx': ROOT.std.vector('double')(),
        'y_vtx': ROOT.std.vector('double')(),
        'z_vtx': ROOT.std.vector('double')(),
        'px_vtx': ROOT.std.vector('double')(),
        'py_vtx': ROOT.std.vector('double')(),
        'pz_vtx': ROOT.std.vector('double')(),
    }   

    for key in track_data:
        tree.Branch(key, track_data[key])

    for i in range(n_events):
        # Fill event data
        run_id[0] = 1 
        spill_id[0] = 10
        event_id[0] = i + 1 
        fpga_bits[:] = np.random.choice([0, 1], size=5)
        nim_bits[:] = np.random.choice([0, 1], size=5)
    
        # Fill hit data
        hit_id, detector_id, element_id, tdc_time, drift_time, drift_distance = create_hit_data(n_hits)
        print("hit_id: ", hit_id)
        print("det_id: ", detector_id)

        hit_data['hit_id'].clear()
        hit_data['detector_id'].clear()
        hit_data['element_id'].clear()
        hit_data['tdc_time'].clear()
        hit_data['drift_time'].clear()
        hit_data['drift_distance'].clear()
       
        for i in range(len(hit_id)):
            hit_data['hit_id'].push_back(int(hit_id[i]))
            hit_data['detector_id'].push_back(int(detector_id[i]))
            hit_data['element_id'].push_back(int(element_id[i]))
            hit_data['tdc_time'].push_back(float(tdc_time[i]))
            hit_data['drift_time'].push_back(float(drift_time[i]))
            hit_data['drift_distance'].push_back(float(drift_distance[i]))

        matrix.Zero()  # Reset matrix to zero

        for i in range(len(hit_id)):
            ele_id = int(element_id[i])
            det_id = int(detector_id[i])
            drift_dis = float(drift_distance[i])  

            if 0 <= ele_id < max_elements and 0 <= det_id < max_detectors:
             matrix[ele_id, det_id] = drift_dis

        # Fill track data
        track_data_values = create_track_data(np.random.randint(1, 5)) 
        track_data['track_id'].clear()
        track_data['charge'].clear()
        track_data['x_vtx'].clear()
        track_data['y_vtx'].clear()
        track_data['z_vtx'].clear()
        track_data['px_vtx'].clear()
        track_data['py_vtx'].clear()
        track_data['pz_vtx'].clear()

        for i in range(len(track_data_values[0])):  # Assuming all track_data_values arrays have the same size
            track_data['track_id'].push_back(int(track_data_values[0][i]))
            track_data['charge'].push_back(int(track_data_values[1][i]))
            track_data['x_vtx'].push_back(float(track_data_values[2][i]))
            track_data['y_vtx'].push_back(float(track_data_values[3][i]))
            track_data['z_vtx'].push_back(float(track_data_values[4][i]))
            track_data['px_vtx'].push_back(float(track_data_values[5][i]))
            track_data['py_vtx'].push_back(float(track_data_values[6][i]))
            track_data['pz_vtx'].push_back(float(track_data_values[7][i]))

        tree.Fill()

    file.Write()
    file.Close()

# Create a ROOT file with n events
create_root_file("vector_sim.root", 5000)
