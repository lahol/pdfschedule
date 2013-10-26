#include "page-cache.h"
#include "utils.h"
#include <stdio.h>

struct PageCachePage {
    guint pagenumber;
    cairo_t *rendered;
};

guint pc_width = 1920;
guint pc_height = 1080;
gchar *doc_uri = NULL;

GList *pc_pages = NULL; /* [element-type: struct PageCachePage] */
GList *pc_current_page = NULL;

void page_cache_set_dimensions(guint width, guint height)
{
    pc_width = width;
    pc_height = height;
}

void page_cache_clear_cache(void)
{
}

void page_cache_set_file(gchar *filename)
{
    gchar *uri = util_make_uri(filename);
    fprintf(stderr, "uri: \"%s\"\n", uri);

    g_free(uri);
}

void page_cache_load_document(void)
{
}

void page_cache_add_page(guint pagenumber)
{
}

cairo_t *page_cache_render_page(guint pagenumber)
{
    return NULL;
}

cairo_t *page_cache_get_rendered(struct PageCachePage *pg)
{
    if (pg == NULL)
        return NULL;
    if (pg->rendered)
        return pg->rendered;
    
    pg->rendered = page_cache_render_page(pg->pagenumber);
    return pg->rendered;
}

cairo_t *page_cache_get_page(guint pagenumber)
{
    return NULL;
}

cairo_t *page_cache_get_next_page(void)
{
    /* next page in list */
    return page_cache_get_page(0);
}

cairo_t *page_cache_get_current_page(void)
{
    return page_cache_get_rendered(pc_current_page);
}

void page_cache_cleanup(void)
{
}
