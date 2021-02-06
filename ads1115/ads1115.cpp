#include <unistd.h>
#include <iostream>
#include <iomanip>

#include "err/err.h"
#include "read_words/read_words.h"

#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <errno.h>
#include <cstring>

#include <sys/time.h> // gettimeofday

#define VERSION "1.0"

/* SPP interface to ADS1113/1114/1115 ADC converters (i2c connection) */

using namespace std;

/*********************************************************************/
// compare strings
bool
is_cmd(const vector<string> & args, const char *name){
  return strcasecmp(args[0].c_str(), name)==0;
}

// device registers
#define REG_CONV 0
#define REG_CONF 1
#define REG_LTHR 2
#define REG_HTHR 3


// I2C communication (see https://github.com/shenki/linux-i2c-example)
static inline int32_t
i2c_smbus_access(int file, char read_write, __u8 command,
                 int size, union i2c_smbus_data *data){
  struct i2c_smbus_ioctl_data args;
  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;
  return ioctl(file,I2C_SMBUS,&args);
}


static inline int32_t
i2c_smbus_read_byte_data(int file, __u8 command){
  union i2c_smbus_data data;
  if (i2c_smbus_access(file,I2C_SMBUS_READ,command,I2C_SMBUS_BYTE_DATA,&data))
    return -1;
  else
    return 0x0FF & data.byte;
}

static inline int32_t
i2c_smbus_read_word_data(int file, __u8 command){
  union i2c_smbus_data data;
  if (i2c_smbus_access(file,I2C_SMBUS_READ,command,I2C_SMBUS_WORD_DATA,&data))
    return -1;
  else
    return ((data.word<<8)&0xff00)|((data.word>>8)&0x00ff);
}

static inline int32_t
i2c_smbus_write_word_data(int file, __u8 command, uint16_t v){
  union i2c_smbus_data data;
  data.word = ((v<<8)&0xff00)|((v>>8)&0x00ff);
  return i2c_smbus_access(file,I2C_SMBUS_WRITE,command,I2C_SMBUS_WORD_DATA,&data);
}

/*********************************************************************/
// 

int
encode_chan(const std::string & s){
  if (s == "AB") return 0;
  if (s == "AD") return 1;
  if (s == "BD") return 2;
  if (s == "CD") return 3;
  if (s == "A") return 4;
  if (s == "B") return 5;
  if (s == "C") return 6;
  if (s == "D") return 7;
  throw Err() << "unknown channel setting: " << s;
}

const char *
decode_chan(int v){
  switch(v){
    case 0: return "AB";
    case 1: return "AD";
    case 2: return "BD";
    case 3: return "CD";
    case 4: return "A";
    case 5: return "B";
    case 6: return "C";
    case 7: return "D";
  }
  throw Err() << "unknown channel setting: " << v;
}

int
encode_range(const std::string & s){
  if (s == "6.144") return 0;
  if (s == "4.096") return 1;
  if (s == "2.048") return 2;
  if (s == "1.024") return 3;
  if (s == "0.512") return 4;
  if (s == "0.256") return 5;
  throw Err() << "unknown range setting: " << s;
}

double
range_to_num(const std::string & s){
  return atof(s.c_str());
}

const char *
decode_range(int v){
  switch(v){
    case 0: return "6.144";
    case 1: return "4.096";
    case 2: return "2.048";
    case 3: return "1.024";
    case 4: return "0.512";
    case 5:
    case 6:
    case 7: return "0.256";
  }
  throw Err() << "unknown range setting: " << v;
}

int
encode_rate(const std::string & s){
  if (s == "8") return 0;
  if (s == "16") return 1;
  if (s == "32") return 2;
  if (s == "64") return 3;
  if (s == "128") return 4;
  if (s == "250") return 5;
  if (s == "475") return 6;
  if (s == "860") return 7;
  throw Err() << "unknown rate setting: " << s;
}

double
rate_to_num(const std::string & s){
  return atof(s.c_str());
}

const char *
decode_rate(int v){
  switch(v){
    case 0: return "8";
    case 1: return "16";
    case 2: return "32";
    case 3: return "64";
    case 4: return "128";
    case 5: return "250";
    case 6: return "475";
    case 7: return "860";
  }
  throw Err() << "unknown rate setting: " << v;
}

const char *
decode_comp(int v){
  switch(v){
    case 0: return "after one";
    case 1: return "after two";
    case 2: return "after four";
    case 3: return "off";
  }
  throw Err() << "unknown comparator setting: " << v;
}


/*********************************************************************/
int
main(int argc, char *argv[]){
  try {

    /* default values */
    const char *dev="/dev/i2c-0"; // i2c bus
    uint8_t addr=0x48;            // default device address

    /* parse  options */
    while(1){
      opterr=0;
      int c = getopt(argc, argv, "hd:a:");
      if (c==-1) break;
      switch (c){
        case '?':
        case ':': throw Err() << "incorrect options, see -h"; /* error msg is printed by getopt*/
        case 'd': dev = optarg; break;
        case 'a': addr = atoi(optarg); break;
        case 'h':
          cout << "ads1115 -- SPP interface to ADS1113/1114/1115 ADC converters\n"
                  "Usage: ads1115 [options]\n"
                  "Options:\n"
                  " -d <dev>  -- I2C device path (default /dev/i2c-0)\n"
                  " -a <addr> -- I2C address (default 0x48)\n"
                  " -h        -- write this help message and exit\n";
          return 0;
      }
    }

    // open I2C interface
    int file = open(dev, O_RDWR);
    if (file < 0) throw Err()
      << "can't open i2c device " << dev << ": " << strerror(errno);

    // find device
    if (ioctl(file, I2C_SLAVE, addr) < 0) throw Err()
      << "can't use device address " << addr << ": " << strerror(errno);

    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cout << "ADS1113/1114/1115 device is opened. Type help to see command list.\n";
    cout << "#OK\n";

    while (1){
      try {
        auto args = read_words(cin);
        if (args.size()<1) break;

        do {
          // print help
          if (is_cmd(args, "help")){
            if (args.size()!=1) throw Err() << "Usage: help";
            cout <<
              "help  -- Get list of commands.\n"
              "get_time -- Get current time.\n"
              "*idn? -- Get ID string: \"ads1115 " VERSION "\".\n"
              "get_conf -- Get device configuration.\n"
              "get (A|B|C|D|AB|AD|BD|CD) <range> <rate> -- do a single measurement.\n"
              " Arguments:\n"
              " - Channels A..D, double letters for differential mode.\n"
              " - Range: 6.144, 4.096, 2.048, 1.024, 0.512, 0.256 [V],\n"
              "   bi-directional in differential mode.\n"
              " - Rate: 8,16,32,64,128,250,475,860 [1/s].\n"
            ;
            break;
          }

          // print time
          if (is_cmd(args, "get_time")){
            if (args.size()!=1) throw Err() << "Usage: get_time";
            struct timeval tv;
            gettimeofday(&tv, NULL);
            cout << tv.tv_sec << "." << setfill('0') << setw(6) << tv.tv_usec << "\n";
            break;
          }

          // print id
          if (is_cmd(args, "*idn?")){
            if (args.size()!=1) throw Err() << "Usage: *idn?";
            cout << "ads1115 " VERSION "\n";
            break;
          }

          // print device configuration
          if (is_cmd(args, "get_conf")){
            if (args.size()!=1) throw Err() << "Usage: get_conf";

            // conversion register
            int32_t data = i2c_smbus_read_word_data(file, REG_CONV);
            if (data<0) throw Err() << "can't read Conversion register";
            std::cout << "conversion register: 0x" << std::hex << data << "\n";

            // low threshold
            data = i2c_smbus_read_word_data(file, REG_LTHR);
            if (data<0) throw Err() << "can't read Lo_thresh register";
            std::cout << "lo_thresh register: 0x" << std::hex << data << "\n";

            // high threshold
            data = i2c_smbus_read_word_data(file, REG_HTHR);
            if (data<0) throw Err() << "can't read Hi_thresh register";
            std::cout << "hi_thresh register: 0x" << std::hex << data << "\n";

            // Config register
            data = i2c_smbus_read_word_data(file, REG_CONF);
            if (data<0) throw Err() << "can't read Config register";
            std::cout << "config register:    0x" << std::hex << data << "\n";
            std::cout << "status:     " << ((data>>15)? "ready":"busy") << "\n";
            std::cout << "chan:       " << decode_chan((data>>12)&7) << "\n";
            std::cout << "range:      " << decode_range((data>>9)&7) << " V\n";
            std::cout << "mode:       " << ((data>>8)? "single":"cont") << "\n";
            std::cout << "rate:       1/" << decode_rate((data>>5)&7) << " s\n";
            std::cout << "comp.type:  " << ((data>>4) ? "window":"trad") << "\n";
            std::cout << "comp.pol:   " << ((data>>3) ? "act.high":"act.low") << "\n";
            std::cout << "comp.latch: " << ((data>>2) ? "on":"off") << "\n";
            std::cout << "comp.queue: " << decode_comp(data&3) << "\n";

            break;
          }

          // do a measurement
          if (is_cmd(args, "get")){
            if (args.size()!=4) throw Err() << "Usage: get <chan> <range> <rate>";

            // default value with cleared mplex (bits 12-14), gain (9-11), rate (5-7)
            uint16_t conf=0b0000000100000011;

            conf |= 1 << 15; // start conversion
            conf |= encode_chan(args[1]) << 12;
            conf |= encode_range(args[2]) << 9;
            conf |= encode_rate(args[3]) << 5;

            int delay = 1000000/rate_to_num(args[3]);
            double scale = range_to_num(args[2]) / 32768.0;

            // write conf register
            if (i2c_smbus_write_word_data(file, REG_CONF, conf))
               throw Err() << "can't write Config register";

            // wait for conversion
            usleep(delay);
            while (1) {
              int data = i2c_smbus_read_word_data(file, 1);
              if (data>>15) break;
              usleep(100);
            }

            // read data
            int data = i2c_smbus_read_word_data(file, REG_CONV);
            std::cout << (int16_t)data * scale << "\n";

            break;
          }


        } while(0);
        cout << "#OK\n" << flush;
      }
      catch (Err E){ cout << "\n#Error: " << E.str() << "\n" << flush; }
    }

  }
  catch (Err E){
    cout << "#SPP001\n"; // a command-line protocol, version 001.
    cerr << "#Error: " << E.str() << "\n";
    return 1;
  }

}
