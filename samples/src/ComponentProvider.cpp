#include "ComponentProvider.h"

static ComponentProvider* psComponentProvider = nullptr;
static IStreamSource* psFakeStreamSource = nullptr;
static IStreamSource* psOryxStreamSource = nullptr;

static IKvsPlayer* psEMPTYPlayer = nullptr;
static IKvsPlayer* psAwsProducer = nullptr;

ComponentProvider::ComponentProvider() {}

ComponentProvider::~ComponentProvider() {}

ComponentProvider* ComponentProvider::GetInstance() {
    if (psComponentProvider == nullptr) {
        psComponentProvider = new ComponentProvider();
    }
    return psComponentProvider;
}

int ComponentProvider::SetStreamSource(StreamSourceType type, IStreamSource *pStreamSource) {
    int result = 0;
    if (type == StreamSourceType::FAKE) {
        psFakeStreamSource = pStreamSource;
    } else if (type == StreamSourceType::ORYX) {
        psOryxStreamSource = pStreamSource;
    } else {
        result = 1;
    }
    return result;
}

IStreamSource* ComponentProvider::GetStreamSource(StreamSourceType type) {
    if (type == StreamSourceType::FAKE) {
        return psFakeStreamSource;
    } else if (type == StreamSourceType::ORYX) {
        return psOryxStreamSource;
    } else {
        return nullptr;
    }
}

int ComponentProvider::SetKvsPlayer(PlayerType type, IKvsPlayer *pKvsPlayer) {
    int result = 0;
    if (type == PlayerType::EMPTY) {
        psEMPTYPlayer = pKvsPlayer;
    } else if (type == PlayerType::AWSPRODUCKER) {
        psAwsProducer = pKvsPlayer;
    } else {
        result = 1;
    }
    return result;
}

IKvsPlayer* ComponentProvider::GetKvsPlayer(PlayerType type) {
    if (type == PlayerType::EMPTY) {
        return psEMPTYPlayer;
    } else if (type == PlayerType::AWSPRODUCKER) {
        return psAwsProducer;
    } else {
        return nullptr;
    }
}
