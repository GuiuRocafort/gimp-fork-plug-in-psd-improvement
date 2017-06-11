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

#include "psd-load.h"

guint32 load_image( const char* filename, gboolean *resolution_loaded, GError **error )
{
  FILE* f;
  guint32 image_ID;
  PSDimage img;

  //OPEN FILE
  f = open_file( filename, error );
  if( f == NULL ) {
    return -1;
  }

  //READ FILE HEADER
  if( read_file_header( f, &img, error ) == -1 ) return -1;

  //CREATE GIMP IMAGE
  image_ID = gimp_image_new_with_precision( img.width,
                                            img.height,
                                            img.gimp_base_type,
                                            img.gimp_precision );

  gimp_image_set_filename( image_ID, filename );
  gimp_image_undo_disable( image_ID );

  //READ COLOR MODE
  if( read_color_mode( f , &img, image_ID, error ) == -1 ) return -1;

  //READ FILE RESOURCES
  if( read_img_res( f, error ) == -1 ) return -1;

  //READ_LAYER_AND_MASK_INFORMATION_SECTION
  if( read_layer_mask_info( f, error ) == -1 ) return -1;

  //READ IMAGE DATA SECTION
  if( read_image_data ( f, image_ID, &img, error ) == -1 ) return -1;

  //CLOSE IMAGE
  gimp_image_clean_all(image_ID);
  gimp_image_undo_enable( image_ID );
  fclose( f );

  return image_ID;
}
