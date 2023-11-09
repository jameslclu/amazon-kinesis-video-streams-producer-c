#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPLAYER_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPLAYER_H
#include "AllInterfaces.h"

enum KvsPlayerState {
   None = 0,
    Initing = 1,
    Starting = 2,
    Paus = 3,
    Stoping = 4,
    Stopped = 5,
    Deinited = 6
};

class KvsPlayer: IKvsPlayer {
  public:
    KvsPlayer();
    ~KvsPlayer();
    int Init();
    int Deinit();
    // Get the data source from the ResourceProvider
    // int SetSource(IStreamSource &streamSource);
    // Get the render from the RenderPROVIDER
    // int SetRender(KvsOutput &kvsOutput);
    int Start();
    int Pause();
    int Stop();
    int GetStatus(KvsPlayerState state);
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_KVSPLAYER_H
