#ifndef KVSPRODUCERSERVICE_KVSSERVICESTUB_H
#define KVSPRODUCERSERVICE_KVSSERVICESTUB_H
#include <string>
#include <glib/gprintf.h>
#include <glib/gtypes.h>
#include <glib/gmain.h>
#include <gio/gio.h>
#include <gio/giotypes.h>
#include <gio/gdbusconnection.h>
#include "IKvsService.h"

class KvsServiceStub : public IKvsServiceListener{

  public:
    KvsServiceStub(IKvsService* pService);

    virtual ~KvsServiceStub();

    int Init();

    int Deinit();

    bool IsReady();
    int Interrupt(int cmd);

    int cmd1(std::string const &str, std::string &result);

    int cmd2(std::string const &str, std::string &result);

    virtual int onKvsServiceCallback(std::string const &str);
  private:
    static void on_bus_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

    static void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data);

    static void on_name_lost(GDBusConnection *connection, const gchar *name, gpointer user_data);

    static void
    handle_method_call(GDBusConnection *connection,
                       const gchar *sender,
                       const gchar *object_path,
                       const gchar *interface_name,
                       const gchar *method_name,
                       GVariant *parameters,
                       GDBusMethodInvocation *invocation,
                       gpointer user_data);

    static GVariant *
    handle_get_property(GDBusConnection *connection,
                        const gchar *sender, const gchar *object_path,
                        const gchar *interface_name, const gchar *property_name,
                        GError **error, gpointer user_data);

    static gboolean handle_set_property(GDBusConnection *connection, const gchar *sender,
                                        const gchar *object_path, const gchar *interface_name,
                                        const gchar *property_name, GVariant *value,
                                        GError **error, gpointer user_data);
};


#endif // KVSPRODUCERSERVICE_KVSSERVICESTUB_H
