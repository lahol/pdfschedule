#include "page-cache.h"
#include "utils.h"
#include <cairo.h>
#include <poppler.h>
#include <stdio.h>

struct PageCachePage {
    guint pagenumber;
    cairo_surface_t *surface;
};

guint pc_width = 1920;
guint pc_height = 1080;
gchar *pc_doc_uri = NULL;

GList *pc_pages = NULL; /* [element-type: struct PageCachePage] */
GList *pc_current_page = NULL;

PopplerDocument *pc_doc = NULL;

void page_cache_render_page(struct PageCachePage *pg);
void page_cache_render_all(void);

void page_cache_set_dimensions(guint width, guint height)
{
    pc_width = width;
    pc_height = height;
}

void page_cache_free_page(struct PageCachePage *pg)
{
    if (pg) {
        DLOG("unref: page_cache_free_page\n");
        if (pg->surface != NULL)
            cairo_surface_destroy(pg->surface);
        g_free(pg);
    }
}

void page_cache_clear_cache(void)
{
    g_list_free_full(pc_pages, (GFreeFunc)page_cache_free_page);
    pc_pages = NULL;
    pc_current_page = NULL;
}

void page_cache_set_file(gchar *filename)
{
    pc_doc_uri = util_make_uri(filename);
}

void page_cache_load_document(void)
{
    if (pc_doc != NULL) {
        DLOG("unref: page_cache_load_document, doc\n");
        g_object_unref(G_OBJECT(pc_doc));
        pc_doc = NULL;
    }
    if (pc_doc_uri != NULL) {
        pc_doc = poppler_document_new_from_file(pc_doc_uri, NULL, NULL);
    }

    if (pc_doc == NULL)
        return;

    page_cache_render_all();
}

void page_cache_add_page(guint pagenumber)
{
    struct PageCachePage *pg = g_malloc0(sizeof(struct PageCachePage));
    pg->pagenumber = pagenumber;
    pc_pages = g_list_append(pc_pages, pg);
}

void page_cache_select_page(guint pagenumber)
{
    pc_current_page = pc_pages;
    while (pc_current_page) {
        if (pc_current_page->data &&
                ((struct PageCachePage *)pc_current_page->data)->pagenumber == pagenumber)
            break;
    }
}

void page_cache_select_first_page(void)
{
    pc_current_page = pc_pages;
}

void page_cache_select_next_page(void)
{
    if (pc_current_page != NULL && pc_current_page->next != NULL)
        pc_current_page = pc_current_page->next;
    else
        pc_current_page = pc_pages;
}

gboolean page_cache_fetch_page(cairo_surface_t **surf, guint *w, guint *h)
{
    if (pc_current_page == NULL)
        return FALSE;

    if (((struct PageCachePage *)pc_current_page->data)->surface == NULL)
        page_cache_render_page((struct PageCachePage *)pc_current_page->data);
    
    if (surf) *surf = ((struct PageCachePage *)pc_current_page->data)->surface;
    if (w) *w = pc_width;
    if (h) *h = pc_height;

    return TRUE;
}

void page_cache_render_page(struct PageCachePage *pg)
{
    if (pg == NULL || pc_doc == NULL)
        return;
    if (pg->surface != NULL) {
        cairo_surface_destroy(pg->surface);
        pg->surface = NULL;
    }

    PopplerPage *page = poppler_document_get_page(pc_doc, pg->pagenumber);
    if (page == NULL)
        return;

    double ph, pw, scale, tmp;
    poppler_page_get_size(page, &pw, &ph);

    pg->surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, (int)pc_width, (int)pc_height);
    if (pg->surface == NULL)
        goto out;

    cairo_t *cr = cairo_create(pg->surface);
    if (cr == NULL) {
        DLOG("unref: page_cache_render_page, cr\n");
        g_object_unref(pg->surface);
        goto out;
    }

    scale = ((double)pc_width)/((double)pw);
    tmp = ((double)pc_height)/((double)ph);
    if (tmp < scale) scale = tmp;

    double ox = (pc_width - scale * pw) * 0.5f;
    double oy = (pc_height - scale * ph) * 0.5f;

    cairo_translate(cr, ox, oy);
    cairo_scale(cr, scale, scale);

    poppler_page_render(page, cr);
    cairo_destroy(cr);

out:
    DLOG("unref: page_cache_render_page, page\n");
    g_object_unref(page);
}

void page_cache_render_all(void)
{
    GList *tmp;
    for (tmp = pc_pages; tmp != NULL; tmp = g_list_next(tmp)) {
        page_cache_render_page((struct PageCachePage *)tmp->data);
    }
}
/*cairo_t *page_cache_render_page(guint pagenumber)
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
{*/
    /* next page in list */
   /* return page_cache_get_page(0);
}

cairo_t *page_cache_get_current_page(void)
{
    return page_cache_get_rendered(pc_current_page);
}
*/
void page_cache_cleanup(void)
{
    page_cache_clear_cache();
    DLOG("unref: page_cache_cleanup\n");
    if (pc_doc)
        g_object_unref(pc_doc);
    g_free(pc_doc_uri);
}
