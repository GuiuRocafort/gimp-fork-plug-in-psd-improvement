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

#ifndef __PSD_SAVE__
#define __PSD_SAVE__

#include "common.h"
#include "save/save-utils.h"
#include "save/psd-save-header.h"
#include "save/psd-save-img-res.h"
#include "save/psd-save-layer-mask.h"
#include "save/psd-save-colormode.h"
#include "save/psd-save-imagedata.h"
#include "save/psd-save-gui.h"

guint32 save_image( const char* filename,
                    gint32 image_id ,
                    gboolean show_dialog,
                    GError **error );

#endif
