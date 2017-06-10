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

#include "psd-save-colormode.h"

//Local functions
static void reshuffle_cmap_write (guchar *mapGimp);


guint32
save_colormode( FILE* f, gint32 image_ID, GError **error )
{
  guchar *cmap;
  guchar *cmap_modified;
  gint    i;
  gint32  nColors;

  cmap = gimp_image_get_colormap (image_ID, &nColors);

  if (nColors == 0)
    {
      g_debug("WRITE EMPTY COLOR MODE");
      write32bitInteger(0, f, error );
    }

  //Add padding to make the length 768
  else if (nColors != 256)
    {
      g_debug ("WRITTING INDEXED COLORMODE");
      write32bitInteger(768, f, error );

      cmap_modified = g_malloc (768);
      for (i = 0; i < nColors * 3; i++)
        cmap_modified[i] = cmap[i];

      for (i = nColors * 3; i < 768; i++)
        cmap_modified[i] = 0;

      reshuffle_cmap_write (cmap_modified);

      //Write colormap
      if( fwrite( cmap_modified, 768, 1, f ) < 1 )
        {
        return -1;
      }

      g_free (cmap_modified);
    }
  //The palette has 256 colors
  else
    {
      g_debug ("WRITTING INDEXED COLORMODE");
      write32bitInteger( 768, f, error );
      reshuffle_cmap_write (cmap);

      if( fwrite( cmap, 768, 1, f ) < 1 )
        {
          return -1;
        }
    }

  return 0;
}

static void
reshuffle_cmap_write (guchar *mapGimp)
{
  guchar *mapPSD;
  gint    i;

  mapPSD = g_malloc (768);

  for (i = 0; i < 256; i++)
    {
      mapPSD[i] = mapGimp[i * 3];
      mapPSD[i + 256] = mapGimp[i * 3 + 1];
      mapPSD[i + 512] = mapGimp[i * 3 + 2];
    }

  for (i = 0; i < 768; i++)
    {
      mapGimp[i] = mapPSD[i];
    }

  g_free (mapPSD);
}
