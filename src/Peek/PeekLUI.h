#ifndef PeekLUI_h
#define PeekLUI_h

#define SAMPLE_COUNT 16

class PeekLUI: public LogicalUnitInfo {

friend class PeekDriver;

public:

    PeekLUI() : LogicalUnitInfo() {}
    ~PeekLUI() {}

private:

    int sampleIndex[2] = {0,0};
    unsigned long samples[2][SAMPLE_COUNT + 1];
    bool isSampleBufferFull[2] = {false, false};

};

#endif
