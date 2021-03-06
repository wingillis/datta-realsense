																																																																																																																					//saving to disk

#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API

#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>


bool save_frame_raw_data(const std::string& filename, rs2::frame frame)
{
    bool ret = false;
    auto image = frame.as<rs2::video_frame>();
    if (image)
    {
        std::ofstream outfile(filename.data(), std::ofstream::binary | std::ofstream::app); //the std::ofstream::app is key as it allows me to append!
        outfile.write(static_cast<const char*>(image.get_data()), image.get_height()*image.get_stride_in_bytes()); //as you can see I do not close the file. This is not good practice. I need to add a signal function that closes the file when I hit ctrl-C AND then terminates the program (maybe even with a message)	
        ret = true;
    }

    return ret;
}


// Capture Example demonstrates how to
// capture depth and color video streams and render them to the screen
int main(int argc, char * argv[]) try
{
    rs2::colorizer color_map;
    
    auto depth_units = rs2::context().query_devices().front()
        .query_sensors().front().get_option(RS2_OPTION_DEPTH_UNITS);
    
    rs2::pipeline pipe;
    rs2::config myconfig; //I am declaring the configuration object
    myconfig.enable_stream(RS2_STREAM_DEPTH, 848, 480, RS2_FORMAT_Z16, 90); //This is the configuration that I want. I adding this stream to my configuration. I chose UYVY format because I heard frames might drop on linux otherwise
    pipe.start(myconfig);

    while(true) // Application still alive?
    {
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
	rs2::frame depth = data.get_depth_frame(); // Find and colorize the depth data
        save_frame_raw_data("depth.bin", depth);
    }

    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
