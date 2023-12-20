#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H

#include <com/amazonaws/kinesis/video/mkvgen/Include.h>
#include <com/amazonaws/kinesis/video/common/CommonDefs.h>
#include <com/amazonaws/kinesis/video/cproducer/Include.h>

class IStreamSource {
  public:
    virtual int GetVideoFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS) = 0;
    virtual int GetAudioFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS) = 0;
};

class IKvsRender {
  public:
    virtual int BaseInit() = 0;
    virtual int BaseDeinit() = 0;
    virtual STATUS PutVideoFrame(STREAM_HANDLE streamHandle, PFrame pFrame) = 0;
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H
