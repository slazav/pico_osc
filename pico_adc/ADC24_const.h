#ifndef ADC24_CONST_H
#define ADC24_CONST_H

#include <pico/HRDL.h>
#include <sstream>

// Functions for converting ADC24 constants
// from/to string representation.

/************************************************/
template<typename T>
T str_to_type(const std::string & s){
  std::istringstream ss(s);
  T val;
  ss >> std::showbase >> val;
  if (ss.fail() || !ss.eof())
    throw Err() << "can't parse value: \"" << s << "\"";
  return val;
}
template<typename T>
std::string type_to_str(const T & t){
  std::ostringstream ss;
  ss << t;
  return ss.str();
}

/************************************************/
// Conversion time.
// String values have same format as in get_tconvs()
// output: 60 100 180 340 660.

HRDL_CONVERSION_TIME str_to_convt(const std::string & s){
  if (s== "60") return HRDL_60MS;
  if (s=="100") return HRDL_100MS;
  if (s=="180") return HRDL_180MS;
  if (s=="340") return HRDL_340MS;
  if (s=="660") return HRDL_660MS;
  throw Err() << "error: unknown conversion time: " << s;
}

std::string convt_to_str(const HRDL_CONVERSION_TIME tconv){
  switch (tconv){
    case  HRDL_60MS: return "60";
    case HRDL_100MS: return "100";
    case HRDL_180MS: return "180";
    case HRDL_340MS: return "340";
    case HRDL_660MS: return "660";
  }
  throw Err() << "error: unknown conversion time: " << tconv;
}

/************************************************/
// Voltage range.
// String values have same format as in get_ranges()
// output: 2500 1250 625 312.5 156.25 78.125 39.0625

HRDL_RANGE str_to_range(const std::string & s){
  if (s=="2500")    return HRDL_2500_MV;
  if (s=="1250")    return HRDL_1250_MV;
  if (s=="625")     return HRDL_625_MV;
  if (s=="312.5")   return HRDL_313_MV;
  if (s=="156.25")  return HRDL_156_MV;
  if (s=="78.125")  return HRDL_78_MV;
  if (s=="39.0625") return HRDL_39_MV;
  throw Err() << "error: unknown input range: " << s;
}

std::string range_to_str(const HRDL_RANGE r){
   switch (r){
    case HRDL_2500_MV: return "2500";
    case HRDL_1250_MV: return "1250";
    case HRDL_625_MV:  return "625";
    case HRDL_313_MV:  return "312.5";
    case HRDL_156_MV:  return "156.25";
    case HRDL_78_MV:   return "78.125";
    case HRDL_39_MV:   return "39.0625";
  }
  throw Err() << "error: unknown input range: " << r;
}

double str_to_volt(const std::string & s){
  if (s=="2500")    return  2.5;
  if (s=="1250")    return  1.25;
  if (s=="625")     return  0.625;
  if (s=="312.5")   return  0.3125;
  if (s=="156.25")  return  0.15625;
  if (s=="78.125")  return  0.078125;
  if (s=="39.0625") return  0.0390625;
  throw Err() << "error: unknown input range: " << s;
}

double range_to_volt(const HRDL_RANGE r){
  return(str_to_volt(range_to_str(r)));
}

/************************************************/

// digital channel
HRDL_DIGITAL_IO_CHANNEL int_to_dch(const int i){
  switch(i){
    case 1: return HRDL_DIGITAL_IO_CHANNEL_1;
    case 2: return HRDL_DIGITAL_IO_CHANNEL_2;
    case 3: return HRDL_DIGITAL_IO_CHANNEL_3;
    case 4: return HRDL_DIGITAL_IO_CHANNEL_4;
  }
  throw Err() << "error: unknown digital channel: " << i;
}

HRDL_DIGITAL_IO_CHANNEL str_to_dch(const std::string & s){
  return int_to_dch(str_to_type<int>(s));
}

int dch_to_int(const HRDL_DIGITAL_IO_CHANNEL ch){
  switch (ch){
    case HRDL_DIGITAL_IO_CHANNEL_1: return 1;
    case HRDL_DIGITAL_IO_CHANNEL_2: return 2;
    case HRDL_DIGITAL_IO_CHANNEL_3: return 3;
    case HRDL_DIGITAL_IO_CHANNEL_4: return 4;
  }
  throw Err() << "error: unknown digital channel: " << ch;
}

/************************************************/
// analog channel
HRDL_INPUTS int_to_ch(const int i){
  switch (i){
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
  throw Err() << "error: unknown analog channel: " << i;
}

HRDL_INPUTS str_to_ch(const std::string & s){
  return int_to_ch(str_to_type<int>(s));
}

int ch_to_int(const HRDL_INPUTS ch){
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

/************************************************/
// Block method (not used).

HRDL_BLOCK_METHOD str_to_blockm(const std::string & s){
  if (strcasecmp(s.c_str(),"block")==0)  return HRDL_BM_BLOCK;
  if (strcasecmp(s.c_str(),"window")==0) return HRDL_BM_WINDOW;
  if (strcasecmp(s.c_str(),"stream")==0) return HRDL_BM_STREAM;
  throw Err() << "error: unknown block method: " << s;
}

/************************************************/
// errors:

std::string err_to_str(const int e){
  switch ((enHRDLErrorCode)e){
    case HRDL_OK:            return "No error";
    case HRDL_KERNEL_DRIVER: return "driver error";
    case HRDL_NOT_FOUND:     return "device not found";
    case HRDL_CONFIG_FAIL:   return "device configuration error";
    case HRDL_ERROR_OS_NOT_SUPPORTED: return "OS not supported";
    case HRDL_MAX_DEVICES:   return "device limit reached";
  }
  return "unknown error";
}

std::string serr_to_str(const int e){
  switch ((enSettingsError)e){
    case SE_CONVERSION_TIME_OUT_OF_RANGE: return "conversion time out of range";
    case SE_SAMPLEINTERVAL_OUT_OF_RANGE: return "sampling interval out of range";
    case SE_CONVERSION_TIME_TOO_SLOW: return "conversion time too slow";
    case SE_CHANNEL_NOT_AVAILABLE: return "channel not available";
    case SE_INVALID_CHANNEL: return "invalid channel";
    case SE_INVALID_VOLTAGE_RANGE: return "invalid voltage range";
    case SE_INVALID_PARAMETER: return "invalid parameter";
    case SE_CONVERSION_IN_PROGRESS: return "conversion in progress";
    case SE_COMMUNICATION_FAILED: return "communication failed";
    case SE_OK: return "No coniguration error";
  }
  return "unknown error";
}

#endif


