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

#ifndef __PSD_LOAD__
#define __PSD_LOAD__

#include "common.h"
#include "load/load-utils.h"
#include "load/psd-load-header.h"
#include "load/psd-load-colormode.h"
#include "load/psd-load-img-res.h"
#include "load/psd-load-layer-mask.h"
#include "load/psd-load-imagedata.h"

guint32 load_image( const char* filename, gboolean *resolution_loaded, GError **error );

#endif
