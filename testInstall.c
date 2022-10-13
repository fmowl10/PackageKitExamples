#include <gio/gio.h>
#include <glib.h>
#include <locale.h>
#include <packagekit-glib2/packagekit.h>
#include <stdio.h>

void installReadyCb(PkTask *task, GAsyncResult *res, gpointer user_data);

void cb(PkProgress *p, PkProgressType type, gpointer user_data)
{
    PkStatusEnum status;
    gint percent;
    gboolean cancel;
    g_object_get(
        p,
        "status", &status,
        "percentage", &percent,
        "allow-cancel", &cancel,
        NULL);

    g_print("%s ", pk_status_enum_to_string(status));

    if (status != PK_STATUS_ENUM_UNKNOWN && type == PK_PROGRESS_TYPE_PERCENTAGE)
    {
        printf("%d\n", percent);
    }
}

void installReadyCb(PkTask *task, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;
    PkResults *resq = pk_task_generic_finish((PkTask *)task, res, &error);
    g_main_loop_quit(user_data);
}
int main()
{
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    PkTask *task = pk_task_new();
    gchar **packages = g_new0(gchar *, 2);
    packages[0] = g_strdup("hello;2.10-2ubuntu4;amd64;ubuntu-jammy-main");
    packages[1] = NULL;

    setlocale(LC_ALL, "");

    pk_task_install_packages_async(task, packages, NULL,
                                   cb, NULL,
                                   (GAsyncReadyCallback)installReadyCb, loop);

    g_main_loop_run(loop);
    return 0;
}
