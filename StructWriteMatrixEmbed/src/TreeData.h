#ifndef _TREE_DATA__H_
#define _TREE_DATA__H_
#include <phool/PHObject.h>
#include <TMatrixD.h>

//Matrix constants
const int max_elements = 201;
const int max_detectors = 55;

struct EventData {
  int  run_id;
  int  spill_id;
  int  event_id;
  int fpga_bits[5];
  int  nim_bits[5];
  TMatrixD *matrix; 
  EventData();
  virtual ~EventData() {
    if (matrix) delete matrix;
  } 
  ClassDef(EventData, 1); 
};



struct HitData {
  int    detector_id;    
  int    element_id;     
  double tdc_time;       
  double drift_distance; 

  HitData();
  virtual ~HitData() {;}
  ClassDef(HitData, 1);
};

struct TrackData {
  int    track_id;
  int    charge;
  double x_vtx;
  double y_vtx;
  double z_vtx;
  double px_vtx;
  double py_vtx;
  double pz_vtx;

  TrackData();
  virtual ~TrackData() {;}

  ClassDef(TrackData, 1);
};

typedef std::vector<HitData> HitList;
typedef std::vector<TrackData > TrackList;

#endif /* _TREE_DATA__H_ */
