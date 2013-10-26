#ifndef __PAGE_CACHE_H__
#define __PAGE_CACHE_H__

#include <glib.h>
#include <cairo.h>

void page_cache_set_dimensions(guint width, guint height);
void page_cache_clear_cache(void);
void page_cache_set_file(gchar *filename);
void page_cache_load_document(void);
void page_cache_add_page(guint pagenumber);

void page_cache_select_page(guint pagenumber);
void page_cache_select_first_page(void);
void page_cache_select_next_page(void);

gboolean page_cache_fetch_page(cairo_surface_t **surf, guint *w, guint *h);

void page_cache_cleanup(void);

#endif
