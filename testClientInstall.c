#include <gio/gio.h>
#include <glib.h>
#include <locale.h>
#include <packagekit-glib2/packagekit.h>
#include <stdio.h>

void cb(PkProgress *p, PkProgressType type, gpointer user_data)
{
    PkStatusEnum status;
    gint percent;
    gboolean cancel;
    if (type == PK_PROGRESS_TYPE_PACKAGE_ID)
    {
        g_print("%s\n", pk_progress_get_package_id(p));
    }
    g_object_get(
        p,
        "status", &status,
        "percentage", &percent,
        "allow-cancel", &cancel,
        NULL);

    g_print("%s", pk_status_enum_to_string(status));
    if (status == PK_STATUS_ENUM_LOADING_CACHE && type == PK_PROGRESS_TYPE_PERCENTAGE)
    {
        printf(" %d", percent);
    }
    printf("\n");
}

void installReadyCb(PkClient *client, GAsyncResult *res, gpointer user_data)
{
    GError *error = NULL;

    PkResults *resq = pk_client_generic_finish((PkClient *)client, res, &error);
    if (resq != NULL)
    {
        GPtrArray *array = pk_results_get_package_array(resq);
        if (array->len)
        {
            for (guint i = 0; i < array->len; i++)
            {
                if (g_ptr_array_index(array, i) != NULL)
                    pk_package_print(g_ptr_array_index(array, i));
            }
        }
        else
        {
            g_print("there is no package you want\n");
        }
    }

    g_main_loop_quit(user_data);
}

int main()
{
    setlocale(LC_ALL, "");
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    PkClient *client = pk_client_new();
    gchar **packages = g_new0(gchar *, 2);
    packages[0] = g_strdup("hello;2.10-2ubuntu4;amd64;ubuntu-jammy-main");
    packages[1] = NULL;

    pk_client_set_locale(client, g_get_language_names()[0]);
    setlocale(LC_ALL, g_get_language_names()[0]);
    g_print("%s\n", g_get_language_names()[0]);

    pk_client_install_packages_async(client, TRUE, packages, NULL,
                                     cb, NULL,
                                     (GAsyncReadyCallback)installReadyCb, loop);

    g_main_loop_run(loop);
    return 0;
}
