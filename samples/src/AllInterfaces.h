//
// Created by jh on 11/9/23.
//

#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H
#include "com/amazonaws/kinesis/video/common/CommonDefs.h"

class IStreamSource {
  public:
    virtual int GetVideoFrame(int index, PBYTE* pdata, UINT32 *psize) = 0;
    virtual int GetAudioFrame(int index, PBYTE* pdata, UINT32 *psize) = 0;
};

class IKvsPlayer {
  public:
};


#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H
