#include <gio/gio.h>
#include <glib.h>
#include <locale.h>
#include <packagekit-glib2/packagekit.h>
#include <stdio.h>
int main()
{
    GError *error = NULL;
    PkError *error_code = NULL;
    PkResults *results = NULL;
    GPtrArray *array = NULL;
    PkPackage *item;
    gchar **values = NULL;
    gchar **package_ids = NULL;
    uint i;
    PkTask *task;

    setlocale(LC_ALL, "");

    task = pk_task_new();

    /* resolve the package name */
    values = g_new0(gchar *, 1 + 1);
    values[0] = g_strdup("hello");
    values[1] = NULL;
    results = pk_task_resolve_sync(task, PK_FILTER_ENUM_NOT_INSTALLED, values, NULL, NULL, NULL, &error);

    /* check error code */
    error_code = pk_results_get_error_code(results);
    if (error_code != NULL)
    {
        g_printerr("%s: %s, %s\n", "Resolving of packages failed",
                   pk_error_enum_to_string(pk_error_get_code(error_code)),
                   pk_error_get_details(error_code));
        goto out;
    }

    /* get the packages returned */
    array = pk_results_get_package_array(results);
    package_ids = g_new0(gchar *, array->len + 1);
    for (i = 0; i < array->len; i++)
    {
        item = g_ptr_array_index(array, i);
        package_ids[i] = g_strdup(pk_package_get_id(item));
        printf("%s\n", package_ids[i]);
        pk_package_print(item);
    }

    /* install the packages */
    results = pk_task_install_packages_sync(task, package_ids, NULL, NULL, NULL, &error);

    /* check error code */
    /*error_code = pk_results_get_error_code(results);
    if (error_code != NULL)
    {
        g_printerr("%s: %s, %s\n", "Error installing package(s)!",
                   pk_error_enum_to_string(pk_error_get_code(error_code)),
                   pk_error_get_details(error_code));
        goto out;
    }*/

out:
    g_strfreev(values);
    g_object_unref(task);
    if (error_code != NULL)
        g_object_unref(error_code);
    if (array != NULL)
        g_ptr_array_unref(array);
    if (package_ids != NULL)
        g_strfreev(package_ids);
    if (results != NULL)
        g_object_unref(results);
}
