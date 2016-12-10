#include "pico.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> /*atof!*/
#include <string.h>

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
/* coupling AC/DC */
int16_t
str2coupling(const char *str){
  if (strcasecmp(str, "AC")==0) return PS_AC;
  if (strcasecmp(str, "DC")==0) return PS_DC;
  printf("error: unknown coupling (should be AC or DC): %s\n", str);
  return -1;
}
const char *
coupling2str(int16_t n){
  if (n == PS_AC) return "AC";
  if (n == PS_DC) return "DC";
  printf("error: unknown coupling: %i\n", n);
  return "";
}

/**********************************************************/
/* channel A,B,C,D,EXT */
int16_t
str2chan(const char *str){
  if (strcasecmp(str, "A")==0) return PS3000A_CHANNEL_A;
  if (strcasecmp(str, "B")==0) return PS3000A_CHANNEL_B;
  if (strcasecmp(str, "C")==0) return PS3000A_CHANNEL_C;
  if (strcasecmp(str, "D")==0) return PS3000A_CHANNEL_D;
  if (strcasecmp(str, "EXT")==0) return PS3000A_EXTERNAL;
  printf("error: unknown channel (should be A,B,C,D or EXT): %s\n", str);
  return -1;
}
const char *
chan2str(int16_t n){
  if (n == PS3000A_CHANNEL_A) return "A";
  if (n == PS3000A_CHANNEL_B) return "B";
  if (n == PS3000A_CHANNEL_C) return "C";
  if (n == PS3000A_CHANNEL_D) return "D";
  if (n == PS3000A_EXTERNAL) return "EXT";
  printf("error: unknown channel: %i\n", n);
  return "";
}

/**********************************************************/
/* convert sensitivity, Vpp */
int16_t
str2range(const char *str){
  int range = (int)(1000*atof(str)/2.0);
  switch (range){
    case    10: return PS3000A_10MV;
    case    20: return PS3000A_20MV;
    case    50: return PS3000A_50MV;
    case   100: return PS3000A_100MV;
    case   200: return PS3000A_200MV;
    case   500: return PS3000A_500MV;
    case  1000: return PS3000A_1V;
    case  2000: return PS3000A_2V;
    case  5000: return PS3000A_5V;
    case 10000: return PS3000A_10V;
    case 20000: return PS3000A_20V;
    case 50000: return PS3000A_50V;
  }
  printf("error: unknown input range: %s\n", str);
  return -1;
}
const char *
range2str(int16_t n){
  switch (n){
    case PS3000A_10MV:  return "0.02";
    case PS3000A_20MV:  return "0.04";
    case PS3000A_50MV:  return "0.1";
    case PS3000A_100MV: return "0.2";
    case PS3000A_200MV: return "0.4";
    case PS3000A_500MV: return "1";
    case PS3000A_1V:    return "2";
    case PS3000A_2V:    return "4";
    case PS3000A_5V:    return "10";
    case PS3000A_10V:   return "20";
    case PS3000A_20V:   return "40";
    case PS3000A_50V:   return "100";
  }
  printf("error: unknown input range: %i\n", n);
  return "";
}

/**********************************************************/
/* convert trigger direction */
int16_t
str2dir(const char *str){
  if (strcasecmp(str, "above")==0)   return PS3000A_ABOVE;
  if (strcasecmp(str, "below")==0)   return PS3000A_BELOW;
  if (strcasecmp(str, "rising")==0)  return PS3000A_RISING;
  if (strcasecmp(str, "falling")==0) return PS3000A_FALLING;
  if (strcasecmp(str, "rising_or_falling")==0) return PS3000A_RISING_OR_FALLING;
  printf("error: unknown trigger dir: %s\n", str);
  return -1;
}
const char *
dir2str(int16_t n){
  switch (n){
    case PS3000A_ABOVE:   return "ABOVE";
    case PS3000A_BELOW:   return "BELOW";
    case PS3000A_RISING:  return "RISING";
    case PS3000A_FALLING: return "FALLING";
    case PS3000A_RISING_OR_FALLING: return "RISING_OR_FALLING";
  }
  printf("error: unknown trigger dir: %i\n", n);
  return "";
}

/**********************************************************/
/* convert wave type */
int16_t
str2wave(const char *str){
  if (strcasecmp(str, "sine")==0)      return PS3000A_SINE;
  if (strcasecmp(str, "square")==0)    return PS3000A_SQUARE;
  if (strcasecmp(str, "triangle")==0)  return PS3000A_TRIANGLE;
  if (strcasecmp(str, "ramp_up")==0)   return PS3000A_RAMP_UP;
  if (strcasecmp(str, "ramp_down")==0) return PS3000A_RAMP_DOWN;
  if (strcasecmp(str, "sync")==0)      return PS3000A_SINC;
  if (strcasecmp(str, "gaussian")==0)  return PS3000A_GAUSSIAN;
  if (strcasecmp(str, "half_sine")==0) return PS3000A_HALF_SINE;
  if (strcasecmp(str, "dc")==0)        return PS3000A_DC_VOLTAGE;
  printf("error: unknown wave type: %s\n", str);
  return -1;
}
const char *
wave2str(int16_t n){
  switch (n){
    case PS3000A_SINE:       return "SINE";
    case PS3000A_SQUARE:     return "SQUARE";
    case PS3000A_TRIANGLE:   return "TRIANGLE";
    case PS3000A_RAMP_UP:    return "RAMP_UP";
    case PS3000A_RAMP_DOWN:  return "RAMP_DOWN";
    case PS3000A_SINC:       return "SINC";
    case PS3000A_GAUSSIAN:   return "GAUSSIAN";
    case PS3000A_HALF_SINE:  return "HALF_SINE";
    case PS3000A_DC_VOLTAGE: return "DC_VOLTAGE";
  }
  printf("error: unknown wave type: %i\n", n);
  return "";
}

/**********************************************************/
/* convert sweep direction */
int16_t
str2sweep(const char *str){
  if (strcasecmp(str, "up")==0)      return PS3000A_UP;
  if (strcasecmp(str, "down")==0)    return PS3000A_DOWN;
  if (strcasecmp(str, "updown")==0)  return PS3000A_UPDOWN;
  if (strcasecmp(str, "downup")==0)  return PS3000A_DOWNUP;
  printf("error: unknown sweep type: %s\n", str);
  return -1;
}
const char *
sweep2str(int16_t n){
  switch (n){
    case PS3000A_UP:     return "UP";
    case PS3000A_DOWN:   return "DOWN";
    case PS3000A_UPDOWN: return "UPDOWN";
    case PS3000A_DOWNUP: return "DOWNUP";
  }
  printf("error: unknown sweep type: %i\n", n);
  return "";
}

/**********************************************************/
/* convert generator voltage */
int32_t
str2gen_volt(const char *str){
  return (int32_t)(1e6*atof(str));
}

/**********************************************************/
/* convert generator trigger source */
int16_t
str2gen_trigsrc(const char *str){
  if (strcasecmp(str, "none")==0)      return PS3000A_SIGGEN_NONE;
  if (strcasecmp(str, "scope")==0)     return PS3000A_SIGGEN_SCOPE_TRIG;
  if (strcasecmp(str, "aux")==0)       return PS3000A_SIGGEN_AUX_IN;
  if (strcasecmp(str, "ext")==0)       return PS3000A_SIGGEN_EXT_IN;
  if (strcasecmp(str, "soft")==0)      return PS3000A_SIGGEN_SOFT_TRIG;
  printf("error: unknown generator trigger source: %s\n", str);
  return -1;
}
const char *
gen_trigsrc2str(int16_t n){
  switch (n){
    case PS3000A_SIGGEN_NONE:       return "NONE";
    case PS3000A_SIGGEN_SCOPE_TRIG: return "SCOPE";
    case PS3000A_SIGGEN_AUX_IN:     return "AUX";
    case PS3000A_SIGGEN_EXT_IN:     return "EXT";
    case PS3000A_SIGGEN_SOFT_TRIG:  return "SOFT";
  }
  printf("error: unknown generator trigger source: %i\n", n);
  return "";
}

/**********************************************************/
/* convert generator trigger direction */
int16_t
str2gen_trigdir(const char *str){
  if (strcasecmp(str, "rising")==0)    return PS3000A_SIGGEN_RISING;
  if (strcasecmp(str, "falling")==0)   return PS3000A_SIGGEN_FALLING;
  if (strcasecmp(str, "gate_high")==0) return PS3000A_SIGGEN_GATE_HIGH;
  if (strcasecmp(str, "gate_low")==0)  return PS3000A_SIGGEN_GATE_LOW;
  printf("error: unknown generator trigger dir: %s\n", str);
  return -1;
}
const char *
gen_trigdir2str(int16_t n){
  switch (n){
    case PS3000A_SIGGEN_RISING:    return "RISING";
    case PS3000A_SIGGEN_FALLING:   return "FALLING";
    case PS3000A_SIGGEN_GATE_HIGH: return "GATE_HIGH";
    case PS3000A_SIGGEN_GATE_LOW:  return "GATE_LOW";
  }
  printf("error: unknown generator trigger dir: %i\n", n);
  return "";
}

/**********************************************************/
/* convert time units */
int16_t
str2tunits(const char *str){
  if (strcasecmp(str, "fs")==0)    return PS3000A_FS;
  if (strcasecmp(str, "ps")==0)    return PS3000A_PS;
  if (strcasecmp(str, "ns")==0)    return PS3000A_NS;
  if (strcasecmp(str, "us")==0)    return PS3000A_US;
  if (strcasecmp(str, "ms")==0)    return PS3000A_MS;
  if (strcasecmp(str, "s")==0)    return PS3000A_S;
  printf("error: unknown time units: %s\n", str);
  return -1;
}
const char *
tunits2str(int16_t n){
  switch (n){
    case PS3000A_FS: return "fs";
    case PS3000A_PS: return "ps";
    case PS3000A_NS: return "ns";
    case PS3000A_US: return "us";
    case PS3000A_MS: return "ms";
    case PS3000A_S:  return "s";
  }
  printf("error: unknown time units: %i\n", n);
  return "";
}
double
time2dbl(int64_t t, PS3000A_TIME_UNITS tu){
  switch (tu){
    case PS3000A_FS: return (double)t * 1e-15;
    case PS3000A_PS: return (double)t * 1e-12;
    case PS3000A_NS: return (double)t * 1e-9;
    case PS3000A_US: return (double)t * 1e-6;
    case PS3000A_MS: return (double)t * 1e-3;
    case PS3000A_S:  return (double)t;
  }
  printf("error: unknown time units: %i\n", tu);
  return 0;
}