#ifndef MY_I2C_H
#define MY_I2C_H

int i2c_open(const char *path, uint8_t addr);
void i2c_close(int fd);

uint8_t  i2c_read_byte(int file, uint8_t command);
uint16_t i2c_read_word(int file, uint8_t command);
void     i2c_write_byte(int file, uint8_t command, uint8_t v);
void     i2c_write_word(int file, uint8_t command, uint16_t v);

#endif