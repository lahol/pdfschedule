#ifndef __UTILS_H__
#define __UTILS_H__

#include <glib.h>

#ifdef DEBUG
#include <stdio.h>
#define DLOG(fmt, ...) fprintf(stderr, "*DEBUG* " fmt, ##__VA_ARGS__)
#else
#define DLOG(fmt, ...)
#endif

gchar *util_make_uri(const gchar *file);

gboolean util_read_uint(guint *num, const gchar *str);
gboolean util_read_boolean(gboolean *bval, const gchar *str);

#endif
