#include "StructWrite.h"
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TStopwatch.h>
#include <TMatrixD.h>

using namespace std;

void StructWrite::OpenFile(const std::string& file_name, TFile*& m_file, TTree*& m_tree) {
    if (m_file && m_tree) {
        std::cout << "File and tree already opened, skipping." << std::endl;
        return;
    }

    std::cout << "Attempting to open file: " << file_name << std::endl;

    m_file = new TFile(file_name.c_str(), "RECREATE");
    if (!m_file || m_file->IsZombie()) {
        std::cerr << "Error: Could not create file " << file_name << std::endl;
        exit(1);
    } else {
        std::cout << "File " << m_file->GetName() << " opened successfully." << std::endl;
    }

    TStopwatch timer;
    timer.Start();
    m_file->SetCompressionAlgorithm(ROOT::kLZMA);
    m_file->SetCompressionLevel(m_compression_level);

    m_tree = new TTree("tree", "Tree for storing events");
    m_tree->SetAutoFlush(0);

    if (!m_tree) {
        std::cerr << "Error: Could not create tree" << std::endl;
        exit(1);
    } else {
        std::cout << m_tree->GetName() << " created successfully." << std::endl;
    }

/*
    // Set up branches for event-level data
    m_tree->Branch("RunID", &RunID, "RunID/I");
    m_tree->Branch("SpillID", &SpillID, "SpillID/I");
    m_tree->Branch("EventID", &EventID, "EventID/I");
    m_tree->Branch("RFID", &RFID, "RFID/I");
    m_tree->Branch("TurnID", &TurnID, "TurnID/I");
    m_tree->Branch("Intensity", Intensity, "Intensity[33]/I");
    m_tree->Branch("trig_bits", &trig_bits, "trig_bits/I");
*/

   
    m_tree->Branch("evt", &evt);
    m_tree->Branch("list_hit", &list_hit);
    m_tree->Branch("list_trk", &list_trk);

    m_tree->SetAutoFlush(m_auto_flush);
    m_tree->SetBasketSize("*", m_basket_size);
}

// Example of filling data and writing to file
int main() {
	StructWrite writer;
	TFile* m_file = nullptr;
	TTree* m_tree = nullptr;

	writer.OpenFile("struct_sim.root", m_file, m_tree);

	TFile *file = TFile::Open("../../gen/vector_sim.root");
	TTree *tree = (TTree*)file->Get("tree");

	std::vector<int> *hit_id = nullptr;
	std::vector<int> *detector_id = nullptr;
	std::vector<int> *element_id = nullptr;
	std::vector<double> *tdc_time = nullptr;
	std::vector<double> *drift_distance = nullptr;

	std::vector<double> *track_id = nullptr;
	std::vector<double> *charge = nullptr;
	std::vector<double> *x_vtx = nullptr;
	std::vector<double> *y_vtx = nullptr;
	std::vector<double> *z_vtx = nullptr;
	std::vector<double> *px_vtx = nullptr;
	std::vector<double> *py_vtx = nullptr;
	std::vector<double> *pz_vtx = nullptr;


	int fpga_bits[5];  // assuming the arrays have 5 elements
        int nim_bits[5];
	int run_id;	
	int spill_id;
	int event_id;
	
	tree->SetBranchAddress("run_id", &run_id);
	tree->SetBranchAddress("spill_id", &spill_id);
	tree->SetBranchAddress("event_id",&event_id);
	tree->SetBranchAddress("fpga_bits", fpga_bits);
        tree->SetBranchAddress("nim_bits", nim_bits);

	tree->SetBranchAddress("hit_id", &hit_id);
	tree->SetBranchAddress("detector_id", &detector_id);
	tree->SetBranchAddress("element_id", &element_id);
	tree->SetBranchAddress("tdc_time", &tdc_time);
	tree->SetBranchAddress("drift_distance", &drift_distance);

	tree->SetBranchAddress("charge", &charge);
	tree->SetBranchAddress("track_id", &track_id);
	tree->SetBranchAddress("x_vtx", &x_vtx);
	tree->SetBranchAddress("y_vtx", &y_vtx);
	tree->SetBranchAddress("z_vtx", &z_vtx);
	tree->SetBranchAddress("px_vtx", &px_vtx);
	tree->SetBranchAddress("py_vtx", &py_vtx);
	tree->SetBranchAddress("pz_vtx", &pz_vtx);

	//TMatrixD *matrix = nullptr;
        //tree->SetBranchAddress("matrix", &matrix);

	Long64_t nentries = tree->GetEntries();

	for (Long64_t i = 0; i < nentries; i++) {

		int event= i; 
		tree->GetEntry(i);

		//writer.evt.matrix = matrix; 
		writer.evt.run_id = run_id;
		writer.evt.spill_id = spill_id;
		writer.evt.event_id = event_id;

		  for (int ii = 0; ii < 5; ii++) {
    		writer.evt.fpga_bits[ii] = fpga_bits[ii] ;
    		writer.evt.nim_bits[ii] =  nim_bits[ii];
  }



/*		for (int i = 0; i < 33; ++i) {
			writer.Intensity[i] = event + i;
		} 
*/

/*
		// Print the matrix for each event
        std::cout << "Event " << i << " matrix:" << std::endl;
        if (matrix) {
            matrix->Print();  // This will print the matrix all at once
        } else {
            std::cout << "Matrix is null!" << std::endl;
        }
*/

		writer.list_hit.clear();
		for (size_t j = 0; j < tdc_time->size(); j++) {
			//std::cout << "Event " << i << ", Hit " << j << ", TDC Time: " << tdc_time->at(j) << std::endl;
			HitData hit;
			hit.detector_id = detector_id->at(j);   
			hit.element_id = element_id->at(j);     
			hit.tdc_time = tdc_time->at(j);         
			hit.drift_distance = drift_distance->at(j); 
			writer.list_hit.push_back(hit);
		}

		//Track info
		writer.list_trk.clear();
                for (size_t j = 0; j < track_id->size(); j++) {
                        TrackData trk;
                        trk.charge = charge->at(j);   
                        trk.track_id = track_id->at(j);    
                        trk.x_vtx = x_vtx->at(j);    
                        trk.y_vtx = y_vtx->at(j);    
                        trk.z_vtx = z_vtx->at(j);    
			trk.px_vtx = px_vtx->at(j);    
                        trk.py_vtx = py_vtx->at(j);    
                        trk.pz_vtx = pz_vtx->at(j); 
                        writer.list_trk.push_back(trk);
                } 

		m_tree->Fill();
	}
	m_file->Write("", TObject::kOverwrite);
	std::cout << "Data written to the file." << std::endl;
	m_file->Close();
	delete m_file;
	return 0;
}
