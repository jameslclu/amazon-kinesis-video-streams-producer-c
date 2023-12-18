#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLEPRODUCER_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLEPRODUCER_H

#include "com/amazonaws/kinesis/video/common/CommonDefs.h"
#include <com/amazonaws/kinesis/video/cproducer/Include.h>
#include "AllInterfaces.h"

class SampleProducer: public IKVSRender {
public:
    SampleProducer() {}
    virtual ~SampleProducer() {}
    int BaseInit() { return 0; }
    int BaseDeinit(){ return 0; }
    STATUS PutVideoFrame(PFrame pFrame) {
        return 0;
    }
    STATUS PutAudioFrame(PFrame pFrame) {
        return 0;
    }
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPRODUCER_H
