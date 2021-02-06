#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "err/err.h"
#include <errno.h>
#include <cstring>
#include <unistd.h>

#include "i2c.h"

// I2C communication (see https://github.com/shenki/linux-i2c-example)

static inline void
i2c_access(int file, char read_write, uint8_t command,
                 int size, union i2c_smbus_data *data){
  struct i2c_smbus_ioctl_data args;
  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;
  if (ioctl(file,I2C_SMBUS,&args))
    throw Err() << "can't do " << (read_write? "read":"write")
                << " command: 0x" << std::hex << (int)command;
}

uint8_t
i2c_read_byte(int file, uint8_t command){
  union i2c_smbus_data data;
  i2c_access(file,I2C_SMBUS_READ,command,I2C_SMBUS_BYTE_DATA,&data);
  return (0x0FF & data.byte);
}

uint16_t
i2c_read_word(int file, uint8_t command){
  union i2c_smbus_data data;
  i2c_access(file,I2C_SMBUS_READ,command,I2C_SMBUS_WORD_DATA,&data);
  return ((data.word<<8)&0xff00)|((data.word>>8)&0x00ff);
}

void
i2c_write_byte(int file, uint8_t command, uint8_t v){
  union i2c_smbus_data data;
  data.byte = v;
  i2c_access(file,I2C_SMBUS_WRITE,command,I2C_SMBUS_BYTE_DATA,&data);
}

void
i2c_write_word(int file, uint8_t command, uint16_t v){
  union i2c_smbus_data data;
  data.word = ((v<<8)&0xff00)|((v>>8)&0x00ff);
  i2c_access(file,I2C_SMBUS_WRITE,command,I2C_SMBUS_WORD_DATA,&data);
}

// open I2C interface
int
i2c_open(const char *path, uint8_t addr){
  int fd = open(path, O_RDWR);
  if (fd < 0) throw Err()
    << "can't open i2c device " << path << ": " << strerror(errno);

  // use the address
  if (ioctl(fd, I2C_SLAVE, addr) < 0) throw Err()
    << "can't use device address " << addr << ": " << strerror(errno);

  return fd;
}

void
i2c_close(int fd){
  close(fd);
}
