#include <glib.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include "page-cache.h"
#include "commands.h"
#include "ipc.h"
#include "utils.h"

void main_render_window(cairo_t *cr, int width, int height);
void main_set_fullscreen(gboolean fullscreen);
void main_refresh_display(void);

void main_cleanup(void);

static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data);
static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data);
#if !GTK_CHECK_VERSION(3,0,0)
static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data);
#endif
static gboolean draw_event(GtkWidget *widget, cairo_t *cr, gpointer data);
static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);

gint command_set_handler(gint argc, gchar **argv);
gint command_pages_handler(gint argc, gchar **argv);
gint command_load_handler(gint argc, gchar **argv);
gint command_reload_handler(gint argc, gchar **argv);
gint command_quit_handler(gint argc, gchar **argv);

GdkCursor *blank_cursor = NULL;
GtkWidget *presenter_window = NULL;

gint win_width = 0;
gint win_height = 0;

guint page_interval = 10;
guint timeout_source = 0;
gboolean main_next_page(gpointer data);
void main_run_page_timer(guint interval);

void main_setup_command_handlers(void)
{
    cmd_add_command("set", command_set_handler);
    cmd_add_command("pages", command_pages_handler);
    cmd_add_command("load", command_load_handler);
    cmd_add_command("reload", command_reload_handler);
    cmd_add_command("quit", command_quit_handler);
}

gboolean main_setup_window(void)
{
    presenter_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if (presenter_window == NULL)
        return FALSE;

#if !GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(presenter_window, "expose-event", G_CALLBACK(expose_event), NULL);
#else
    g_signal_connect(presenter_window, "draw", G_CALLBACK(draw_event), NULL);
#endif
    g_signal_connect(presenter_window, "configure-event", G_CALLBACK(configure_event), NULL);
    g_signal_connect(presenter_window, "key-press-event", G_CALLBACK(key_press_event), NULL);
    g_signal_connect(presenter_window, "delete-event", G_CALLBACK(delete_event), NULL);

    gtk_widget_set_app_paintable(presenter_window, TRUE);
    gtk_window_set_default_size(GTK_WINDOW(presenter_window), 800, 600);

    gtk_widget_show_all(presenter_window);

    blank_cursor = gdk_cursor_new(GDK_BLANK_CURSOR);
    gdk_window_set_cursor(gtk_widget_get_window(presenter_window), blank_cursor);

    return TRUE;

}

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    cmd_init();
    main_setup_command_handlers();
    if (!ipc_init("/tmp/pdfscheduleipc", cmd_handle_command_line, NULL))
        goto out;

    if (!main_setup_window())
        goto out;

    gtk_main();

out:
    main_cleanup();

    return 0;
}

void main_cleanup(void)
{
    ipc_cleanup();
    cmd_cleanup();
}

gboolean main_next_page(gpointer data)
{
    page_cache_select_next_page();
    main_refresh_display();

    return TRUE;
}

void main_run_page_timer(guint interval)
{
    if (timeout_source > 0) {
        DLOG("remove source\n");
        g_source_remove(timeout_source);
        timeout_source = 0;
    }
    if (interval > 0)
        timeout_source = g_timeout_add_seconds(interval, main_next_page, NULL);
}

void main_render_window(cairo_t *cr, int width, int height)
{
    cairo_surface_t *surf = NULL;
    guint w, h;
    
    if (!page_cache_fetch_page(&surf, &w, &h) || surf == NULL) {
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_rectangle(cr, 0, 0, width, height);
        cairo_fill(cr);
    }
    else {
        double scale, tmp, ox, oy;

        scale = ((double)width)/((double)w);
        tmp = ((double)height)/((double)h);
        if (tmp < scale) scale = tmp;

        ox = (width - scale * w) * 0.5f;
        oy = (height - scale * h) * 0.5f;

        cairo_translate(cr, ox, oy);
        cairo_scale(cr, scale, scale);

        cairo_set_source_surface(cr, surf, 0.0f, 0.0f);
        cairo_rectangle(cr, 0.0f, 0.0f, w, h);
        cairo_fill(cr);
    }
}

void main_set_fullscreen(gboolean fullscreen)
{
    if (fullscreen)
        gtk_window_fullscreen(GTK_WINDOW(presenter_window));
    else
        gtk_window_unfullscreen(GTK_WINDOW(presenter_window));
}

gint command_set_handler(gint argc, gchar **argv)
{
    if (argc < 2)
        return 1;
    if (g_strcmp0(argv[1], "size") == 0) {
        if (argc < 4)
            return 1;
        guint w = 0, h = 0;
        if (util_read_uint(&w, argv[2]) &&
                util_read_uint(&h, argv[3])) {
            page_cache_set_dimensions(w, h);
            return 0;
        }
    }
    else if (g_strcmp0(argv[1], "fullscreen") == 0) {
        if (argc < 3)
            return 1;
        gboolean fs = FALSE;
        if (util_read_boolean(&fs, argv[2])) {
            main_set_fullscreen(fs);
            return 0;
        }
    }
    else if (g_strcmp0(argv[1], "page-interval") == 0) {
        if (argc < 3)
            return 1;
        guint interval;
        if (util_read_uint(&interval, argv[2])) {
            page_interval = interval;
            main_run_page_timer(page_interval);
            return 0;
        }
    }

    return 1;
}

gint command_pages_handler(gint argc, gchar **argv)
{
    page_cache_clear_cache();

    guint i;
    guint pg;
    for (i = 1; i < argc; ++i) {
        if (util_read_uint(&pg, argv[i])) {
            page_cache_add_page(pg > 0 ? pg - 1 : 0);
        }
    }

    page_cache_select_first_page();
    main_run_page_timer(page_interval);

    main_refresh_display();

    return 0;
}

gint command_load_handler(gint argc, gchar **argv)
{
    fprintf(stderr, "load handler\n");
    if (argc != 2)
        return 1;
    page_cache_set_file(argv[1]);
    page_cache_load_document();
    return 0;
}

gint command_reload_handler(gint argc, gchar **argv)
{
    fprintf(stderr, "reload handler\n");
    page_cache_load_document();
    return 0;
}

gint command_quit_handler(gint argc, gchar **argv)
{
    fprintf(stderr, "quit handler\n");
    gtk_main_quit();
    return 0;
}

void main_refresh_display(void)
{
    if (presenter_window != NULL)
        gtk_widget_queue_draw(presenter_window);
}

static gboolean key_press_event(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
    return FALSE;
}

static gboolean configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    win_width = event->width;
    win_height = event->height;

    main_refresh_display();

    return FALSE;
}

static gboolean draw_event(GtkWidget *widget, cairo_t *cr, gpointer data)
{
    if (cr == NULL)
        return FALSE;

    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, win_width, win_height);
    cairo_fill(cr);

    main_render_window(cr, win_width, win_height);

    return FALSE;
}

#if !GTK_CHECK_VERSION(3,0,0)
static gboolean expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    cairo_t *cr = gdk_cairo_create(gtk_widget_get_window(widget));
    draw_event(widget, cr, data);
    cairo_destroy(cr);

    return FALSE;
}
#endif

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    gtk_main_quit();
    return FALSE;
}
