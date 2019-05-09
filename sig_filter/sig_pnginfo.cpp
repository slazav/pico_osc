#include <png.h>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include "err/err.h"
#include "opt/opt.h"

/*
Read comments from "pnmc" chunk on a PNG file.
*/

using namespace std;

int
main(int argc, char *argv[]){
  try{

    if (argc!=2) throw Err() << "usage: sig_pnginfo <file>";
    const char *file = argv[1];

    FILE * infile;
    if ((infile = fopen(file, "rb")) == NULL)
      throw Err() << "can't open file:" << file;

    png_byte sign[8];
    const char sign_size = 8;
    if ((fread(sign, 1,sign_size, infile)!=sign_size)||
        (png_sig_cmp(sign, 0, sign_size)!=0)){
      throw Err() << "not a PNG file";
    }

    png_structp png_ptr = png_create_read_struct
       (PNG_LIBPNG_VER_STRING, NULL,NULL,NULL);
    if (!png_ptr)
      throw Err() << "can't make png_read_struct";

    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_infop end_info = png_create_info_struct(png_ptr);
    if ((!info_ptr) || (!end_info)) {
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
        throw Err() << "can't make png_info_struct";
    }

    if (setjmp(png_jmpbuf(png_ptr))){
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        throw Err() << "can't read PNG file";
    }

    png_set_keep_unknown_chunks(png_ptr, 2, (png_const_bytep)"pnmc", 0);

    png_init_io(png_ptr, infile);
    png_set_sig_bytes(png_ptr, sign_size);
    png_read_info(png_ptr, info_ptr);

    png_unknown_chunk *ch;
    int chn =  png_get_unknown_chunks(png_ptr, info_ptr, &ch);
    for (int i = 0; i<chn; i++){
      if (strncmp((char *)ch[i].name, "pnmc", 4)==0)
        cout.write((char *)ch[i].data, ch[i].size);
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
  }
  catch (Err e) {
    cerr << "Error: " << e.str() << "\n";
    return 1;
  }
  return 0;
}
