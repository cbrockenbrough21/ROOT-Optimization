#ifndef _TREE_DATA__H_
#define _TREE_DATA__H_
#include <phool/PHObject.h>
#include <TMatrixD.h>
struct EventData {
  int  run_id;
  int  spill_id;
  int  event_id;
  std::vector<int> fpga_bits; // Changed from array to vector
  std::vector<int> nim_bits;
  
  //TMatrixD *matrix; 
  EventData();
  virtual ~EventData() {
/*if (matrix) delete matrix*/;} 
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
