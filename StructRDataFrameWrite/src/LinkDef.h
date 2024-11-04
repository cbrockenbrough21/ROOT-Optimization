#ifdef __CINT__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// Link the EventData, HitData, and TrackData structs for ROOT I/O
#pragma link C++ struct EventData+;
#pragma link C++ struct HitData+;
#pragma link C++ struct TrackData+;
#pragma link C++ class std::vector<HitData>+;
#pragma link C++ class std::vector<TrackData>+;

#endif
