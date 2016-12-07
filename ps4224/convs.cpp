#include <pico/ps4000Api.h>
#include <pico/PicoStatus.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> /*atof!*/
#include <string.h>
#include "err.h"

/* Convertions string <-> pico parameters */

/**********************************************************/
/* Error messages according to PicoStatus.h */
#define i2s_case(x) case PICO_ ## x: return #x
#define i2s_case2(x,y) case PICO_ ## x: return #x y
const char *
pico_err(int n){
  switch(n){
  i2s_case(OK);
  i2s_case(MAX_UNITS_OPENED);
  i2s_case(MEMORY_FAIL);
  i2s_case(NOT_FOUND);
  i2s_case(FW_FAIL);
  i2s_case(OPEN_OPERATION_IN_PROGRESS);
  i2s_case(OPERATION_FAILED);
  i2s_case(NOT_RESPONDING);
  i2s_case(CONFIG_FAIL);
  i2s_case(KERNEL_DRIVER_TOO_OLD);
  i2s_case(EEPROM_CORRUPT);
  i2s_case(OS_NOT_SUPPORTED);
  i2s_case(INVALID_HANDLE);
  i2s_case(INVALID_PARAMETER);
  i2s_case(INVALID_TIMEBASE);
  i2s_case(INVALID_VOLTAGE_RANGE);
  i2s_case(INVALID_CHANNEL);
  i2s_case(INVALID_TRIGGER_CHANNEL);
  i2s_case(INVALID_CONDITION_CHANNEL);
  i2s_case(NO_SIGNAL_GENERATOR);
  i2s_case(STREAMING_FAILED);
  i2s_case(BLOCK_MODE_FAILED);
  i2s_case(NULL_PARAMETER);
  i2s_case(ETS_MODE_SET);
  i2s_case(DATA_NOT_AVAILABLE);
  i2s_case(STRING_BUFFER_TO_SMALL);
  i2s_case(ETS_NOT_SUPPORTED);
  i2s_case(AUTO_TRIGGER_TIME_TO_SHORT);
  i2s_case(BUFFER_STALL);
  i2s_case(TOO_MANY_SAMPLES);
  i2s_case(TOO_MANY_SEGMENTS);
  i2s_case(PULSE_WIDTH_QUALIFIER);
  i2s_case(DELAY);
  i2s_case(SOURCE_DETAILS);
  i2s_case(CONDITIONS);
  i2s_case(USER_CALLBACK);
  i2s_case(DEVICE_SAMPLING);
  i2s_case(NO_SAMPLES_AVAILABLE);
  i2s_case(SEGMENT_OUT_OF_RANGE);
  i2s_case(BUSY);
  i2s_case(STARTINDEX_INVALID);
  i2s_case(INVALID_INFO);
  i2s_case(INFO_UNAVAILABLE);
  i2s_case(INVALID_SAMPLE_INTERVAL);
  i2s_case(TRIGGER_ERROR);
  i2s_case(MEMORY);
  i2s_case(SIG_GEN_PARAM);
  i2s_case(SHOTS_SWEEPS_WARNING);
  i2s_case(SIGGEN_TRIGGER_SOURCE);
  i2s_case(AUX_OUTPUT_CONFLICT);
  i2s_case(AUX_OUTPUT_ETS_CONFLICT);
  i2s_case(WARNING_EXT_THRESHOLD_CONFLICT);
  i2s_case(WARNING_AUX_OUTPUT_CONFLICT);
  i2s_case(SIGGEN_OUTPUT_OVER_VOLTAGE);
  i2s_case(DELAY_NULL);
  i2s_case(INVALID_BUFFER);
  i2s_case(SIGGEN_OFFSET_VOLTAGE);
  i2s_case(SIGGEN_PK_TO_PK);
  i2s_case(CANCELLED);
  i2s_case(SEGMENT_NOT_USED);
  i2s_case(INVALID_CALL);
  i2s_case(GET_VALUES_INTERRUPTED);
  i2s_case(NOT_USED);
  i2s_case(INVALID_SAMPLERATIO);
  i2s_case2(INVALID_STATE,
    " (Operation could not be carried out because device was in an invalid state)");
  i2s_case2(NOT_ENOUGH_SEGMENTS,
    " (Opeation could not be carried out as rapid capture no of waveforms"
    " are greater than the no of memoy segments)");
  i2s_case2(DRIVER_FUNCTION,
    " (A diver function has already been called and not yet finished."
    " Only one call to the diver can be made at any one time)");
  i2s_case(RESERVED);
  i2s_case(INVALID_COUPLING);
  i2s_case(BUFFERS_NOT_SET);
  i2s_case(RATIO_MODE_NOT_SUPPORTED);
  i2s_case(RAPID_NOT_SUPPORT_AGGREGATION);
  i2s_case(INVALID_TRIGGER_PROPERTY);
  i2s_case(INTERFACE_NOT_CONNECTED);
  i2s_case(RESISTANCE_AND_PROBE_NOT_ALLOWED);
  i2s_case(POWER_FAILED);
  i2s_case(SIGGEN_WAVEFORM_SETUP_FAILED);
  i2s_case(FPGA_FAIL);
  i2s_case(POWER_MANAGER);
  i2s_case(INVALID_ANALOGUE_OFFSET);
  i2s_case2(PLL_LOCK_FAILED,
    " (Unable to configue the ps6000");
  i2s_case2(ANALOG_BOARD,
    " (The ps6000 Analog boad is not detectly connected to the digital boad)");
  i2s_case2(CONFIG_FAIL_AWG,
    " (Unable to configue the Signal Generator)");
  i2s_case(INITIALISE_FPGA);
  i2s_case(EXTERNAL_FREQUENCY_INVALID);
  i2s_case(CLOCK_CHANGE_ERROR);
  i2s_case(TRIGGER_AND_EXTERNAL_CLOCK_CLASH);
  i2s_case(PWQ_AND_EXTERNAL_CLOCK_CLASH);
  i2s_case(UNABLE_TO_OPEN_SCALING_FILE);
  i2s_case(MEMORY_CLOCK_FREQUENCY);
  i2s_case(I2C_NOT_RESPONDING);
  i2s_case(NO_CAPTURES_AVAILABLE);
  i2s_case(NOT_USED_IN_THIS_CAPTURE_MODE);
  i2s_case(GET_DATA_ACTIVE);
  i2s_case2(IP_NETWORKED,
    " (used by the PT104 (USB) when connected via the Netwok Socket)");
  i2s_case(INVALID_IP_ADDRESS);
  i2s_case(IPSOCKET_FAILED);
  i2s_case(IPSOCKET_TIMEDOUT);
  i2s_case(SETTINGS_FAILED);
  i2s_case(NETWORK_FAILED);
  i2s_case(WS2_32_DLL_NOT_LOADED);
  i2s_case(INVALID_IP_PORT);
  i2s_case(COUPLING_NOT_SUPPORTED);
  i2s_case(BANDWIDTH_NOT_SUPPORTED);
  i2s_case(INVALID_BANDWIDTH);
  i2s_case(AWG_NOT_SUPPORTED);
  i2s_case(ETS_NOT_RUNNING);
  i2s_case(SIG_GEN_WHITENOISE_NOT_SUPPORTED);
  i2s_case(SIG_GEN_WAVETYPE_NOT_SUPPORTED);
  i2s_case(INVALID_DIGITAL_PORT);
  i2s_case(INVALID_DIGITAL_CHANNEL);
  i2s_case(INVALID_DIGITAL_TRIGGER_DIRECTION);
  i2s_case(SIG_GEN_PRBS_NOT_SUPPORTED);
  i2s_case(ETS_NOT_AVAILABLE_WITH_LOGIC_CHANNELS);
  i2s_case(WARNING_REPEAT_VALUE);
  i2s_case(POWER_SUPPLY_CONNECTED);
  i2s_case(POWER_SUPPLY_NOT_CONNECTED);
  i2s_case(POWER_SUPPLY_REQUEST_INVALID);
  i2s_case(POWER_SUPPLY_UNDERVOLTAGE);
  i2s_case(CAPTURING_DATA);
  i2s_case(USB3_0_DEVICE_NON_USB3_0_PORT);
  i2s_case(NOT_SUPPORTED_BY_THIS_DEVICE);
  i2s_case(INVALID_DEVICE_RESOLUTION);
  i2s_case(INVALID_NUMBER_CHANNELS_FOR_RESOLUTION);
  i2s_case(CHANNEL_DISABLED_DUE_TO_USB_POWERED);
  i2s_case(SIGGEN_DC_VOLTAGE_NOT_CONFIGURABLE);
  i2s_case(NO_TRIGGER_ENABLED_FOR_TRIGGER_IN_PRE_TRIG);
  i2s_case(TRIGGER_WITHIN_PRE_TRIG_NOT_ARMED);
  i2s_case(TRIGGER_WITHIN_PRE_NOT_ALLOWED_WITH_DELAY);
  i2s_case(TRIGGER_INDEX_UNAVAILABLE);
  i2s_case(AWG_CLOCK_FREQUENCY);
  i2s_case2(TOO_MANY_CHANNELS_IN_USE,
    " (There are more than 4 analogue channels with a trigger condition set)");
  i2s_case(NULL_CONDITIONS);
  i2s_case(DUPLICATE_CONDITION_SOURCE);
  i2s_case(INVALID_CONDITION_INFO);
  i2s_case(SETTINGS_READ_FAILED);
  i2s_case(SETTINGS_WRITE_FAILED);
  i2s_case(ARGUMENT_OUT_OF_RANGE);
  i2s_case(HARDWARE_VERSION_NOT_SUPPORTED);
  i2s_case(DIGITAL_HARDWARE_VERSION_NOT_SUPPORTED);
  i2s_case(ANALOGUE_HARDWARE_VERSION_NOT_SUPPORTED);
  i2s_case(UNABLE_TO_CONVERT_TO_RESISTANCE);
  i2s_case(DUPLICATED_CHANNEL);
  i2s_case(INVALID_RESISTANCE_CONVERSION);
  i2s_case(INVALID_VALUE_IN_MAX_BUFFER);
  i2s_case(INVALID_VALUE_IN_MIN_BUFFER);
  i2s_case(SIGGEN_FREQUENCY_OUT_OF_RANGE);
  i2s_case(DEVICE_TIME_STAMP_RESET);
  i2s_case(WATCHDOGTIMER);
  }
}

/**********************************************************/
/* str2bool (enable and help parameters)*/
int16_t
str2bool(const char *str){
  if (str==NULL || strlen(str)==0) return 1;
  return atoi(str)!=0;
}

/**********************************************************/
/* coupling AC,DC */
int16_t
str2coupl(const char *str){
  if (strcasecmp(str, "AC")==0) return 0;
  if (strcasecmp(str, "DC")==0) return 1;
  throw Err() << "error: unknown coupling (should be AC or DC): " << str;
}
const char *
coupl2str(int16_t n){
  return n? "DC":"AC";
}

/**********************************************************/
/* channel A,B,C,D,EXT */
PS4000_CHANNEL
str2chan(const char *str){
  if (strcasecmp(str, "A")==0) return PS4000_CHANNEL_A;
  if (strcasecmp(str, "B")==0) return PS4000_CHANNEL_B;
  if (strcasecmp(str, "C")==0) return PS4000_CHANNEL_C;
  if (strcasecmp(str, "D")==0) return PS4000_CHANNEL_D;
  if (strcasecmp(str, "EXT")==0) return PS4000_EXTERNAL;
  throw Err() << "error: unknown channel (should be A,B,C,D or EXT): " << str;
}
const char *
chan2str(int16_t n){
  if (n == PS4000_CHANNEL_A) return "A";
  if (n == PS4000_CHANNEL_B) return "B";
  if (n == PS4000_CHANNEL_C) return "C";
  if (n == PS4000_CHANNEL_D) return "D";
  if (n == PS4000_EXTERNAL) return "EXT";
  throw Err() << "error: unknown channel: " << n;
}

/**********************************************************/
/* convert sensitivity, Vpp */
PS4000_RANGE
volt2range(float v){
  int range = (int)(1000*v);
  switch (range){
    case    10: return PS4000_10MV;
    case    20: return PS4000_20MV;
    case    50: return PS4000_50MV;
    case   100: return PS4000_100MV;
    case   200: return PS4000_200MV;
    case   500: return PS4000_500MV;
    case  1000: return PS4000_1V;
    case  2000: return PS4000_2V;
    case  5000: return PS4000_5V;
    case 10000: return PS4000_10V;
    case 20000: return PS4000_20V;
    case 50000: return PS4000_50V;
    case 100000: return PS4000_100V;
  }
  throw Err() << "error: unknown input range: " << v;
}
const char *
range2str(PS4000_RANGE n){
  switch (n){
    case PS4000_10MV:  return "0.01";
    case PS4000_20MV:  return "0.02";
    case PS4000_50MV:  return "0.05";
    case PS4000_100MV: return "0.1";
    case PS4000_200MV: return "0.2";
    case PS4000_500MV: return "0.5";
    case PS4000_1V:    return "1";
    case PS4000_2V:    return "2";
    case PS4000_5V:    return "5";
    case PS4000_10V:   return "10";
    case PS4000_20V:   return "20";
    case PS4000_50V:   return "50";
    case PS4000_100V:  return "100";
  }
  throw Err() << "error: unknown input range: " << n;
}

/**********************************************************/
/* convert trigger direction */
THRESHOLD_DIRECTION
str2dir(const char *str){
  if (strcasecmp(str, "above")==0)   return ABOVE;
  if (strcasecmp(str, "below")==0)   return BELOW;
  if (strcasecmp(str, "rising")==0)  return RISING;
  if (strcasecmp(str, "falling")==0) return FALLING;
  if (strcasecmp(str, "rising_or_falling")==0) return RISING_OR_FALLING;
  throw Err() << "error: unknown trigger dir: " << str;
}

const char *
dir2str(THRESHOLD_DIRECTION n){
  switch (n){
    case ABOVE:   return "ABOVE";
    case BELOW:   return "BELOW";
    case RISING:  return "RISING";
    case FALLING: return "FALLING";
    case RISING_OR_FALLING: return "RISING_OR_FALLING";
  }
  throw Err() << "error: unknown trigger dir: " << n;
}

/**********************************************************/
/* convert wave type */
int16_t
str2wave(const char *str){
  if (strcasecmp(str, "sine")==0)      return PS4000_SINE;
  if (strcasecmp(str, "square")==0)    return PS4000_SQUARE;
  if (strcasecmp(str, "triangle")==0)  return PS4000_TRIANGLE;
  if (strcasecmp(str, "ramp_up")==0)   return PS4000_RAMP_UP;
  if (strcasecmp(str, "ramp_down")==0) return PS4000_RAMP_DOWN;
  if (strcasecmp(str, "sync")==0)      return PS4000_SINC;
  if (strcasecmp(str, "gaussian")==0)  return PS4000_GAUSSIAN;
  if (strcasecmp(str, "half_sine")==0) return PS4000_HALF_SINE;
  if (strcasecmp(str, "dc")==0)        return PS4000_DC_VOLTAGE;
  if (strcasecmp(str, "noise")==0)     return PS4000_WHITE_NOISE;
  throw Err() << "error: unknown wave type: " << str;
}

const char *
wave2str(int16_t n){
  switch (n){
    case PS4000_SINE:        return "SINE";
    case PS4000_SQUARE:      return "SQUARE";
    case PS4000_TRIANGLE:    return "TRIANGLE";
    case PS4000_RAMP_UP:     return "RAMP_UP";
    case PS4000_RAMP_DOWN:   return "RAMP_DOWN";
    case PS4000_SINC:        return "SINC";
    case PS4000_GAUSSIAN:    return "GAUSSIAN";
    case PS4000_HALF_SINE:   return "HALF_SINE";
    case PS4000_DC_VOLTAGE:  return "dc";
    case PS4000_WHITE_NOISE: return "noise";
  }
  throw Err() << "error: unknown wave type: " << n;
}

/**********************************************************/
/* convert sweep direction */
int16_t
str2sweep(const char *str){
  if (strcasecmp(str, "up")==0)      return UP;
  if (strcasecmp(str, "down")==0)    return DOWN;
  if (strcasecmp(str, "updown")==0)  return UPDOWN;
  if (strcasecmp(str, "downup")==0)  return DOWNUP;
  throw Err() << "error: unknown sweep type: " << str;
}
const char *
sweep2str(int16_t n){
  switch (n){
    case UP:     return "UP";
    case DOWN:   return "DOWN";
    case UPDOWN: return "UPDOWN";
    case DOWNUP: return "DOWNUP";
  }
  throw Err() << "error: unknown sweep type: " << n;
}

/**********************************************************/
/* convert generator voltage */
int32_t
str2gen_volt(const char *str){
  return (int32_t)(1e6*atof(str));
}

/**********************************************************/
/* convert time units */
PS4000_TIME_UNITS
str2tunits(const char *str){
  if (strcasecmp(str, "fs")==0)    return PS4000_FS;
  if (strcasecmp(str, "ps")==0)    return PS4000_PS;
  if (strcasecmp(str, "ns")==0)    return PS4000_NS;
  if (strcasecmp(str, "us")==0)    return PS4000_US;
  if (strcasecmp(str, "ms")==0)    return PS4000_MS;
  if (strcasecmp(str, "s")==0)    return PS4000_S;
  throw Err() << "error: unknown time units: " << str;
}
const char *
tunits2str(PS4000_TIME_UNITS n){
  switch (n){
    case PS4000_FS: return "fs";
    case PS4000_PS: return "ps";
    case PS4000_NS: return "ns";
    case PS4000_US: return "us";
    case PS4000_MS: return "ms";
    case PS4000_S:  return "s";
  }
  throw Err() << "error: unknown time units: " << n;
}
double
time2dbl(int64_t t, PS4000_TIME_UNITS tu){
  switch (tu){
    case PS4000_FS: return (double)t * 1e-15;
    case PS4000_PS: return (double)t * 1e-12;
    case PS4000_NS: return (double)t * 1e-9;
    case PS4000_US: return (double)t * 1e-6;
    case PS4000_MS: return (double)t * 1e-3;
    case PS4000_S:  return (double)t;
  }
  throw Err() << "error: unknown time units: %i\n" << tu;
}
