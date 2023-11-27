#include "ComponentProvider.h"

static ComponentProvider* psComponentProvider = nullptr;
static IStreamSource* psFakeStreamSource = nullptr;
static IStreamSource* psOryxStreamSource = nullptr;

static IKVSRender* psEMPTYRender = nullptr;
static IKVSRender* psAwsProducer = nullptr;

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

int ComponentProvider::SetKvsRender(RenderType type, IKVSRender *pKvsPlayer) {
    int result = 0;
    if (type == RenderType::EMPTY) {
        psEMPTYRender = pKvsPlayer;
    } else if (type == RenderType::AWSPRODUCER) {
        psAwsProducer = pKvsPlayer;
    } else {
        result = 1;
    }
    return result;
}

IKVSRender* ComponentProvider::GetKvsRender(RenderType type) {
    if (type == RenderType::EMPTY) {
        return psEMPTYRender;
    } else if (type == RenderType::AWSPRODUCER) {
        return psAwsProducer;
    } else {
        return nullptr;
    }
}
