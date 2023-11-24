#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPRODUCER_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPRODUCER_H

#include "com/amazonaws/kinesis/video/common/CommonDefs.h"
#include <com/amazonaws/kinesis/video/cproducer/Include.h>

#include "KvsServiceConfig.h"
#include "StreamSource.h"
#include "AllInterfaces.h"
class KvsProducer: public IKvsRender {
  public:
    KvsProducer();
    virtual ~KvsProducer();
    int SetDataSource(SampleStreamSource* psource);
    int StartUpload();
    int SetHandler(STREAM_HANDLE* handler);
    int Init();
    int Deinit();
    int SetStreamName(PCHAR name);
    virtual int BaseInit();
    virtual int BaseDeinit();
    virtual STATUS PutVideoFrame(STREAM_HANDLE streamHandle, PFrame pFrame);
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPRODUCER_H
