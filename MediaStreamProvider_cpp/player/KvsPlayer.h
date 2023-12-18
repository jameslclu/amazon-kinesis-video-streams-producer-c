#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPLAYER_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPLAYER_H
#include "AllInterfaces.h"
#include "ServiceBase.h"

enum KvsPlayerState {
   None = 0,
    Initing = 1,
    Starting = 2,
    Paus = 3,
    Stoping = 4,
    Stopped = 5,
    Deinited = 6
};

enum PLAYER_SOURCE {
    NONE = 0,
    LOCALFILE = 1,
    ORXY
};

enum PLAY_OUTPUT {
    NO = 0,
    KVS
};

enum AV_CONFIG {
    IN_A_THREAD = 0,
    IN_TWO_THREADS = 1
};

class KvsPlayer: public ServiceBase {
  public:
    KvsPlayer();
    ~KvsPlayer();
    int Init();
    int Deinit();
    // Get the data source from the ResourceProvider
    // int SetSource(IStreamSource &streamSource);
    // Get the render from the RenderPROVIDER
    // int SetRender(KvsOutput &kvsOutput);
    int Start(std::string option);
    int Pause();
    int Stop();
    int GetStatus(KvsPlayerState state);

    int HandleAsyncMethod(const MethodItem& method);
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPLAYER_H
