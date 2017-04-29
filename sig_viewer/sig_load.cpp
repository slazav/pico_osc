#include <tcl.h>
#include <tk.h>
#include <blt.h>

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>  // strtok
#include <cstdlib>  // atof
#include "../sig_filter/signal.h"
#include "../pico_rec/err.h"

/*
  This is a library for reading signal data into TCL BLT vectors
  Usage:
    sig_load <prefix> <file name>
      -- Load signal file into blt vectors  prefix_x, prefix_y0, prefix_y1 etc...
         Return number of channels.
*/


// register command names
Tcl_ObjCmdProc SigLoadObjCmd,
               TxtLoadObjCmd;

extern "C" {
int Sig_load_Init (Tcl_Interp *interp) {

  Tcl_CreateObjCommand(interp, "sig_load", SigLoadObjCmd,
                   (ClientData)0, (Tcl_CmdDeleteProc*)NULL);
  Tcl_CreateObjCommand(interp, "txt_load", TxtLoadObjCmd,
                   (ClientData)0, (Tcl_CmdDeleteProc*)NULL);
  return TCL_OK;
}
}

using namespace std;

/**********************************************************/
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
    Blt_Vector *vx;
    string vxn = string(pref) + "_x"; // vector name
    // we have to check separately if the vector already exists
    if (Blt_VectorExists(interp, (char *)vxn.c_str()) != TCL_OK)
      throw Err() << "vector exists: " << vxn;
    if (Blt_CreateVector(interp, (char *)vxn.c_str(), 0, &vx) != TCL_OK)
      throw Err() << "can't create vector: " << vxn;
    // vectors for data channels
    vector<Blt_Vector*> vy(num);
    for (int i=0; i<num; i++){
      ostringstream ss;
      ss << pref << "_y" << i;
      if (Blt_VectorExists(interp, (char *)ss.str().c_str()) != TCL_OK)
        throw Err() << "vector exists: " << vxn;
      if (Blt_CreateVector (interp, (char *)ss.str().c_str(), 0, &vy[i]) != TCL_OK)
        throw Err() << "can't create vector: " << ss.str();
      vy[0]->min = +HUGE_VAL;
      vy[0]->max = -HUGE_VAL;
    }

    // read data array
    int bufsize = 1<<16;
    int cnt = 0;
    int16_t buf[bufsize*num];

    while (!ff.eof()){
      ff.read((char *)buf, bufsize*num*sizeof(int16_t));
      int len = ff.gcount()/num/sizeof(int16_t);

      // resize and fill time vector:
      if (Blt_ResizeVector(vx, cnt+len) != TCL_OK)
        throw Err() << "can't fill vector: " << vxn;
      for (int i=0; i<len; i++)
        vx->valueArr[cnt+i] = sig.dt*(cnt+i) + sig.t0;
      vx->min = sig.t0;
      vx->max = vx->valueArr[cnt+len-1];

      // resize and fill data vectors:
      for (int n=0; n< num; n++){
        if (Blt_ResizeVector(vy[n], cnt+len) != TCL_OK)
          throw Err() << "can't fill vector: " << vxn;
        for (int i=0; i<len; i++){
          double v = buf[i*num+n] * sig.chan[n].sc;
          vy[n]->valueArr[cnt+i] = v;
          if (vy[n]->min > v) vy[n]->min=v;
          if (vy[n]->max < v) vy[n]->max=v;
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

/**********************************************************/

// convert a string with space separated numbers into a double vector
vector<double> str2dvec(const char *str){
  char *s, *t, *saveptr;
  const char * delim="\n\t ";
  int j;
  vector<double> ret;
  for (j = 1, s = (char*)str; ; j++, s = NULL) {
    t = strtok_r(s, delim, &saveptr);
    if (t == NULL) break;
    ret.push_back(atof(t));
  }
  return ret;
}


// txt_load code
int TxtLoadObjCmd (ClientData clientData, Tcl_Interp *interp,
                    int argc, Tcl_Obj *const argv[]) {
  try {
    // process arguments
    if (argc != 3) throw Err() << "wrong number of arguments: sig_load <pref> <file>";
    const char *pref  = Tcl_GetString(argv[1]);
    const char *fname = Tcl_GetString(argv[2]);

    // open file
    ifstream ff(fname);
    if (ff.fail()) throw Err() << "can't open file: " << fname;


    // first put data into stl vectors
    vector<vector<double> > vv;
    vector<double> min,max;

    int cnt=0; // point counter
    int cn=0;  // number columns
    while (!ff.eof()){
      // read line
      string line;
      getline(ff,line);

      // remove comments
      size_t nc = line.find('#');
      if (nc!=string::npos) line = line.substr(0,nc);

      // split into numbers
      vector<double> v = str2dvec(line.c_str());

      if (cnt==0){
        // first line: calculate number of columns, create vectors
        if (v.size() < 2) throw Err() << "at least two data columns are needed: " << fname;
        cn = v.size();
        vv.assign(cn, vector<double>());
        min.assign(cn, +HUGE_VAL);
        max.assign(cn, -HUGE_VAL);
      }
      // if some line contains smaller number of columns, fill it with NaN
      if (v.size()<cn) v.resize(cn,nan(""));
      for (int i=0; i<cn; i++){
        vv[i].push_back(v[i]);
        if (v[i]<min[i]) min[i]=v[i];
        if (v[i]>max[i]) max[i]=v[i];
      }
      cnt++;
    }

    cn--; // exclude x-column from the counter.
    // make BLT vectors and transfer data
    // vector for time
    Blt_Vector *vx;
    string vxn = string(pref) + "_x"; // vector name
    if (Blt_VectorExists(interp, (char *)vxn.c_str()) == TCL_OK)
      throw Err() << "vector exists: " << vxn;
    if (Blt_CreateVector (interp, (char *)vxn.c_str(), cnt, &vx) != TCL_OK)
      throw Err() << "can't create vector: " << vxn;
    vx->min = min[0];
    vx->max = max[0];

    // vectors for data channels
    vector<Blt_Vector*> vy(cn);
    for (int n=0; n<cn; n++){
      ostringstream ss;
      ss << pref << "_y" << n;
      if (Blt_VectorExists(interp, (char *)ss.str().c_str()) == TCL_OK)
        throw Err() << "vector exists: " << vxn;
      if (Blt_CreateVector (interp, (char *)ss.str().c_str(), cnt, &vy[n]) != TCL_OK)
        throw Err() << "can't create vector: " << ss.str();
      vy[n]->min = min[n+1];
      vy[n]->max = max[n+1];
    }

    for (int i=0; i<cnt; i++){
      vx->valueArr[i] = vv[0][i];
      for (int n=0; n<cn; n++){
        vy[n]->valueArr[i] = vv[n+1][i];
      }
    }

    Tcl_SetObjResult(interp, Tcl_NewIntObj(cn));

  }

  catch(Err e){
    // return error message as a tcl result
    Tcl_SetResult(interp, (char*)e.str().c_str(), TCL_VOLATILE);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/**********************************************************/