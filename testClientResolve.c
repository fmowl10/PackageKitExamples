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
    if (type == PK_PROGRESS_TYPE_PACKAGE_ID)
        g_print("dddl\n");
    g_object_get(
        p,
        "status", &status,
        "percentage", &percent,
        "allow-cancel", &cancel,
        NULL);

    g_print("%s ", pk_progress_get_package_id(p));

    g_print("%s", pk_status_enum_to_string(status));

    if (status == PK_STATUS_ENUM_LOADING_CACHE && type == PK_PROGRESS_TYPE_PERCENTAGE)
    {
        printf(" %d", percent);
    }
    printf("\n");
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
            for (int i = 0; i < array->len; i++)
            {
                if (g_ptr_array_index(array, i) != NULL)
                {
                    g_print("%s\n", pk_package_get_summary(g_ptr_array_index(array, i)));
                    pk_package_print(g_ptr_array_index(array, i));
                    g_print("%s\n", pk_package_get_id(g_ptr_array_index(array, i)));
                }
            }
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
    setlocale(LC_ALL, "");
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    PkTask *task = pk_task_new();
    PkClient *client = pk_client_new();
    gchar **packages = g_new0(gchar *, 3);
    packages[0] = g_strdup("hello");
    packages[1] = g_strdup("vim");
    packages[2] = NULL;

    g_print("%s\n", pk_client_get_locale(client));

    // pk_client_set_locale(client, g_get_language_names()[0]);
    g_print("%s\n", g_get_language_names()[0]);

    pk_client_resolve_async(client, pk_filter_bitfield_from_string("arch;newest"), packages, NULL,
                            cb, NULL,
                            (GAsyncReadyCallback)resolveReadyCb, loop);

    g_main_loop_run(loop);
    return 0;
}
