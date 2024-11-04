#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Link the classes for ROOT dictionary
#pragma link C++ class EventData+;
#pragma link C++ class HitData+;
#pragma link C++ class TrackData+;
#pragma link C++ class StructWrite+;

// Link the vectors for ROOT dictionary
#pragma link C++ class std::vector<HitData>+;
#pragma link C++ class std::vector<TrackData>+;

#endif
