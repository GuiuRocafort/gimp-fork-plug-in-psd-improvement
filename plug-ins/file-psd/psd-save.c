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

#include "psd-save.h"

guint32 save_image( const char* filename,
                    gint32 image_ID,
                    GError **error )

{
  FILE* f;
  gint32 nlayers;
  gint32* layers;
  gint32 merged_layer;
  gboolean has_alpha;
  GimpPrecision precision;
  gint i;

  //OPEN FILE
  f = open_file_wb( filename, error );
  if( f == NULL ) return -1;

  precision = gimp_image_get_precision( image_ID );

  //CREATE MERGED LAYER
  has_alpha = FALSE;
  layers = gimp_image_get_layers( image_ID, &nlayers );
  for(i=0; i<nlayers; ++i)
    {
      if( gimp_drawable_has_alpha( layers[i] ) ) has_alpha = TRUE;
    }

  merged_layer  = gimp_layer_new_from_visible (image_ID, image_ID, "psd-save");

  //None of the layers have alpha, therefore the merged_image neither
  if( !has_alpha ){
    gimp_layer_flatten( merged_layer  );
  }

  //WRITE FILE HEADER
  save_psd_header( f, error, merged_layer, precision );

  //WRITE COLOR MODE
  g_debug("WRITE EMPTY COLOR MODE");
  write32bitInteger( 0, f, error );

  //WRITE FILE RESOURCES
  if( save_image_resources( f, error, image_ID ) == -1 ) return -1;

  //WRITE LAYER AND MASK INFORMATION SECTION
  g_debug("WRITE EMPTY LAYER AND MASK");
  write32bitInteger( 0, f, error );

  //WRITE IMAGE DATA
  save_imagedata(f, merged_layer, error );

  //CLOSE FILE
  fclose(f );

  return 0;
}
