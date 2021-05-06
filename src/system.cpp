#include "system.hpp"

System::System(cv::Size frame_size, cv::Rect sign_area, uint32_t server_port) {
    this->frame_size.write(frame_size);
    
    this->sign_area.write(sign_area);

    this->server_port.write(server_port);
    
    this->device_id.write(0);
}
