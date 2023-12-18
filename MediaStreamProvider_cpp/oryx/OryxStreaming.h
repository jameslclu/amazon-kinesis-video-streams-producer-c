#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_ORYXSTREAMING_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_ORYXSTREAMING_H
#include "AllInterfaces.h"
#include "am_base_include.h"
#include "am_define.h"
#include "am_export_if.h"

class OryxStreaming: public IStreamSource {
  public:
    int Init();
    int Deinit();
    int Reset();
    int GetVideoFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS);
    int GetAudioFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS);
    int GetAVFrame(AMExportPacket* pPackage);
    int GetAFrame(AMExportPacket* pPackage);
    int GetVFrame(AMExportPacket* pPackage);
    int ReleaseAVFrame(AMExportPacket* pPackage);
    int ReleaseAFrame(AMExportPacket* pPackage);
    int ReleaseVFrame(AMExportPacket* pPackage);
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_ORYXSTREAMING_H
