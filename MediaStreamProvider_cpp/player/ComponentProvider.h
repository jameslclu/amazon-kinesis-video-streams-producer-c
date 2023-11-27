#ifndef _COMPONENTPROVIDER_H
#define _RESOURCEPROVIDER_H

#include "AllInterfaces.h"

enum StreamSourceType {
    FAKE,
    ORYX
};

enum RenderType {
    EMPTY = 0,
    AWSPRODUCER = 1
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
