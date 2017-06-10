#include "psd-load-header.h"
#include "psd-save-header.h"
#include "common.h"

int main( int argc, char* argv[] )
{

  FILE* f = NULL;
  GIMPimage gimg;
  PSDimage pimg;
  GError* error = NULL;

  //Open file for testing
  f = g_fopen( "files/psd-header.bin", "wb" );
  if( !f ){
    g_error("Cannot create file for testing ");
    return -1;
  }

  //Set img structure to desired values
  gimg.base_type = GIMP_RGB;
  gimg.precision = GIMP_PRECISION_U8_GAMMA;
  gimg.image_type = GIMP_RGB_IMAGE;
  gimg.width = 2;
  gimg.height = 2;

  /*
  //Call psd_save_header
  if( save_psd_header( f, &gimg, &error ) == -1 )
    {
      return -1;
    }

  //Set file cursor to beginning
  fseek( f, 0, SEEK_SET );

  //Call psd_read_header
  if( read_file_header( f, &pimg, &error ) == -1 )
    {
      return -1;
      }
  */

  fclose(f);
  return 0;
}
