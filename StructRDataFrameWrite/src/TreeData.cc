#include "TreeData.h"

using namespace std;

// EventData constructor
EventData::EventData()
    : run_id(0), spill_id(0), event_id(0), fpga_bits(5, 0), nim_bits(5, 0) {
    // Vectors `fpga_bits` and `nim_bits` are initialized with 5 elements, each set to 0
}

// HitData constructor
HitData::HitData()
    : detector_id(0), element_id(0), tdc_time(0), drift_distance(0) {
    // No additional initialization needed
}

// TrackData constructor
TrackData::TrackData()
    : track_id(0), charge(0), x_vtx(0), y_vtx(0), z_vtx(0),
      px_vtx(0), py_vtx(0), pz_vtx(0) {
    // No additional initialization needed
}

// Add ClassImp macros for ROOT to generate dictionaries
ClassImp(EventData)
ClassImp(HitData)
ClassImp(TrackData)
