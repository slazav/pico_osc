#include <ps3000aApi.h>

/* error messages according to PicoStatus.h */
const char *
pico_err(int n){
  switch(n){
  case 0x00000000UL: return "OK";
  case 0x00000001UL: return "MAX_UNITS_OPENED";
  case 0x00000002UL: return "MEMORY_FAIL";
  case 0x00000003UL: return "NOT_FOUND";
  case 0x00000004UL: return "FW_FAIL";
  case 0x00000005UL: return "OPEN_OPERATION_IN_PROGRESS";
  case 0x00000006UL: return "OPERATION_FAILED";
  case 0x00000007UL: return "NOT_RESPONDING";
  case 0x00000008UL: return "CONFIG_FAIL";
  case 0x00000009UL: return "KERNEL_DRIVER_TOO_OLD";
  case 0x0000000AUL: return "EEPROM_CORRUPT";
  case 0x0000000BUL: return "OS_NOT_SUPPORTED";
  case 0x0000000CUL: return "INVALID_HANDLE";
  case 0x0000000DUL: return "INVALID_PARAMETER";
  case 0x0000000EUL: return "INVALID_TIMEBASE";
  case 0x0000000FUL: return "INVALID_VOLTAGE_RANGE";
  case 0x00000010UL: return "INVALID_CHANNEL";
  case 0x00000011UL: return "INVALID_TRIGGER_CHANNEL";
  case 0x00000012UL: return "INVALID_CONDITION_CHANNEL";
  case 0x00000013UL: return "NO_SIGNAL_GENERATOR";
  case 0x00000014UL: return "STREAMING_FAILED";
  case 0x00000015UL: return "BLOCK_MODE_FAILED";
  case 0x00000016UL: return "NULL_PARAMETER";
  case 0x00000017UL: return "ETS_MODE_SET";
  case 0x00000018UL: return "DATA_NOT_AVAILABLE";
  case 0x00000019UL: return "STRING_BUFFER_TO_SMALL";
  case 0x0000001AUL: return "ETS_NOT_SUPPORTED";
  case 0x0000001BUL: return "AUTO_TRIGGER_TIME_TO_SHORT";
  case 0x0000001CUL: return "BUFFER_STALL";
  case 0x0000001DUL: return "TOO_MANY_SAMPLES";
  case 0x0000001EUL: return "TOO_MANY_SEGMENTS";
  case 0x0000001FUL: return "PULSE_WIDTH_QUALIFIER";
  case 0x00000020UL: return "DELAY";
  case 0x00000021UL: return "SOURCE_DETAILS";
  case 0x00000022UL: return "CONDITIONS";
  case 0x00000023UL: return "USER_CALLBACK";
  case 0x00000024UL: return "DEVICE_SAMPLING";
  case 0x00000025UL: return "NO_SAMPLES_AVAILABLE";
  case 0x00000026UL: return "SEGMENT_OUT_OF_RANGE";
  case 0x00000027UL: return "BUSY";
  case 0x00000028UL: return "STARTINDEX_INVALID";
  case 0x00000029UL: return "INVALID_INFO";
  case 0x0000002AUL: return "INFO_UNAVAILABLE";
  case 0x0000002BUL: return "INVALID_SAMPLE_INTERVAL";
  case 0x0000002CUL: return "TRIGGER_ERROR";
  case 0x0000002DUL: return "MEMORY";
  case 0x0000002EUL: return "SIG_GEN_PARAM";
  case 0x0000002FUL: return "SHOTS_SWEEPS_WARNING";
  case 0x00000030UL: return "SIGGEN_TRIGGER_SOURCE";
  case 0x00000031UL: return "AUX_OUTPUT_CONFLICT";
  case 0x00000032UL: return "AUX_OUTPUT_ETS_CONFLICT";
  case 0x00000033UL: return "WARNING_EXT_THRESHOLD_CONFLICT";
  case 0x00000034UL: return "WARNING_AUX_OUTPUT_CONFLICT";
  case 0x00000035UL: return "SIGGEN_OUTPUT_OVER_VOLTAGE";
  case 0x00000036UL: return "DELAY_NULL";
  case 0x00000037UL: return "INVALID_BUFFER";
  case 0x00000038UL: return "SIGGEN_OFFSET_VOLTAGE";
  case 0x00000039UL: return "SIGGEN_PK_TO_PK";
  case 0x0000003AUL: return "CANCELLED";
  case 0x0000003BUL: return "SEGMENT_NOT_USED";
  case 0x0000003CUL: return "INVALID_CALL";
  case 0x0000003DUL: return "GET_VALUES_INTERRUPTED";
  case 0x0000003FUL: return "NOT_USED";
  case 0x00000040UL: return "INVALID_SAMPLERATIO";
  case 0x00000041UL: return "INVALID_STATE"
    " (Operation could not be carried out because device was in an invalid state)";
  case 0x00000042UL: return "NOT_ENOUGH_SEGMENTS"
    " (Opeation could not be carried out as rapid capture no of waveforms"
    " are greater than the no of memoy segments)";
  case 0x00000043UL: return "DRIVER_FUNCTION"
    " (A diver function has already been called and not yet finished."
    " Only one call to the diver can be made at any one time)";
  case 0x00000044UL: return "RESERVED";
  case 0x00000045UL: return "INVALID_COUPLING";
  case 0x00000046UL: return "BUFFERS_NOT_SET";
  case 0x00000047UL: return "RATIO_MODE_NOT_SUPPORTED";
  case 0x00000048UL: return "RAPID_NOT_SUPPORT_AGGREGATION";
  case 0x00000049UL: return "INVALID_TRIGGER_PROPERTY";
  case 0x0000004AUL: return "INTERFACE_NOT_CONNECTED";
  case 0x0000004BUL: return "RESISTANCE_AND_PROBE_NOT_ALLOWED";
  case 0x0000004CUL: return "POWER_FAILED";
  case 0x0000004DUL: return "SIGGEN_WAVEFORM_SETUP_FAILED";
  case 0x0000004EUL: return "FPGA_FAIL";
  case 0x0000004FUL: return "POWER_MANAGER";
  case 0x00000050UL: return "INVALID_ANALOGUE_OFFSET";
  case 0x00000051UL: return "PLL_LOCK_FAILED"
    " (Unable to configue the ps6000)";
  case 0x00000052UL: return "ANALOG_BOARD"
    " (The ps6000 Analog boad is not detectly connected to the digital boad)";
  case 0x00000053UL: return "CONFIG_FAIL_AWG"
    " (Unable to configue the Signal Generator)";
  case 0x00000054UL: return "INITIALISE_FPGA";
  case 0x00000056UL: return "EXTERNAL_FREQUENCY_INVALID";
  case 0x00000057UL: return "CLOCK_CHANGE_ERROR";
  case 0x00000058UL: return "TRIGGER_AND_EXTERNAL_CLOCK_CLASH";
  case 0x00000059UL: return "PWQ_AND_EXTERNAL_CLOCK_CLASH";
  case 0x0000005AUL: return "UNABLE_TO_OPEN_SCALING_FILE";
  case 0x0000005BUL: return "MEMORY_CLOCK_FREQUENCY";
  case 0x0000005CUL: return "I2C_NOT_RESPONDING";
  case 0x0000005DUL: return "NO_CAPTURES_AVAILABLE";
  case 0x0000005EUL: return "NOT_USED_IN_THIS_CAPTURE_MODE";
  case 0x00000103UL: return "GET_DATA_ACTIVE";
  case 0x00000104UL: return "IP_NETWORKED"
    " (used by the PT104 (USB) when connected via the Netwok Socket)";
  case 0x00000105UL: return "INVALID_IP_ADDRESS";
  case 0x00000106UL: return "IPSOCKET_FAILED";
  case 0x00000107UL: return "IPSOCKET_TIMEDOUT";
  case 0x00000108UL: return "SETTINGS_FAILED";
  case 0x00000109UL: return "NETWORK_FAILED";
  case 0x0000010AUL: return "WS2_32_DLL_NOT_LOADED";
  case 0x0000010BUL: return "INVALID_IP_PORT";
  case 0x0000010CUL: return "COUPLING_NOT_SUPPORTED";
  case 0x0000010DUL: return "BANDWIDTH_NOT_SUPPORTED";
  case 0x0000010EUL: return "INVALID_BANDWIDTH";
  case 0x0000010FUL: return "AWG_NOT_SUPPORTED";
  case 0x00000110UL: return "ETS_NOT_RUNNING";
  case 0x00000111UL: return "SIG_GEN_WHITENOISE_NOT_SUPPORTED";
  case 0x00000112UL: return "SIG_GEN_WAVETYPE_NOT_SUPPORTED";
  case 0x00000113UL: return "INVALID_DIGITAL_PORT";
  case 0x00000114UL: return "INVALID_DIGITAL_CHANNEL";
  case 0x00000115UL: return "INVALID_DIGITAL_TRIGGER_DIRECTION";
  case 0x00000116UL: return "SIG_GEN_PRBS_NOT_SUPPORTED";
  case 0x00000117UL: return "ETS_NOT_AVAILABLE_WITH_LOGIC_CHANNELS";
  case 0x00000118UL: return "WARNING_REPEAT_VALUE";
  case 0x00000119UL: return "POWER_SUPPLY_CONNECTED";
  case 0x0000011AUL: return "POWER_SUPPLY_NOT_CONNECTED";
  case 0x0000011BUL: return "POWER_SUPPLY_REQUEST_INVALID";
  case 0x0000011CUL: return "POWER_SUPPLY_UNDERVOLTAGE";
  case 0x0000011DUL: return "CAPTURING_DATA";
  case 0x0000011EUL: return "USB3_0_DEVICE_NON_USB3_0_PORT";
  case 0x0000011FUL: return "NOT_SUPPORTED_BY_THIS_DEVICE";
  case 0x00000120UL: return "INVALID_DEVICE_RESOLUTION";
  case 0x00000121UL: return "INVALID_NUMBER_CHANNELS_FOR_RESOLUTION";
  case 0x00000122UL: return "CHANNEL_DISABLED_DUE_TO_USB_POWERED";
  case 0x00000123UL: return "SIGGEN_DC_VOLTAGE_NOT_CONFIGURABLE";
  case 0x00000124UL: return "NO_TRIGGER_ENABLED_FOR_TRIGGER_IN_PRE_TRIG";
  case 0x00000125UL: return "TRIGGER_WITHIN_PRE_TRIG_NOT_ARMED";
  case 0x00000126UL: return "TRIGGER_WITHIN_PRE_NOT_ALLOWED_WITH_DELAY";
  case 0x00000127UL: return "TRIGGER_INDEX_UNAVAILABLE";
  case 0x00000128UL: return "AWG_CLOCK_FREQUENCY";
  case 0x00000129UL: return "TOO_MANY_CHANNELS_IN_USE"
    " (There are more than 4 analogue channels with a trigger condition set)";
  case 0x0000012AUL: return "NULL_CONDITIONS";
  case 0x0000012BUL: return "DUPLICATE_CONDITION_SOURCE";
  case 0x0000012CUL: return "INVALID_CONDITION_INFO";
    case 0x0000012DUL: return "SETTINGS_READ_FAILED";
  case 0x0000012EUL: return "SETTINGS_WRITE_FAILED";
  case 0x0000012FUL: return "ARGUMENT_OUT_OF_RANGE";
  case 0x00000130UL: return "HARDWARE_VERSION_NOT_SUPPORTED";
  case 0x00000131UL: return "DIGITAL_HARDWARE_VERSION_NOT_SUPPORTED";
  case 0x00000132UL: return "ANALOGUE_HARDWARE_VERSION_NOT_SUPPORTED";
  case 0x00000133UL: return "UNABLE_TO_CONVERT_TO_RESISTANCE";
  case 0x00000134UL: return "DUPLICATED_CHANNEL";
  case 0x00000135UL: return "INVALID_RESISTANCE_CONVERSION";
  case 0x00000136UL: return "INVALID_VALUE_IN_MAX_BUFFER";
  case 0x00000137UL: return "INVALID_VALUE_IN_MIN_BUFFER";
  case 0x00000138UL: return "SIGGEN_FREQUENCY_OUT_OF_RANGE";
  case 0x01000000UL: return "DEVICE_TIME_STAMP_RESET";
  case 0x10000000UL: return "WATCHDOGTIMER";
  }
}

void
print_err(n){
  if (n) printf("error: %s\n", pico_err(n));
  else  printf("OK\n");
}

int
str2coupling(const char *str){
  if (strcasecmp(str, "ac")==0) return PS3000A_AC;
  if (strcasecmp(str, "dc")==0) return PS3000A_DC;
  printf("error: coupling should be AC or DC\n");
  return -1;
}

int
str2chan(const char *str){
  if (strcasecmp(str, "a")==0) return PS3000A_CHANNEL_A;
  if (strcasecmp(str, "b")==0) return PS3000A_CHANNEL_B;
  if (strcasecmp(str, "ext")) return PS3000A_EXTERNAL;
  printf("error: coupling should be AC or DC\n");
  return -1;
}


/* convert sensitivity, Vpp */
int
str2range(const char *str){
  int range = (1000*atof(str)/2);
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
/* convert sensitivity, Vpp */
const char *
range2str(int n){
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
  return "";
}

/* convert trigger dir */
int
str2dir(const char *str){
  if (strcasecmp(str, "above")==0)   return PS3000A_ABOVE;
  if (strcasecmp(str, "below")==0)   return PS3000A_BELOW;
  if (strcasecmp(str, "rising")==0) return PS3000A_RISING;
  if (strcasecmp(str, "falling")==0) return PS3000A_FALLING;
  if (strcasecmp(str, "rising_or_falling")==0) return PS3000A_RISING_OR_FALLING;
  printf("error: unknown trigger dir: %s\n", str);
  return -1;
}

int
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

int
str2sweep(const char *str){
  if (strcasecmp(str, "up")==0)      return PS3000A_UP;
  if (strcasecmp(str, "down")==0)    return PS3000A_DOWN;
  if (strcasecmp(str, "updown")==0)  return PS3000A_UPDOWN;
  if (strcasecmp(str, "downup")==0)  return PS3000A_DOWNUP;
  printf("error: unknown sweep type: %s\n", str);
  return -1;
}

int
str2siggen_src(const char *str){
  if (strcasecmp(str, "none")==0)      return PS3000A_SIGGEN_NONE;
  if (strcasecmp(str, "scope")==0)     return PS3000A_SIGGEN_SCOPE_TRIG;
  if (strcasecmp(str, "aux")==0)       return PS3000A_SIGGEN_AUX_IN;
  if (strcasecmp(str, "ext")==0)       return PS3000A_SIGGEN_EXT_IN;
  if (strcasecmp(str, "soft")==0)      return PS3000A_SIGGEN_SOFT_TRIG;
  printf("error: unknown signal generator trigger source: %s\n", str);
  return -1;
}

int
str2siggen_dir(const char *str){
  if (strcasecmp(str, "rising")==0)    return PS3000A_SIGGEN_RISING;
  if (strcasecmp(str, "falling")==0)   return PS3000A_SIGGEN_FALLING;
  if (strcasecmp(str, "high")==0)      return PS3000A_SIGGEN_GATE_HIGH;
  if (strcasecmp(str, "low")==0)       return PS3000A_SIGGEN_GATE_LOW;
  printf("error: unknown signal generator trigger source: %s\n", str);
  return -1;
}



