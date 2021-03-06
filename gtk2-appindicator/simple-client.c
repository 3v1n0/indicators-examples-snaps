/*
A small piece of sample code demonstrating a very simple application
with an indicator.

Copyright 2009 Canonical Ltd.

Authors:
    Ted Gould <ted@canonical.com>

This program is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License version 3, as published
by the Free Software Foundation.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranties of
MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "libappindicator/app-indicator.h"
#include "libdbusmenu-glib/server.h"
#include "libdbusmenu-glib/menuitem.h"

#define DEFAULT_ICON "tray-online"
#define DEFAULT_ACTIVE_ICON "tray-new-im"

#define LOCAL_ICON ICON_PATH G_DIR_SEPARATOR_S "simple-client-test-icon.png"
#define LOCAL_ACTIVE_ICON ICON_PATH G_DIR_SEPARATOR_S "simple-client-test-icon-active.png"


GMainLoop * mainloop = NULL;
static GtkWidget *local_icon_menu = NULL;
static GtkWidget *local_theme_menu = NULL;
static gboolean active = TRUE;
static gboolean can_haz_label = TRUE;

static void
label_toggle_cb (GtkWidget * widget, gpointer data)
{
    can_haz_label = !can_haz_label;

    if (can_haz_label) {
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "Hide label");
    } else {
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "Show label");
    }

    return;
}

static void
activate_clicked_cb (GtkWidget *widget, gpointer data)
{
    AppIndicator * ci = APP_INDICATOR(data);

    if (active) {
        app_indicator_set_status (ci, APP_INDICATOR_STATUS_ATTENTION);
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "I'm okay now");
        active = FALSE;
    } else {
        app_indicator_set_status (ci, APP_INDICATOR_STATUS_ACTIVE);
        gtk_menu_item_set_label(GTK_MENU_ITEM(widget), "Get Attention");
        active = TRUE;
    }

    return;
}

static void
reset_icons (AppIndicator * ci)
{
    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(local_theme_menu))) {
        app_indicator_set_icon_theme_path(ci, ICON_PATH);
        app_indicator_set_icon_full(ci, "custom-themed-icon", "Custom Themed Icon");
        app_indicator_set_attention_icon_full(ci, "custom-themed-icon-attention", "CUstom Themed Icon for Attention");
    } else {
        app_indicator_set_icon_full(ci, DEFAULT_ICON, "System Icon");
        app_indicator_set_attention_icon_full(ci, DEFAULT_ACTIVE_ICON, "System Active Icon");
        app_indicator_set_icon_theme_path(ci, NULL);
    }

    return;
}

static void
local_icon_toggle_cb (GtkWidget *widget, gpointer data)
{
    AppIndicator * ci = APP_INDICATOR(data);

    if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(widget))) {
        app_indicator_set_icon_full(ci, LOCAL_ICON, "Local Icon");
        app_indicator_set_attention_icon_full(ci, LOCAL_ACTIVE_ICON, "Local Attention Icon");
    } else {
        reset_icons(ci);
    }

    return;
}

static void
local_theme_toggle_cb (GtkWidget *widget, gpointer data)
{
    if (!gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(local_icon_menu))) {
        reset_icons(APP_INDICATOR(data));
    }

    return;
}

static void
item_clicked_cb (GtkWidget *widget, gpointer data)
{
  const gchar *text = (const gchar *)data;

  g_print ("%s clicked!\n", text);
}

static void
toggle_sensitivity_cb (GtkWidget *widget, gpointer data)
{
  GtkWidget *target = (GtkWidget *)data;

  gtk_widget_set_sensitive (target, !gtk_widget_is_sensitive (target));
}

static void
image_clicked_cb (GtkWidget *widget, gpointer data)
{
  gtk_image_set_from_stock (GTK_IMAGE (gtk_image_menu_item_get_image (
                            GTK_IMAGE_MENU_ITEM (widget))),
                            GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
}

static void
scroll_event_cb (AppIndicator * ci, gint delta, guint direction, gpointer data)
{
    g_print("Got scroll event! delta: %d, direction: %d\n", delta, direction);
}

static void
append_submenu (GtkWidget *item)
{
  GtkWidget *menu;
  GtkWidget *mi;
  GtkWidget *prev_mi;

  menu = gtk_menu_new ();

  mi = gtk_menu_item_new_with_label ("Sub 1");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
  g_signal_connect (mi, "activate",
                    G_CALLBACK (item_clicked_cb), "Sub 1");

  prev_mi = mi;
  mi = gtk_menu_item_new_with_label ("Sub 2");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
  g_signal_connect (mi, "activate",
                    G_CALLBACK (toggle_sensitivity_cb), prev_mi);

  mi = gtk_menu_item_new_with_label ("Sub 3");
  gtk_menu_shell_append (GTK_MENU_SHELL (menu), mi);
  g_signal_connect (mi, "activate",
                    G_CALLBACK (item_clicked_cb), "Sub 3");

  gtk_widget_show_all (menu);

  gtk_menu_item_set_submenu (GTK_MENU_ITEM (item), menu);
}

guint percentage = 0;

static gboolean
percent_change (gpointer user_data)
{
    percentage = (percentage + 1) % 100;
    if (can_haz_label) {
        gchar * percentstr = g_strdup_printf("%d%%", percentage + 1);
        app_indicator_set_label (APP_INDICATOR(user_data), percentstr, "100%");
        g_free(percentstr);
    } else {
        app_indicator_set_label (APP_INDICATOR(user_data), NULL, NULL);
    }
    return TRUE;
}

int
main (int argc, char ** argv)
{
    GtkWidget *menu = NULL;
    AppIndicator *ci = NULL;

    gtk_init (&argc, &argv);

    ci = app_indicator_new ("example-simple-client",
                            DEFAULT_ICON,
                            APP_INDICATOR_CATEGORY_APPLICATION_STATUS);

    g_assert (IS_APP_INDICATOR (ci));
    g_assert (G_IS_OBJECT (ci));

    app_indicator_set_status (ci, APP_INDICATOR_STATUS_ACTIVE);
    app_indicator_set_attention_icon_full(ci, DEFAULT_ACTIVE_ICON, "System Attention Icon");
    app_indicator_set_label (ci, "1%", "100%");
    app_indicator_set_title (ci, "Test Inidcator");

    g_signal_connect (ci, "scroll-event",
      G_CALLBACK (scroll_event_cb), NULL);

    g_timeout_add_seconds(1, percent_change, ci);

    menu = gtk_menu_new ();
    GtkWidget *item = gtk_check_menu_item_new_with_label ("1");
    g_signal_connect (item, "activate",
                      G_CALLBACK (item_clicked_cb), "1");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    gtk_widget_show (item);

    item = gtk_radio_menu_item_new_with_label (NULL, "2");
    g_signal_connect (item, "activate",
                      G_CALLBACK (item_clicked_cb), "2");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    gtk_widget_show (item);

    item = gtk_menu_item_new_with_label ("3");
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    append_submenu (item);
    gtk_widget_show (item);

    GtkWidget *toggle_item = gtk_menu_item_new_with_label ("Toggle 3");
    g_signal_connect (toggle_item, "activate",
                      G_CALLBACK (toggle_sensitivity_cb), item);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), toggle_item);
    gtk_widget_show(toggle_item);

    item = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW, NULL);
    g_signal_connect (item, "activate",
                      G_CALLBACK (image_clicked_cb), NULL);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    gtk_widget_show(item);

    item = gtk_menu_item_new_with_label ("Get Attention");
    g_signal_connect (item, "activate",
                      G_CALLBACK (activate_clicked_cb), ci);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    gtk_widget_show(item);
    app_indicator_set_secondary_activate_target(ci, item);

    item = gtk_menu_item_new_with_label ("Show label");
    label_toggle_cb(item, ci);
    g_signal_connect (item, "activate",
                      G_CALLBACK (label_toggle_cb), ci);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    gtk_widget_show(item);

    item = gtk_check_menu_item_new_with_label ("Set Icon with Full Path");
    local_icon_menu = item;
    g_signal_connect (item, "activate",
                      G_CALLBACK (local_icon_toggle_cb), ci);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    gtk_widget_show(item);

    item = gtk_check_menu_item_new_with_label ("Enable Local Theme");
    local_theme_menu = item;
    g_signal_connect (item, "activate",
                      G_CALLBACK (local_theme_toggle_cb), ci);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
    gtk_widget_show(item);

    app_indicator_set_menu (ci, GTK_MENU (menu));

    mainloop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainloop);

    return 0;
}
