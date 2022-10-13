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

    if (status == PK_STATUS_ENUM_LOADING_CACHE && type == PK_PROGRESS_TYPE_PERCENTAGE)
    {
        printf("%d\n", percent);
    }
}

void resolveReadyCb(PkClient *client, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;

    PkResults *resq = pk_client_generic_finish((PkClient *)client, res, &error);
    if (resq != NULL)
    {
        GPtrArray *array = pk_results_get_package_array(resq);
        if (array->len)
        {
            PkTask *task = pk_task_new();
            pk_package_print(g_ptr_array_index(array, 0));
            gchar **packages = g_new0(gchar *, 2);
            packages[0] = g_strdup(pk_package_get_id(g_ptr_array_index(array, 0)));
            packages[1] = NULL;
            g_print("%s %s\n", packages[0], packages[1]);
            pk_task_install_packages_async(task, packages, NULL, cb, NULL,
                                           (GAsyncReadyCallback)installReadyCb, user_data);
        }
        else
        {
            g_print("there is no package you want\n");
        }
    }

    g_main_loop_quit(user_data);
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
    PkClient *client = pk_client_new();
    gchar **packages = g_new0(gchar *, 2);
    packages[0] = g_strdup("hello");
    packages[1] = NULL;

    pk_client_set_locale(client, g_get_language_names()[0]);
    // setlocale(LC_ALL, "ko_KR.UTF-8");
    setlocale(LC_ALL, g_get_language_names()[0]);
    g_print("%s\n", g_get_language_names()[0]);
    // g_print("%s\n", pk_client_get_locale(client));

    // pk_client_set_interactive(client, TRUE);

    pk_client_resolve_async(client, PK_FILTER_ENUM_ARCH | PK_FILTER_ENUM_NOT_INSTALLED, packages, NULL,
                            cb, NULL,
                            (GAsyncReadyCallback)resolveReadyCb, loop);

    g_main_loop_run(loop);
    return 0;
}
