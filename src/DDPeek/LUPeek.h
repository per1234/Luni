#ifndef LUPeek_h
#define LUPeek_h

#define SAMPLE_COUNT 16

class LUPeek: public LogicalUnitInfo {

friend class DDPeek;

public:

    LUPeek() : LogicalUnitInfo() {}
    ~LUPeek() {}

private:

    int sampleIndex[2] = {0,0};
    unsigned long samples[2][SAMPLE_COUNT + 1];
    bool isSampleBufferFull[2] = {false, false};

};

#endif
