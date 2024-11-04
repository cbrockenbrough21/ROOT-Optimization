import ROOT
import sys
import time

ROOT.gSystem.Load("libstruct_write_example.so")

def time_reading(root_file):
    # Open the ROOT file
    file = ROOT.TFile.Open(root_file, "READ")
    if not file or file.IsZombie():
        print(f"Error: Could not open file {root_file}")
        return

    # Access the TTree
    tree = file.Get("tree")
    if not tree:
        print("Error: Could not find the tree in the file.")
        file.Close()
        return


    # Get the number of entries in the input tree
    n_entries = tree.GetEntries()
    print(f"Total entries in the input file: {n_entries}")

    # Start timing for reading entries
    start_time = time.time()

    # Loop over the entries in the input tree
    for i in range(n_entries):
        tree.GetEntry(i)


    # Stop timing
    end_time = time.time()
    read_time = end_time - start_time

    # Print the read time
    print(f"Read Time: {read_time:.3f} seconds")

    # Close the file
    file.Close()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 time_reading_script.py <root_file>")
        sys.exit(1)

    root_file = sys.argv[1]
    time_reading(root_file)
