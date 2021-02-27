#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio/videoio_c.h>

#include <iostream>
#include <string>
#include <array>
#include <thread>
#include <mutex>

std::array<std::string, 3> video_array = {
    "/home/ksato/sandbox/multithread_cv/sample/test_1.mp4",
    "/home/ksato/sandbox/multithread_cv/sample/test_2.mp4",
    "/home/ksato/sandbox/multithread_cv/sample/test_3.mp4",
};

std::mutex x_mtx;

void streaming_test(int index){
    int ret = 0;
    int key = 0;

    cv::Mat frame;
    cv::VideoCapture video;
    video.open(video_array.at(index % 3));
    if(!video.isOpened()){
        std::cout << "ERROR: Can't open " << video_array.at(index) << std::endl;
        return;
    }

    std::string window_name = "STREAM" + std::to_string(index);

    while(1){
        ret = video.read(frame);
        if(!ret){
            video.set(CV_CAP_PROP_POS_FRAMES, 0);
            continue;
        }

        {
            std::lock_guard<std::mutex>   lock(x_mtx);
            cv::imshow(window_name, frame);
            key = cv::waitKey(1);
        }
        if(key == 'q')  break;
    }
    
    {
        std::lock_guard<std::mutex>   lock(x_mtx);
        cv::destroyAllWindows();
    }
    return;
}

int main(){
    std::thread test_th0([&]{ streaming_test(0); });
    std::thread test_th1([&]{ streaming_test(1); });

    test_th0.join();
    test_th1.join();
}
