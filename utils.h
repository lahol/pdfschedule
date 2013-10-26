#ifndef __UTILS_H__
#define __UTILS_H__

#include <glib.h>

gchar *util_make_uri(const gchar *file);

gboolean util_read_uint(guint *num, const gchar *str);
gboolean util_read_boolean(gboolean *bval, const gchar *str);

#endif
