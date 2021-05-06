#include "i2c-api.hpp"

namespace i2c {
    I2C::I2C(const Device& device) {
        open(device);
    }

    bool I2C::open(const Device& device) {
        dev_ = device;


        if((fd_ = ::open(device_.filename_, O_RDWR)) < 0) {
            LOG(ERROR) << "Can not access to the i2c adapter " << dev_.filename_ << "\n"; 
            return false;
        }

        if(ioctl(fd_, I2C_SLAVE, device_.address_) < 0) {
            LOG(ERROR) << "Can not opened the slave device " << std::hex << std::uppercase << dev_.addr_ << "\n";
            return false;
        }

        return true;
    }
    
    void I2C::close() {
        LOG(NOTICE) << "Close connection with i2c device " << std::hex << std::uppercase << dev_.addr_ << "\n";
        ::close(fd_);
    }

    ssize_t I2C::read(void* receive, size_t size) {
        ssize_t size_ = 0;

        if((size_ = ::read(fd_, receive, size)) < 0)
            LOG(WARNING) << "Can not read data from the slave device.\n";

        return size_;
    }

    ssize_t I2C::write(void* transmitter, size_t size) {
        ssize_t size_ = 0;

        if((size_ = ::write(fd_, transmitter, size)) < 0) {
            LOG(WARNING) << "Can not send data to the slave device.\n";
            close();
        }

        return size_;
    }
}
