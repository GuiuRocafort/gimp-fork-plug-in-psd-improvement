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

static void colormode_changed( GtkComboBox *widget,
                               gpointer data );


Babl*
save_dialog( GIMPimage* img, PSDimage* psd )
{
  GtkWidget *dialog;
  GtkWidget *hbox;
  GtkWidget *hbox2;
  GtkWidget *table;
  GtkWidget *table2;
  GtkWidget *color_mode_label;
  GtkWidget *pixel_depth_label;
  gint active_colormode;
  PSDExportSettings settings;

  settings.img = img;

  /* Initialize main export dialog */
  dialog = gimp_export_dialog_new( _("PSD"), PLUG_IN_BINARY, SAVE_PROC );

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
  settings.color_mode = gtk_combo_box_new_text( );

  /* Populate the menu */
  gtk_combo_box_append_text( settings.color_mode, "BITMAP" );
  gtk_combo_box_append_text( settings.color_mode, "GRAYSCALE" );
  if( img->base_type != GIMP_GRAY )
    {
      gtk_combo_box_append_text( settings.color_mode, "INDEXED" );
      gtk_combo_box_append_text( settings.color_mode, "RGB" );
      gtk_combo_box_append_text( settings.color_mode, "CMYK" );
      gtk_combo_box_append_text( settings.color_mode, "LAB" );
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

  gtk_combo_box_set_active( settings.color_mode, active_colormode );
  gtk_box_pack_start( GTK_BOX(hbox), settings.color_mode, FALSE, FALSE, 0 );

  /* Pixel depth label */
  pixel_depth_label = gtk_label_new(_("Pixel depth") );
  gtk_label_set_xalign (GTK_LABEL (pixel_depth_label), 0.0);
  gtk_box_pack_start( GTK_BOX(hbox2), pixel_depth_label, FALSE, FALSE, 0 );

  /* Pixel depth Select Menu */
  settings.pixel_depth = gtk_combo_box_new_text( );
  gtk_combo_box_append_text( settings.pixel_depth, "1 bit" );
  gtk_combo_box_append_text( settings.pixel_depth, "8 bits" );
  gtk_combo_box_append_text( settings.pixel_depth, "16 bits" );
  gtk_combo_box_append_text( settings.pixel_depth, "32 bits" );
  gtk_combo_box_set_active( settings.pixel_depth, 0 );

  gtk_box_pack_start( GTK_BOX(hbox2), settings.pixel_depth, FALSE, FALSE, 0 );

  /* Change signal for the color mode select */
  g_signal_connect ( settings.color_mode, "changed",
                     G_CALLBACK (colormode_changed),
                     &settings );

  g_signal_connect (dialog, "response",
                    G_CALLBACK (export_response),
                    &settings );

  g_signal_connect (dialog, "destroy",
                    G_CALLBACK (gtk_main_quit),
                    NULL );

  /* Show the dialog */
  gtk_widget_show_all( dialog );
  gtk_main();

  return settings.format;
}

static void export_response( GtkWidget *widget,
                             gint response_id,
                             gpointer data )
{
  PSDExportSettings* s = data;
  gchar* color_mode, *pixel_depth;

  if( response_id != GTK_RESPONSE_OK )
    {
      s->format = NULL;
      gtk_widget_destroy( widget );
      return;
    }

  s->format = babl_format("R'G'B' u8");
  color_mode = gtk_combo_box_get_active_text( s->color_mode );
  pixel_depth = gtk_combo_box_get_active_text( s->pixel_depth );

  g_debug("Dialog returns color mode: %s , pixel depth: %s",
          color_mode,
          pixel_depth );

  if( strcmp("BITMAP", color_mode ) == 0 )
    {
      if( strcmp( "1 bit", pixel_depth ) == 0 )
        {
          g_debug("TODO: MISSING 1 bit BITMAP");
        }
    }

  if( strcmp("INDEXED", color_mode ) == 0 )
    {
      if( strcmp( "8 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format("Y' u8");
          g_debug("Babl format: Y' u8");
        }
    }

  if ( strcmp("GRAYSCALE", color_mode ) == 0 )
    {
      if( strcmp( "8 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "Y' u8");
          g_debug( "Babl format: Y' u8");
        }
      else if( strcmp( "16 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "Y' u16");
          g_debug( "Babl format: Y' u16");
        }
      else if( strcmp( "32 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "Y' u32");
          g_debug( "Babl format: Y' u32");
        }
    }
  else if ( strcmp( "RGB", color_mode ) == 0 )
    {
      if( strcmp( "8 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "R'G'B' u8");
          g_debug( "Babl format: R'G'B' u8");
        }
      else if( strcmp( "16 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "R'G'B' u16");
          g_debug( "Babl format: R'G'B' u16");
        }
      else if( strcmp( "32 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "R'G'B' u32");
          g_debug( "Babl format: R'G'B' u32");
        }
    }
  else if ( strcmp( "CMYK", color_mode ) == 0 )
    {
      if( strcmp( "8 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "CMYK u8");
          g_debug( "Babl format: CMYK u8");
        }
      else if( strcmp( "16 bits", pixel_depth ) == 0 )
        {
          g_debug( "TODO: MISSING Babl CMYK 16 bits");
        }
    }
  else if( strcmp( "LAB", color_mode ) == 0 )
    {
      if( strcmp( "8 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "CIE Lab u8");
          g_debug( "Babl format: CIE Lab u8");
        }
      else if( strcmp( "16 bits", pixel_depth ) == 0 )
        {
          s->format = babl_format( "CIE Lab u16");
          g_debug( "Babl format: CIE Lab u16");
        }
    }

  gtk_widget_destroy( widget );
}

static void colormode_changed( GtkComboBox *widget,
                               gpointer data )
{

}
