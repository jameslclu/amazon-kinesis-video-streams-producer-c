#ifndef KVSPROVIDER_SETTINGS_H
#define KVSPROVIDER_SETTINGS_H
#include <string>

enum Setting {
    STREAM_NAME,
    END_POINT,
    CERT_LOCATION,
    KEY_LOCATION,
    CA_LOCATION,
    ROLE_ALIAS,
    THING_NAME,
    REGION
};

class Settings {
  public:
    Settings();
    virtual ~Settings();

    int GetString(Setting id, std::string &str);
    int Init();
    int Deinit();
};

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_SETTINGS_H
