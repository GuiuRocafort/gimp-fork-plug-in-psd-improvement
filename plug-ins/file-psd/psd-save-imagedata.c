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

 #include "psd-save-imagedata.h"


guint32 save_imagedata( FILE* f, gint32 merged_layer, GError *error )
{

  g_debug("\nMERGED IMAGE DATA ");
  g_debug("Compression : RAW" );

  //RAW
  write16bitInteger( 0, f, error );

  switch( gimp_drawable_type_with_alpha( merged_layer  ))
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
    case GIMP_GRAY_IMAGE:
    case GIMP_GRAYA_IMAGE:
      write_raw_imagedata( f, merged_layer, error );
      break;
    case GIMP_INDEXED_IMAGE:
    case GIMP_INDEXEDA_IMAGE:
      g_debug("INDEXED");
      return -1;
      break;
    default:
      return -1;
      break;
    }

  return 0;
}
