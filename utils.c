#include "utils.h"
#include <stdlib.h>

gchar *util_make_uri(const gchar *file)
{
    gchar *buf;
    gchar *scheme;
    gchar *cur_dir = NULL;
    gchar *uri;

    if (file == NULL)
        return NULL;

    scheme = g_uri_parse_scheme(file);
    if (scheme == NULL) {
        if (!g_path_is_absolute(file)) {
            cur_dir = g_get_current_dir();
            buf = g_build_filename(cur_dir, file, NULL);
            g_free(cur_dir);
            uri = g_filename_to_uri(buf, NULL, NULL);
            g_free(buf);
        }
        else {
            uri = g_filename_to_uri(file, NULL, NULL);
        }
        return uri;
    }
    else {
        g_free(scheme);
        return g_strdup(file);
    }
}

gboolean util_read_uint(guint *num, const gchar *str)
{
    gchar *endptr = NULL;
    if (str == NULL)
        return FALSE;

    guint n = strtoul(str, &endptr, 10);
    if (num) *num = n;

    if (str == endptr || (endptr && endptr[0] != 0))
        return FALSE;

    return TRUE;
}

gboolean util_read_boolean(gboolean *bval, const gchar *str)
{
    if (str == NULL)
        return FALSE;

    if (g_ascii_strcasecmp(str, "true") == 0 ||
            g_strcmp0(str, "1") == 0 ||
            g_ascii_strcasecmp(str, "yes") == 0 ||
            g_ascii_strcasecmp(str, "on") == 0) {
        if (bval) *bval = TRUE;
        return TRUE;
    }
    else if (g_ascii_strcasecmp(str, "false") == 0 ||
            g_strcmp0(str, "0") == 0 ||
            g_ascii_strcasecmp(str, "no") == 0 ||
            g_ascii_strcasecmp(str, "off") == 0) {
        if (bval) *bval = FALSE;
        return TRUE;
    }

    return FALSE;
}
