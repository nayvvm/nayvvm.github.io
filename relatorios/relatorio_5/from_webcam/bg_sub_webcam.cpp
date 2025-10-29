/**
 * @file bg_sub.cpp
 * @brief Background subtraction tutorial sample code (Webcam version)
 * @author Domenico D. Bloisi (Modified by Gemini)
 */

#include <iostream>
#include <sstream>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

// Modified parameters to prioritize the webcam
const char* params
    = "{ help h           |                       | Print usage }"
      // Changed default input to 0 (webcam index)
      "{ input            | 0                     | Index of the camera or Path to a video file }"
      "{ algo             | MOG2                  | Background subtraction method (KNN, MOG2) }";

int main(int argc, char* argv[])
{
    CommandLineParser parser(argc, argv, params);
    parser.about( "This program shows how to use background subtraction methods provided by "
                  " OpenCV. It prioritizes webcam input.\n" );
    if (parser.has("help"))
    {
        parser.printMessage();
        return 0;
    }
    if (!parser.check()) {
        parser.printErrors();
        return 0;
    }

    // --- Background Subtractor Initialization ---
    Ptr<BackgroundSubtractor> pBackSub;
    String algo = parser.get<String>("algo");
    if (algo == "MOG2")
        pBackSub = createBackgroundSubtractorMOG2();
    else if (algo == "KNN")
        pBackSub = createBackgroundSubtractorKNN();
    else {
        cerr << "Invalid background subtraction algorithm specified: " << algo << ". Using MOG2." << endl;
        pBackSub = createBackgroundSubtractorMOG2();
    }

    // --- Webcam Capture Setup ---
    String input_str = parser.get<String>("input");
    VideoCapture capture;
    
    // Check if the input is a number (webcam index) or a file path
    if (isdigit(input_str[0])) {
        // Input is likely a device index (e.g., 0, 1)
        int camera_index = stoi(input_str);
        capture.open(camera_index);
        cout << "Attempting to open webcam with index: " << camera_index << endl;
    } else {
        // Input is likely a file path
        capture.open(input_str);
        cout << "Attempting to open video file: " << input_str << endl;
    }
    
    if (!capture.isOpened()){
        cerr << "Unable to open video source: " << input_str << endl;
        // Return a specific error code for better debugging
        return -1; 
    }

    // --- Video Writer Setup (Optional but kept for continuity) ---
    // Get frame dimensions and FPS from the capture source
    int frame_width = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT));
    double fps_double = capture.get(CAP_PROP_FPS);
    // Webcam FPS can be 0, so default to 30 for smoother recording
    int fps = (fps_double > 0) ? cvRound(fps_double) : 30; 
    
    Size frame_size(frame_width, frame_height);

    // Create VideoWriter to save the foreground mask
    VideoWriter output("webcam_output_fgmask.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frame_size);

    if (!output.isOpened()) {
        cerr << "Could not open the output video file for write: webcam_output_fgmask.avi" << endl;
        // Continue without writing output if it fails, or exit
    }

    // --- Main Processing Loop ---
    Mat frame, fgMask, bgrFgMask;
    int frame_count = 0;
    while (true) {
        capture >> frame;
        if (frame.empty())
            break;
        
        frame_count++;

        // Apply background subtraction
        pBackSub->apply(frame, fgMask);

        // Display frame count on the original frame
        rectangle(frame, cv::Point(10, 2), cv::Point(120,20),
                  cv::Scalar(255,255,255), -1);
        stringstream ss;
        ss << frame_count;
        string frameNumberString = "Frame: " + ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));

        // Convert the single-channel fgMask to a 3-channel BGR image for writing
        cvtColor(fgMask, bgrFgMask, cv::COLOR_GRAY2BGR);
        
        // Write the converted BGR foreground mask to the output video
        if (output.isOpened()) {
            output.write(bgrFgMask);
        }

        imshow("Webcam Feed", frame);
        imshow("Foreground Mask", fgMask);
        
        // Get the input from the keyboard (Wait 1ms)
        int keyboard = waitKey(1); 
        if (keyboard == 'q' || keyboard == 27) // 'q' or ESC
            break;
    }

    capture.release();
    output.release();
    destroyAllWindows();

    return 0;
}
