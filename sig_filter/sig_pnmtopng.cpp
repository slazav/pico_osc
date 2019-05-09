#include <png.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "err/err.h"
#include "opt/opt.h"

/*
A filter program for converting PNM images to PNG. Save all PNM comments
into "pnmc" chunk.
sig_filter writes some useful information where, we want to keep it.
*/

using namespace std;

int
main(){
  try{

    // read PNM header:
    string l;
    cin >> l >> ws;
    if (cin.fail() || l != "P6") throw Err() << "bad pnm file (not binary PNM)";

    // read comments!
    string comments;
    while (1) {
      char c = cin.get();
      if (cin.fail()) throw Err() << "error reading pnm file";
      if (c!='#') break;

      getline(cin, l);
      if (cin.fail()) throw Err() << "error reading pnm file";
      comments += l + "\n";
    }
    cin.unget();

    int w,h,cols;
    cin >> w >> h >> cols;
    cin.get(); // a single(!) whitespace
    if (cols!=255) throw Err() << "bad pnm file (not 256-color)";

    // Now we are ready to read data.

    // Set out output to pnm:
    png_structp png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) throw Err() << "can't write png";

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
       png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
       throw Err() << "can't write png";
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        png_destroy_write_struct(&png_ptr, &info_ptr);
        throw Err() << "can't write png";
    }

    png_init_io(png_ptr, stdout);


    // png header
    int color_type = PNG_COLOR_TYPE_RGB;

    png_set_IHDR(png_ptr, info_ptr, w, h, 8,
       PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
       PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    // comments
    {
       png_unknown_chunk ch;
       strcpy((char*)ch.name, "pnmc");
       ch.data = (png_byte*)comments.data();
       ch.size = comments.size();
       ch.location = PNG_HAVE_IHDR;
       png_set_unknown_chunks(png_ptr, info_ptr, &ch, 1);
    }

    png_write_info(png_ptr, info_ptr);

    png_bytep buf = (png_bytep)png_malloc(png_ptr, w*3);
    if (!buf) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      throw Err() << "can't write png";
    }

    for (int y=0; y<h; y++){
      cin.read((char*)buf, w*3);
      png_write_row(png_ptr, buf);
    }
    png_free(png_ptr, buf);
    png_write_end(png_ptr, info_ptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
  }
  catch (Err e) {
    cerr << "Error: " << e.str() << "\n";
    return 1;
  }
  return 0;
}
