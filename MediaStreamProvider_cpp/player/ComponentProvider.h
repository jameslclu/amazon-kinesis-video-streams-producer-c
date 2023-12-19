#ifndef _COMPONENTPROVIDER_H
#define _COMPONENTPROVIDER_H

#include "AllInterfaces.h"

enum StreamSourceType {
    FAKE,
    ORYX,
    ORYX_WITHBUFFER
};

enum RenderType {
    EMPTY = 0,
    AWSPRODUCER_EVENTSTREAM = 1,
    AWSPRODUCER_MANUALSTERAM = 2
};

class ComponentProvider {
  public:

    ComponentProvider();
    ~ComponentProvider();

    static ComponentProvider* GetInstance();

    static int SetStreamSource(StreamSourceType type, IStreamSource *pStreamSource);
    static IStreamSource* GetStreamSource(StreamSourceType type);

    static int SetKvsRender(RenderType type, IKVSRender *pKvsPlayer);
    static IKVSRender* GetKvsRender(RenderType type);
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_RESOURCEPROVIDER_H
