#include "KvsServiceStub.h"
#include <string>
#include "KvsServiceConfig.h"
static IKvsService *spIKvsService;
static GMainLoop *sp_loop;
static guint owner_id;
static GDBusConnection *sp_GDBusConnection = nullptr;
static bool s_ready = false;
static GDBusNodeInfo *introspection_data = nullptr;
//dbus-send --system --type=method_call --print-reply --dest=fxn.kvsservice /fxn/kvsservice fxn.kvsservice.cmd1 string:'{"key":"audio_speaker_volume"}'
static const gchar* introspection_xml = {
    "<node>"
    "  <interface name='fxn.kvsservice'>"
    "    <method name='cmd1'>"
    "      <arg type='s' name='value' direction='in'/>"
    "      <arg type='s' name='response' direction='out'/>"
    "    </method>"
    "    <method name='cmd2'>"
    "      <arg type='s' name='value' direction='in'/>"
    "      <arg type='s' name='response' direction='out'/>"
    "    </method>"
    "    <signal name='onSignal'>"
    "      <arg type='s' name='event'>"
    "      </arg>"
    "    </signal>"
    "    <property type='s' name='Version' access='read'/>"
    "  </interface>"
    "</node>"};

//============================================================
void
KvsServiceStub::handle_method_call(GDBusConnection *connection,
                                 const gchar *sender,
                                 const gchar *object_path,
                                 const gchar *interface_name,
                                 const gchar *method_name,
                                 GVariant *parameters,
                                 GDBusMethodInvocation *invocation,
                                 gpointer user_data) {
    (void) connection;
    (void) sender;
    (void) user_data;
    //MLogger::LOG(Level::DEBUG, "handle_method_call: sender:%s, object_path=%s, interface_name=%s, method_name=%s", sender,
                 //object_path, interface_name, method_name);
    std::string result = "invalid";
    const gchar *pdata;
    if (g_strcmp0(interface_name, INTERFACE_NAME) == 0) {
        if (g_strcmp0(method_name, "cmd1") == 0) {
            g_variant_get(parameters, "(&s)", &pdata);
            (void) spIKvsService->cmd1(pdata, result);
            //result = "ServiceA::cmd1";
        } else if (g_strcmp0(method_name, "cmd2") == 0) {
            g_variant_get(parameters, "(&s)", &pdata);
            (void) spIKvsService->cmd2(pdata, result);
            //result = "ServiceA::cmd2";
        }
    }
    gchar *response = g_strdup_printf("%s", result.c_str());
    g_dbus_method_invocation_return_value(invocation, g_variant_new("(s)", response));
    g_free(response);
}

GVariant *
KvsServiceStub::handle_get_property(GDBusConnection *connection,
                                  const gchar *sender, const gchar *object_path,
                                  const gchar *interface_name, const gchar *property_name,
                                  GError **error, gpointer user_data) {
    (void)connection;
    (void)sender;
    (void)object_path;
    (void)error;
    (void)user_data;
    GVariant *ret = nullptr;
    //MLogger::LOG(Level::DEBUG, "handle_get_property: object_path=%s, property_name=%s", interface_name, property_name);
    if (g_strcmp0(interface_name, INTERFACE_NAME) == 0) {
        if (g_strcmp0(property_name, "Version") == 0) {
            std::string str;
            (void) spIKvsService->GetAPIVersion(str);
            ret = g_variant_new_string(str.c_str());
        }
    }

    return ret;
}

gboolean KvsServiceStub::handle_set_property(GDBusConnection *connection, const gchar *sender,
                                           const gchar *object_path, const gchar *interface_name,
                                           const gchar *property_name, GVariant *value,
                                           GError **error, gpointer user_data) {
    (void)connection;
    (void)sender;
    (void)object_path;
    (void)interface_name;
    (void)property_name;
    (void)value;
    (void)error;
    (void)user_data;
    return *error == nullptr;
}

void KvsServiceStub::on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data) {
    (void) user_data;
    guint registration_id;
    //MLogger::LOG(Level::DEBUG, "on_bus_acquired: %s", name);

    if (introspection_data == nullptr || introspection_data->interfaces == nullptr) {
        //MLogger::LOG(Level::ERROR, "on_bus_acquired: introspection data error");
    } else {
        static const GDBusInterfaceVTable interface_vtable = {&handle_method_call, &handle_get_property, &handle_set_property};

        registration_id = g_dbus_connection_register_object(connection,
                                                            OBJECT_PATH,
                                                            introspection_data->interfaces[0],
                                                            &interface_vtable,
                                                            nullptr,  /* user_data */
                                                            nullptr,  /* user_data_free_func */
                                                            nullptr); /* GError** */

        if (registration_id <= 0U) {
            //MLogger::LOG(Level::ERROR, "on_bus_acquired: %d", registration_id);
            s_ready = false;
        } else {
            sp_GDBusConnection = connection;
            s_ready = true;
        }
    }
}

void KvsServiceStub::on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data) {
    (void) connection;
    (void) user_data;
    //MLogger::LOG(Level::DEBUG, "on_name_acquired: %s", name);
}

void KvsServiceStub::on_name_lost(GDBusConnection *connection, const gchar *name, gpointer user_data) {
    (void) connection;
    (void) user_data;
    //MLogger::LOG(Level::DEBUG, "on_name_lost: %s", name);
}

//==============================================
KvsServiceStub::KvsServiceStub(IKvsService *pservice) {
    if (pservice == nullptr) {
        //MLogger::LOG(Level::ERROR, "ServiceAStub: Service is null");
    } else {
        spIKvsService = pservice;
        spIKvsService->SetListener(this);
    }
}

KvsServiceStub::~KvsServiceStub() {

}

int KvsServiceStub::Init() {
    int v;
    introspection_data = g_dbus_node_info_new_for_xml(introspection_xml, nullptr);
    if (introspection_data == nullptr || introspection_data->interfaces == nullptr) {
        v = 1;
    } else {
        owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM,
                                  BUS_NAME,
                                  G_BUS_NAME_OWNER_FLAGS_REPLACE,
                                  on_bus_acquired,
                                  on_name_acquired,
                                  on_name_lost,
                                  nullptr,
                                  nullptr);

        sp_loop = g_main_loop_new(nullptr, FALSE);
        if (sp_loop != nullptr) {
            g_main_loop_run(sp_loop);
            //(void) MLogger::LOG(Level::INFO, "Init: <- g_main_loop_run");
            g_main_loop_unref(sp_loop);
            sp_loop = nullptr;
        }
        v = 0;
    }
    return v;
}
int KvsServiceStub::Interrupt(int cmd) {
    g_main_quit(sp_loop);
    return 0;
}

int KvsServiceStub::Deinit() {
    if (owner_id != 0U) {
        g_bus_unown_name(owner_id);
        owner_id = 0U;
    }

    if (sp_GDBusConnection != nullptr) {
        g_object_unref(sp_GDBusConnection);
    }

    if (nullptr != sp_loop) {
        g_main_loop_quit(sp_loop);
    }

    return 0;
}

bool KvsServiceStub::IsReady() {
    return s_ready;
}

int KvsServiceStub::onKvsServiceCallback(const std::string &str) {
    int v = 0;
    //printf("ServiceAStub::onServiceASignal: str=%s\n", str.c_str());
    GError *pError = nullptr;
    (void)g_dbus_connection_emit_signal(sp_GDBusConnection, nullptr, OBJECT_PATH, INTERFACE_NAME,
                                         "onSignal", g_variant_new("(s)", str.c_str()), &pError);
    if (pError != nullptr) {
        //MLogger::LOG(Level::ERROR, "onServiceASignal: Failed to emit the signal: Reason: %s.", pError->message);
        v = -1;
    }

    return v;
}

//#include "IServiceA.h"
