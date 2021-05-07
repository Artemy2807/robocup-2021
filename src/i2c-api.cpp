#include "i2c-api.hpp"

namespace i2c {
    I2C::I2C(const Device& device) {
        open(device);
    }

    bool I2C::open(const Device& device) {
        device_ = device;


        if((fd_ = ::open(device_.filename_, O_RDWR)) < 0) {
            LOG(ERROR) << "Can not access to the i2c adapter " << device_.filename_ << "\n"; 
            return false;
        }

        if(ioctl(fd_, I2C_SLAVE, device_.addr_) < 0) {
            LOG(ERROR) << "Can not opened the slave device " << std::hex << std::uppercase << device_.addr_ << "\n";
            return false;
        }
        
        LOG(NOTICE) << "Connected to the i2c slave device " << std::hex << std::uppercase << device_.addr_ << "\n";
        return true;
    }
    
    void I2C::close() {
        LOG(NOTICE) << "Close connection with i2c device " << std::hex << std::uppercase << device_.addr_ << "\n";
        ::close(fd_);
    }

    ssize_t I2C::read(void* receive, size_t size) {
        return ::read(fd_, receive, size);
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
