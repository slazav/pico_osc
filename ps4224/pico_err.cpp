// Error messages according to PicoStatus.h

#include <pico/PicoStatus.h>
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

