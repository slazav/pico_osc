#include <tcl.h>
#include <tk.h>
#include <blt.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include "../pico_filter/signal.h"
#include "../pico_rec/err.h"

/*
  This is a library for reading signal data into TCL BLT vectors
  Usage:
    sig_load <prefix> <file name>
      -- Load signal file into blt vectors  prefix_x, prefix_y0, prefix_y1 etc...
         Return number of channels.
*/


// register command names
Tcl_ObjCmdProc SigLoadObjCmd;

extern "C" {
int Sig_load_Init (Tcl_Interp *interp) {

  Tcl_CreateObjCommand(interp, "sig_load", SigLoadObjCmd,
                   (ClientData)0, (Tcl_CmdDeleteProc*)NULL);
  return TCL_OK;
}
}

using namespace std;

// sig_load code
int SigLoadObjCmd (ClientData clientData, Tcl_Interp *interp,
                    int argc, Tcl_Obj *const argv[]) {

  try {
    // process arguments
    if (argc != 3) throw Err() << "wrong number of arguments: sig_load <pref> <file>";
    const char *pref  = Tcl_GetString(argv[1]);
    const char *fname = Tcl_GetString(argv[2]);

    // open file and read header
    ifstream ff(fname);
    if (ff.fail()) throw Err() << "can't open file: " << fname;
    Signal sig = read_header(ff);

    int num = sig.chan.size();
    if (num<1) throw Err() << "no data found in file: " << fname;

    // Create BLT vectors:
    // vector for time
    Blt_Vector *vt;
    string vtn = string(pref) + "_x"; // vector name
    if (Blt_CreateVector (interp, (char *)vtn.c_str(), 0, &vt) != TCL_OK)
      throw Err() << "can't create vector: " << vtn;
    // vectors for data channels
    vector<Blt_Vector*> vv(num);
    for (int i=0; i<num; i++){
      ostringstream ss;
      ss << pref << "_y" << i;
      if (Blt_CreateVector (interp, (char *)ss.str().c_str(), 0, &vv[i]) != TCL_OK)
        throw Err() << "can't create vector: " << ss.str();
      vv[0]->min = +HUGE_VAL;
      vv[0]->max = -HUGE_VAL;
    }

    // read data array
    int bufsize = 1<<16;
    int cnt = 0;
    int16_t buf[bufsize*num];

    while (!ff.eof()){
      ff.read((char *)buf, bufsize*num*sizeof(int16_t));
      int len = ff.gcount()/num/sizeof(int16_t);

      // resize and fill time vector:
      if (Blt_ResizeVector(vt, cnt+len) != TCL_OK)
        throw Err() << "can't fill vector: " << vtn;
      for (int i=0; i<len; i++)
        vt->valueArr[cnt+i] = sig.dt*(cnt+i) + sig.t0;
      vt->min = sig.t0;
      vt->max = vt->valueArr[cnt+len-1];

      // resize and fill data vectors:
      for (int n=0; n< num; n++){
        if (Blt_ResizeVector(vv[n], cnt+len) != TCL_OK)
          throw Err() << "can't fill vector: " << vtn;
        for (int i=0; i<len; i++){
          double v = buf[i*num+n] * sig.chan[n].sc;
          vv[n]->valueArr[cnt+i] = v;
          if (vv[n]->min > v) vv[n]->min=v;
          if (vv[n]->max < v) vv[n]->max=v;
        }
      }
      cnt+=len;
    }
    Tcl_SetObjResult(interp, Tcl_NewIntObj(num));
  }

  catch(Err e){
    // return error message as a tcl result
    Tcl_SetResult(interp, (char*)e.str().c_str(), TCL_VOLATILE);
    return TCL_ERROR;
  }
  return TCL_OK;
}


// sig_fft code
int SigFFTObjCmd (ClientData clientData, Tcl_Interp *interp,
                    int argc, Tcl_Obj *const argv[]) {
  return TCL_OK;
}
