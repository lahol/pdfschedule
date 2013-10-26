#ifndef __PAGE_CACHE_H__
#define __PAGE_CACHE_H__

#include <glib.h>
#include <cairo.h>

void page_cache_set_dimensions(guint width, guint height);
void page_cache_clear_cache(void);
void page_cache_set_file(gchar *filename);
void page_cache_load_document(void);
void page_cache_add_page(guint pagenumber);
cairo_t *page_cache_get_page(guint pagenumber);
cairo_t *page_cache_get_next_page(void);
cairo_t *page_cache_get_current_page(void);

void page_cache_cleanup(void);

#endif
