#include "Settings.h"

Settings::Settings() {

}

Settings::~Settings() {

}

int Settings::Init() {
    return 0;
}

int Settings::Deinit() {
    return 0;
}

int Settings::GetString(Setting id, std::string &str) {
    int result = 0;
    switch (id) {
        case STREAM_NAME:
            str = "SH20-eventStream-db-B813329BB08C";
            break;
        case END_POINT:
            str = "cne66nccv56pg.credentials.iot.ca-central-1.amazonaws.com";
            break;
        case CERT_LOCATION:
            str = "/home/camera/kvs/cert";
            break;
        case KEY_LOCATION:
            str = "/home/camera/kvs/privkey";
            break;
        case CA_LOCATION:
            str = "/home/camera/kvs/rootca.pem";
            break;
        case ROLE_ALIAS:
            str = "KvsCameraIoTRoleAlias";
            break;
        case THING_NAME:
            str = "db-B813329BB08C";
            break;
        case REGION:
            str = "ca-central-1";
            break;
        default:
            result = 1;
    }
    return result;
}