#pragma once
#include <linux/i2c-dev.h> 
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include "logger.hpp"

namespace i2c {

    struct Device {
        int n_i2c_,
            addr_;
        char filename_[20];

        Device(int n_i2c = 1, int addr = 0x08) :
            n_i2c_(n_i2c),
            addr_(addr)
        {
            snprintf(filename_, 19, "/dev/i2c-%d", n_i2c_);
        }
    };

    class I2C {
    public:
        I2C() = default;
        I2C(const Device& device);

        bool open(const Device& device);
        void close();

        ssize_t read(void* receive, size_t size);
        ssize_t write(void* transmitter, size_t size);
        
    private:
        int fd_;
        Device device_;
        
    };

} // namespace i2c
