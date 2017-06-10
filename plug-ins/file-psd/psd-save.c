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
  gint32 nLayers;
  gint32* layers;
  gint32 merged_layer;
  GIMPimage img;

  //OPEN FILE
  f = open_file_wb( filename, error );
  if( f == NULL ) return -1;

  layers = gimp_image_get_layers( image_ID, &nLayers );
  //There are no layers in the image. Error
  if( nLayers < 0 )
    {
      return -1;
    }

  merged_layer = layers[0];

  img.base_type = gimp_image_base_type( image_ID );
  img.precision = gimp_image_get_precision( image_ID );
  img.image_type = gimp_drawable_type( merged_layer );
  img.width = gimp_image_width( image_ID );
  img.height = gimp_image_height( image_ID );

  //WRITE FILE HEADER
  save_psd_header( f, &img, error );

  //WRITE COLOR MODE
  if( img.base_type == GIMP_INDEXED ){
    save_colormode( f, image_ID, error );
  }
  else{
    g_debug("WRITE EMPTY COLOR MODE");
    write32bitInteger( 0, f, error );
  }

  //WRITE FILE RESOURCES
  if( save_image_resources( f, error, image_ID ) == -1 ) return -1;

  //WRITE EMPTY LAYER AND MASK INFORMATION SECTION
  g_debug("WRITE EMPTY LAYER AND MASK");
  write32bitInteger( 0, f, error );

  //WRITE IMAGE DATA
  save_imagedata(f, merged_layer, error );

  //CLOSE FILE
  fclose(f );

  return 0;
}
