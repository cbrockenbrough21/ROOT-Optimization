#ifndef STRUCT_WRITE_H
#define STRUCT_WRITE_H

#include <vector>
#include "TreeData.h"  // Include TreeData for HitData and HitList
#include <TFile.h>  // Include for TFile
#include <TTree.h>  // Include for TTree

// StructWrite class to manage event-level data
class StructWrite {
public:
    // Event-level data members
    int RunID = 0;
    int SpillID = 0;
    int RFID = 0;
    int EventID = 0;
    int TurnID = 0;
    int trig_bits = 0;
    int Intensity[33] = {0};  // Initialize array with zero values

    // Hit data (list of hits for each event)
    EventData  evt;
    TrackList list_trk;  
    HitList list_hit;  

    // Compression settings for ROOT output
    int compression_algo = 1;
    int compression_level = 1;
    int m_basket_size = 32000;
    int m_auto_flush = 1000;

    // Function to open ROOT file and create TTree
    void OpenFile(const std::string& file_name, TFile*& m_file, TTree*& m_tree);
};

#endif // STRUCT_WRITE_H

