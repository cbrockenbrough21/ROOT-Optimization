#include "TreeData.h"
using namespace std;


ClassImp(EventData)
ClassImp(HitData)
ClassImp(TrackData)

EventData::EventData()
  : run_id(0)
  , spill_id(0)
  , event_id(0)
{
 for (int ii = 0; ii < 5; ii++) {
    fpga_bits [ii] = 0;
    nim_bits [ii] = 0;
  }
}

 
HitData::HitData()
  : detector_id(0)
  , element_id(0)
  , tdc_time(0)
  , drift_distance(0)
{
  ;
}

TrackData::TrackData() 
  :charge(0) 
  , track_id(0)
  , x_vtx(0)
  , y_vtx(0)
  , z_vtx(0)
  , px_vtx(0)
  , py_vtx(0)
  , pz_vtx(0)
{
  ;
}
