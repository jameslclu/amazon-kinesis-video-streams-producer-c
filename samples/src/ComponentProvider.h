#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_COMPONENTPROVIDER_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_RESOURCEPROVIDER_H

#include "AllInterfaces.h"

enum StreamSourceType {
    FAKE,
    ORYX
};

enum PlayerType {
    EMPTY = 0,
    AWSPRODUCKER = 1
};

class ComponentProvider {
  public:


    ComponentProvider();
    ~ComponentProvider();

    static ComponentProvider* GetInstance();

    static int SetStreamSource(StreamSourceType type, IStreamSource *pStreamSource);
    static IStreamSource* GetStreamSource(StreamSourceType type);

    static int SetKvsPlayer(PlayerType type, IKvsPlayer *pKvsPlayer);
    static IKvsPlayer* GetKvsPlayer(PlayerType type);
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_RESOURCEPROVIDER_H
