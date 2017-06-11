/* GIMP - The GNU Image Manipulation Program
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 *
 * GIMP PSD Plug-in
 * Copyright 2007 by John Marshall
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "psd-save-gui.h"

/* Local functions */
static void export_response( GtkWidget *widget,
                             gint response_id,
                             gpointer data );

static void colormode_changed( GtkWidget *widget,
                               gpointer data );


gboolean
save_dialog( GIMPimage* img )
{
  GtkWidget *dialog;
  GtkWidget *hbox;
  GtkWidget *hbox2;
  GtkWidget *table;
  GtkWidget *table2;
  GtkWidget *color_mode_label;
  GtkWidget *color_mode_menu;
  GtkWidget *pixel_depth_label;
  GtkWidget *pixel_depth_menu;
  gboolean response;
  gint active_colormode;

  /* Initialize main export dialog */
  dialog = gimp_export_dialog_new( _("PSD"), PLUG_IN_BINARY, SAVE_PROC );

  g_signal_connect (dialog, "response",
                    G_CALLBACK (export_response),
                    &response );

  g_signal_connect (dialog, "destroy",
                    G_CALLBACK (gtk_main_quit),
                    NULL );

  gtk_window_set_resizable( GTK_WINDOW(dialog), FALSE );

  /* Color mode Horizontal container */
  hbox = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 12 );
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 8 );

  gtk_box_pack_start( GTK_BOX( gimp_export_dialog_get_content_area( dialog) ),
                      hbox, TRUE, TRUE, 0 );

  /* Pixel Depth Horizontal container */
  hbox2 = gtk_box_new( GTK_ORIENTATION_HORIZONTAL, 12 );
  gtk_container_set_border_width (GTK_CONTAINER (hbox2), 8 );

  gtk_box_pack_start( GTK_BOX( gimp_export_dialog_get_content_area( dialog) ),
                      hbox2, TRUE, TRUE, 0 );

  /* Table for Color mode */
  table = gtk_table_new( 2, 2, FALSE );
  gtk_table_set_col_spacings (GTK_TABLE (table), 2);
  gtk_box_pack_start (GTK_BOX (hbox), table, FALSE, FALSE, 0);

  /* Table for Pixel Depth */
  table2 = gtk_table_new( 2, 2, FALSE );
  gtk_table_set_col_spacings (GTK_TABLE (table2), 2);
  gtk_box_pack_start (GTK_BOX (hbox2), table2, FALSE, FALSE, 0);

  /* Color mode label */
  color_mode_label = gtk_label_new(_("Color mode") );
  gtk_label_set_xalign (GTK_LABEL (color_mode_label), 0.0);
  gtk_box_pack_start( GTK_BOX(hbox), color_mode_label, FALSE, FALSE, 0 );

  /* Color mode Select Menu*/
  color_mode_menu = gtk_combo_box_new_text( );

  /* Populate the menu */
  gtk_combo_box_append_text( color_mode_menu, "BITMAP" );
  gtk_combo_box_append_text( color_mode_menu, "GRAYSCALE" );
  if( img->base_type != GIMP_GRAY )
    {
      gtk_combo_box_append_text( color_mode_menu, "INDEXED" );
      gtk_combo_box_append_text( color_mode_menu, "RGB" );
      gtk_combo_box_append_text( color_mode_menu, "CMYK" );
      gtk_combo_box_append_text( color_mode_menu, "LAB" );
    }

  /* Set active element the original color mode */
  active_colormode = 3;
  switch( img->base_type )
    {
    case GIMP_RGB:
      active_colormode = 3;
      break;
    case GIMP_GRAY:
      active_colormode = 1;
      break;
    case GIMP_INDEXED:
      active_colormode = 2;
      break;
    default:
      return -1;
    }

  gtk_combo_box_set_active( color_mode_menu, active_colormode );
  gtk_box_pack_start( GTK_BOX(hbox), color_mode_menu, FALSE, FALSE, 0 );

  /* Pixel depth label */
  pixel_depth_label = gtk_label_new(_("Pixel depth") );
  gtk_label_set_xalign (GTK_LABEL (pixel_depth_label), 0.0);
  gtk_box_pack_start( GTK_BOX(hbox2), pixel_depth_label, FALSE, FALSE, 0 );

  /* Pixel depth Select Menu */
  pixel_depth_menu = gtk_combo_box_new_text( );
  gtk_combo_box_append_text( pixel_depth_menu, "8 bits" );
  gtk_combo_box_append_text( pixel_depth_menu, "16 bits" );
  gtk_combo_box_append_text( pixel_depth_menu, "32 bits" );
  gtk_combo_box_set_active( pixel_depth_menu, 0 );

  gtk_box_pack_start( GTK_BOX(hbox2), pixel_depth_menu, FALSE, FALSE, 0 );

  /* Change signal for the color mode select */
  g_signal_connect ( color_mode_menu, "changed",
                     G_CALLBACK (colormode_changed),
                     pixel_depth_menu );

  /* Show the dialog */
  gtk_widget_show_all( dialog );
  gtk_main();

  return response;
}

static void export_response( GtkWidget *widget,
                             gint response_id,
                             gpointer data )
{
  gboolean* answer = data;

  switch( response_id )
    {
    case GTK_RESPONSE_OK:
      *answer = TRUE;
      gtk_widget_destroy( widget);
      break;
    default:
      *answer = FALSE;
      gtk_widget_destroy( widget);
      break;
    }
}

static void colormode_changed( GtkWidget *widget,
                               gpointer data )
{
  GtkWidget* pixel_depth_menu = data;
  gchar* new_colormode;

  new_colormode = gtk_combo_box_get_active_text( widget );

  gtk_widget_destroy( pixel_depth_menu );

  if( strcmp( new_colormode, "BITMAP" ) == 0 )
    {
      //1 bit
      gtk_combo_box_append_text( pixel_depth_menu, "1 bit" );
    }
  if( strcmp( new_colormode, "GRAYSCALE" ) == 0 )
    {
      //8, 16, 32
      gtk_combo_box_append_text( pixel_depth_menu, "8 bits" );
      gtk_combo_box_append_text( pixel_depth_menu, "16 bits" );
      gtk_combo_box_append_text( pixel_depth_menu, "32 bits" );
    }
  if( strcmp( new_colormode, "INDEXED" ) == 0 )
    {
      //8 bits
      gtk_combo_box_append_text( pixel_depth_menu, "8 bits" );
    }
  if( strcmp( new_colormode, "RGB" ) == 0 )
    {
      //8, 16, 32
      gtk_combo_box_append_text( pixel_depth_menu, "8 bits" );
      gtk_combo_box_append_text( pixel_depth_menu, "16 bits" );
      gtk_combo_box_append_text( pixel_depth_menu, "32 bits" );
    }
  if( strcmp( new_colormode, "CMYK" ) == 0 )
    {
      //8, 16
      gtk_combo_box_append_text( pixel_depth_menu, "8 bits" );
      gtk_combo_box_append_text( pixel_depth_menu, "16 bits" );
    }
  if( strcmp( new_colormode, "LAB" ) == 0 )
    {
      // 8, 16
      gtk_combo_box_append_text( pixel_depth_menu, "8 bits" );
      gtk_combo_box_append_text( pixel_depth_menu, "16 bits" );
    }

  g_debug("ColorMode changed: %s", new_colormode );
}
