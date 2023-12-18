#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPRODUCER_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPRODUCER_H

#include "com/amazonaws/kinesis/video/common/CommonDefs.h"
#include <com/amazonaws/kinesis/video/cproducer/Include.h>

#include "KvsServiceConfig.h"
#include "SampleStreamSource.h"
#include "AllInterfaces.h"
#include "Settings.h"

class KvsProducer: public IKVSRender {
public:
    KvsProducer();
    virtual ~KvsProducer();
    int SetDataSource(SampleStreamSource* psource);
    int StartUpload();
    int SetHandler(STREAM_HANDLE* handler);
    int Init();
    int Init2();
    int Deinit();
    int SetStreamName(PCHAR name);
    virtual int BaseInit();
    virtual int BaseDeinit();
    virtual STATUS PutVideoFrame(PFrame pFrame);
    virtual STATUS PutAudioFrame(PFrame pFrame);
    virtual STATUS PutAVFrame(PFrame pFrame);
    virtual void KvsProducerPutFrameRoutine(MediaStreamConfig *streamConfig, AMExportPacket *packet);
  private:
    std::string mStreamName;
    std::string mIotCoreCredentialEndPoint;
    std::string mIotCoreCert;
    std::string mIotCorePrivateKey;
    std::string mCaCert;
    std::string mIotCoreRoleAlias;
    std::string mThingName;
    std::string mRegion;
    Settings mSettings;
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPRODUCER_H
