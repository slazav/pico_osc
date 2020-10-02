#ifndef ADC24_CONST_H
#define ADC24_CONST_H

#include <pico/HRDL.h>

// functions for converting ADC24 constants

// convert ms to conversion time
HRDL_CONVERSION_TIME tconvi2convtime(int16_t t){
  switch (t){
    case  60: return HRDL_60MS;
    case 100: return HRDL_100MS;
    case 180: return HRDL_180MS;
    case 340: return HRDL_340MS;
    case 660: return HRDL_660MS;
  }
  throw Err() << "error: unknown conversion time: " << t;
}

const char * tconv2str(HRDL_CONVERSION_TIME tconv){
  switch (tconv){
    case  HRDL_60MS: return "60";
    case HRDL_100MS: return "100";
    case HRDL_180MS: return "180";
    case HRDL_340MS: return "340";
    case HRDL_660MS: return "660";
  }
  throw Err() << "error: unknown conversion time: " << tconv;
}

HRDL_DIGITAL_IO_CHANNEL int2dch(int ch){
  switch (ch){
    case 1: return HRDL_DIGITAL_IO_CHANNEL_1;
    case 2: return HRDL_DIGITAL_IO_CHANNEL_2;
    case 3: return HRDL_DIGITAL_IO_CHANNEL_3;
    case 4: return HRDL_DIGITAL_IO_CHANNEL_4;
  }
  throw Err() << "error: unknown digital channel: " << ch;
}

int dch2int(HRDL_DIGITAL_IO_CHANNEL ch){
  switch (ch){
    case HRDL_DIGITAL_IO_CHANNEL_1: return 1;
    case HRDL_DIGITAL_IO_CHANNEL_2: return 2;
    case HRDL_DIGITAL_IO_CHANNEL_3: return 3;
    case HRDL_DIGITAL_IO_CHANNEL_4: return 4;
  }
  throw Err() << "error: unknown digital channel: " << ch;
}

HRDL_INPUTS int2ach(int ch){
  switch (ch){
    case  1: return HRDL_ANALOG_IN_CHANNEL_1;
    case  2: return HRDL_ANALOG_IN_CHANNEL_2;
    case  3: return HRDL_ANALOG_IN_CHANNEL_3;
    case  4: return HRDL_ANALOG_IN_CHANNEL_4;
    case  5: return HRDL_ANALOG_IN_CHANNEL_5;
    case  6: return HRDL_ANALOG_IN_CHANNEL_6;
    case  7: return HRDL_ANALOG_IN_CHANNEL_7;
    case  8: return HRDL_ANALOG_IN_CHANNEL_8;
    case  9: return HRDL_ANALOG_IN_CHANNEL_9;
    case 10: return HRDL_ANALOG_IN_CHANNEL_10;
    case 11: return HRDL_ANALOG_IN_CHANNEL_11;
    case 12: return HRDL_ANALOG_IN_CHANNEL_12;
    case 13: return HRDL_ANALOG_IN_CHANNEL_13;
    case 14: return HRDL_ANALOG_IN_CHANNEL_14;
    case 15: return HRDL_ANALOG_IN_CHANNEL_15;
    case 16: return HRDL_ANALOG_IN_CHANNEL_16;
  }
  throw Err() << "error: unknown analog channel: " << ch;
}

int ach2int(HRDL_INPUTS ch){
  switch (ch){
    case  HRDL_ANALOG_IN_CHANNEL_1: return 1;
    case  HRDL_ANALOG_IN_CHANNEL_2: return 2;
    case  HRDL_ANALOG_IN_CHANNEL_3: return 3;
    case  HRDL_ANALOG_IN_CHANNEL_4: return 4;
    case  HRDL_ANALOG_IN_CHANNEL_5: return 5;
    case  HRDL_ANALOG_IN_CHANNEL_6: return 6;
    case  HRDL_ANALOG_IN_CHANNEL_7: return 7;
    case  HRDL_ANALOG_IN_CHANNEL_8: return 8;
    case  HRDL_ANALOG_IN_CHANNEL_9: return 9;
    case HRDL_ANALOG_IN_CHANNEL_10: return 10;
    case HRDL_ANALOG_IN_CHANNEL_11: return 11;
    case HRDL_ANALOG_IN_CHANNEL_12: return 12;
    case HRDL_ANALOG_IN_CHANNEL_13: return 13;
    case HRDL_ANALOG_IN_CHANNEL_14: return 14;
    case HRDL_ANALOG_IN_CHANNEL_15: return 15;
    case HRDL_ANALOG_IN_CHANNEL_16: return 16;
  }
  throw Err() << "error: unknown analog channel: " << ch;
}

HRDL_INPUTS str2ach(const char *str){
  if (strcasecmp(str,"01")==0) return HRDL_ANALOG_IN_CHANNEL_1;
  if (strcasecmp(str,"02")==0) return HRDL_ANALOG_IN_CHANNEL_2;
  if (strcasecmp(str,"03")==0) return HRDL_ANALOG_IN_CHANNEL_3;
  if (strcasecmp(str,"04")==0) return HRDL_ANALOG_IN_CHANNEL_4;
  if (strcasecmp(str,"05")==0) return HRDL_ANALOG_IN_CHANNEL_5;
  if (strcasecmp(str,"06")==0) return HRDL_ANALOG_IN_CHANNEL_6;
  if (strcasecmp(str,"07")==0) return HRDL_ANALOG_IN_CHANNEL_7;
  if (strcasecmp(str,"08")==0) return HRDL_ANALOG_IN_CHANNEL_8;
  if (strcasecmp(str,"09")==0) return HRDL_ANALOG_IN_CHANNEL_9;
  if (strcasecmp(str,"10")==0) return HRDL_ANALOG_IN_CHANNEL_10;
  if (strcasecmp(str,"11")==0) return HRDL_ANALOG_IN_CHANNEL_11;
  if (strcasecmp(str,"12")==0) return HRDL_ANALOG_IN_CHANNEL_12;
  if (strcasecmp(str,"13")==0) return HRDL_ANALOG_IN_CHANNEL_13;
  if (strcasecmp(str,"14")==0) return HRDL_ANALOG_IN_CHANNEL_14;
  if (strcasecmp(str,"15")==0) return HRDL_ANALOG_IN_CHANNEL_15;
  if (strcasecmp(str,"16")==0) return HRDL_ANALOG_IN_CHANNEL_16;
  throw Err() << "error: unknown analog channel: " << str;
}

const char * ach2str(HRDL_INPUTS ch){
  switch (ch){
    case  HRDL_ANALOG_IN_CHANNEL_1: return "01";
    case  HRDL_ANALOG_IN_CHANNEL_2: return "02";
    case  HRDL_ANALOG_IN_CHANNEL_3: return "03";
    case  HRDL_ANALOG_IN_CHANNEL_4: return "04";
    case  HRDL_ANALOG_IN_CHANNEL_5: return "05";
    case  HRDL_ANALOG_IN_CHANNEL_6: return "06";
    case  HRDL_ANALOG_IN_CHANNEL_7: return "07";
    case  HRDL_ANALOG_IN_CHANNEL_8: return "08";
    case  HRDL_ANALOG_IN_CHANNEL_9: return "09";
    case HRDL_ANALOG_IN_CHANNEL_10: return "10";
    case HRDL_ANALOG_IN_CHANNEL_11: return "11";
    case HRDL_ANALOG_IN_CHANNEL_12: return "12";
    case HRDL_ANALOG_IN_CHANNEL_13: return "13";
    case HRDL_ANALOG_IN_CHANNEL_14: return "14";
    case HRDL_ANALOG_IN_CHANNEL_15: return "15";
    case HRDL_ANALOG_IN_CHANNEL_16: return "16";
  }
  throw Err() << "error: unknown analog channel: " << ch;
}

// convert str to block method
HRDL_BLOCK_METHOD str2m(const char *str){
  if (strcasecmp(str,"block")==0)  return HRDL_BM_BLOCK;
  if (strcasecmp(str,"window")==0) return HRDL_BM_WINDOW;
  if (strcasecmp(str,"stream")==0) return HRDL_BM_STREAM;
  throw Err() << "error: unknown block method: " << str;
}

// convert voltage range
HRDL_RANGE volt2range(float v){
  int range = (int)(10000*v);
  switch (range){
    case 25000000: return HRDL_2500_MV; // 2500
    case 12500000: return HRDL_1250_MV; // 1250
    case  6250000: return HRDL_625_MV;  // 625
    case  3125000: return HRDL_313_MV;  // 312.5
    case  1562500: return HRDL_156_MV;  // 156.25
    case   781250: return HRDL_78_MV;   // 78.125
    case   390625: return HRDL_39_MV;   // 39.0625
  }
  throw Err() << "error: unknown input range: " << v;
}

const char * range2str(HRDL_RANGE r){
   switch (r){
    case HRDL_2500_MV: return "2500";
    case HRDL_1250_MV: return "1250";
    case  HRDL_625_MV: return "625";
    case  HRDL_313_MV: return "312.5";
    case  HRDL_156_MV: return "156.25";
    case   HRDL_78_MV: return "78.125";
    case   HRDL_39_MV: return "39.0625";
  }
  throw Err() << "error: unknown input range: " << r;
}

#endif


