#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H

#include <com/amazonaws/kinesis/video/mkvgen/Include.h>
#include <com/amazonaws/kinesis/video/common/CommonDefs.h>
#include <com/amazonaws/kinesis/video/cproducer/Include.h>
#include "am_base_include.h"
#include "am_define.h"
#include "am_export_if.h"

class IStreamSource {
  public:
    virtual int Init() = 0;
    virtual int Deinit() = 0;
    virtual int Reset() = 0;
    virtual int GetVideoFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS) = 0;
    virtual int GetAudioFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS) = 0;
    virtual int GetAVFrame(AMExportPacket* pPackage) {
        return 0;
    }
    virtual int GetAFrame(AMExportPacket* pPackage) {
        return 0;
    }

    virtual int GetVFrame(AMExportPacket* pPackage) {
        return 0;
    }
    virtual int ReleaseAVFrame(AMExportPacket* pPackage) {
        return 0;
    }
    virtual int ReleaseAFrame(AMExportPacket* pPackage) {
        return 0;
    }
    virtual int ReleaseVFrame(AMExportPacket* pPackage) {
        return 0;
    }
};

enum class KVSState {

};

class IKVSRenderCallback {

};

enum ServiceType { MANUAL, EVENT };

typedef struct {
    // Flags to control the status and operation of media streaming.
    bool initialized_flag; ///< Indicates if the system is initialized.
    bool running_flag; ///< Indicates if the media streaming is currently running.
                           ///< (orxy streaming status)
    bool recording_flag; ///< Indicates if the media streaming is currently
                           ///< recording. (aws kvs upload status)
    bool start_tag_flag; ///< Indicates if the start tag has already been sent

    // Flags to control the status and operation of the media first fram
    bool video_first_packet_flag; ///< Indicates if the video streaming is first
                                  ///< packet.
    bool audio_first_packet_flag; ///< Indicates if the audio streaming is first
                                  ///< packet.
    uint64_t last_seq_num;        ///< Last sequence number in the communication.

    // Presentation timestamps for media stream control.
    int64_t first_audio_pts;
    int64_t first_video_pts;
    int64_t end_pts;   ///< End presentation timestamp.
    int64_t start_pts; ///< Start presentation timestamp.
    int64_t start_apts; ///< Start presentation timestamp.
    int64_t start_vpts; ///< Start presentation timestamp.
    int64_t video_sent;
    // Configuration for KVS producer and frame structures for audio/video.
    //KvsProducerConfig kvsConfig; ///< Configuration for the KVS producer.
    Frame vFrame;                ///< Frame structure for video.
    Frame aFrame;                ///< Frame structure for audio.

    //KvsProducerObjects kvsObjects; ///< Handle the AWS KVS producer connection.


    enum ServiceType serviceType; ///< Type of service.
} MediaStreamConfig;

class IKVSRender {
  public:
    virtual int BaseInit() = 0;
    virtual int BaseDeinit() = 0;
    virtual STATUS PutVideoFrame(PFrame pFrame) = 0;
    virtual STATUS PutAudioFrame(PFrame pFrame) = 0;
    virtual STATUS PutAVFrame(PFrame pFrame) {
        return 0;
    }
    virtual void KvsProducerPutFrameRoutine(MediaStreamConfig *streamConfig, AMExportPacket *packet) {
        return;
    }
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLINTERFACES_H
