#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <glib.h>

typedef gint (*CmdHandler)(gint, gchar **);

void cmd_init(void);
void cmd_cleanup(void);

void cmd_handle_command_line(gchar *line, gpointer userdata);
gint cmd_run_command(gint argc, gchar **argv);

void cmd_add_command(gchar *command, CmdHandler handler);

#endif
