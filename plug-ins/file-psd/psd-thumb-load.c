
#include "psd-thumb-load.h"

guint32 load_image_thumbnail ( const gchar* filename, gint32* width, gint32* height, GError **error  )
{

  FILE* f;

  f = open_file( filename, error );
  if( f == NULL ) return -1;

  return 0;
}
