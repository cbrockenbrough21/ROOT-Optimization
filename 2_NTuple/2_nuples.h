#ifndef NTUPLE_WRITE_H
#define NTUPLE_WRITE_H

#include <TFile.h>
#include <TTree.h>
#include <TNtuple.h>
#include <string>

class 2_ntuple {
public:
    // Compression settings for ROOT output
    int m_basket_size = 32000;
    int m_auto_flush = 1000;
    int m_compression_level = 1;

    void OpenFile(const std::string &file_name, TFile *&m_file, TNtuple *&event_hit_ntuple, TNtuple *&event_track_ntuple);
};

#endif 