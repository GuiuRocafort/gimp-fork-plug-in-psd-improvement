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

#ifndef __PSD_H__
#define __PSD_H__

#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "psd.h"
#include "psd-load.h"
#include "psd-save.h"
#include "psd-thumb-load.h"

#include "libgimp/stdplugins-intl.h"

/* Set to FALSE to suppress pop-up warnings about lossy file conversions */
#define CONVERSION_WARNINGS             FALSE

#define LOAD_PROC                       "file-psd-load"
#define LOAD_THUMB_PROC                 "file-psd-load-thumb"
#define SAVE_PROC                       "file-psd-save"
#define PLUG_IN_BINARY                  "file-psd"
#define PLUG_IN_ROLE                    "gimp-file-psd"

/* Image color modes */
typedef enum {
  PSD_BITMAP       = 0,                 /* Bitmap image */
  PSD_GRAYSCALE    = 1,                 /* Greyscale image */
  PSD_INDEXED      = 2,                 /* Indexed image */
  PSD_RGB          = 3,                 /* RGB image */
  PSD_CMYK         = 4,                 /* CMYK */
  PSD_MULTICHANNEL = 7,                 /* Multichannel image*/
  PSD_DUOTONE      = 8,                 /* Duotone image*/
  PSD_LAB          = 9                  /* L*a*b image */
} PSDColorMode;

/* Image compression mode */
typedef enum {
  PSD_COMP_RAW     = 0,                 /* Raw data */
  PSD_COMP_RLE,                         /* RLE compressed */
  PSD_COMP_ZIP,                         /* ZIP without prediction */
  PSD_COMP_ZIP_PRED                     /* ZIP with prediction */
} PSDCompressMode;

typedef struct
{
  guint16 channels;
  guint32 height;
  guint32 width;
  guint16 depth;
  guint16 color_mode;
  GimpImageBaseType gimp_base_type;
  GimpPrecision gimp_precision;
  GimpImageType gimp_image_type;
  const Babl* pixel_format;
  gboolean has_alpha;
} PSDimage;

typedef struct
{
  GimpImageBaseType base_type;
  GimpPrecision precision;
  GimpImageType image_type;
  gint32 width, height;
} GIMPimage;

#endif /* __PSD_H__ */
