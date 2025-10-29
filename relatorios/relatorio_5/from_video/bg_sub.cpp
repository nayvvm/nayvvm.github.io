/**
 * @file bg_sub.cpp
 * @brief Background subtraction tutorial sample code
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

// Updated parameters to use the standard OpenCV example video for better default
const char* params
    = "{ help h           |                       | Print usage }"
      "{ input            | movimentos_lentos.avi | Path to a video or a sequence of images }"
      "{ algo             | MOG2                  | Background subtraction method (KNN, MOG2) }";

int main(int argc, char* argv[])
{
    CommandLineParser parser(argc, argv, params);
    parser.about( "This program shows how to use background subtraction methods provided by "
                  " OpenCV. You can process both videos and images.\n" );
    if (parser.has("help"))
    {
        //print help information
        parser.printMessage();
        return 0; // Exit after printing help
    }
    if (!parser.check()) {
        parser.printErrors();
        return 0;
    }

    //! [create]
    //create Background Subtractor objects
    Ptr<BackgroundSubtractor> pBackSub;
    String algo = parser.get<String>("algo");
    if (algo == "MOG2")
        pBackSub = createBackgroundSubtractorMOG2();
    else
        pBackSub = createBackgroundSubtractorKNN();
        
    //! [create]

    //! [capture]
    String input_file = parser.get<String>("input");
    VideoCapture capture(input_file);
    if (!capture.isOpened()){
        //error in opening the video input
        cerr << "Unable to open: " << input_file << endl;
        return -1; // Use -1 for error exit
    }

    // Get frame dimensions and FPS
    int frame_width = static_cast<int>(capture.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(capture.get(CAP_PROP_FRAME_HEIGHT));
    double fps_double = capture.get(CAP_PROP_FPS);
    int fps = (fps_double > 0) ? cvRound(fps_double) : 20; // Use actual FPS or default to 20

    // define frame_size to be used in VideoWriter() argument
    Size frame_size(frame_width, frame_height);

    // Create VideoWriter to save the *foreground mask* as a color video
    // The foreground mask (fgMask) is a single-channel (grayscale) image.
    // We convert it to BGR (3-channel) before writing to an AVI file with MJPG.
    VideoWriter output("output_fgmask.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frame_size);

    if (!output.isOpened()) {
        cerr << "Could not open the output video file for write: output_fgmask.avi" << endl;
        return -1;
    }
    //! [capture]

    Mat frame, fgMask, bgrFgMask;
    int frame_count = 0;
    while (true) {
        capture >> frame;
        if (frame.empty())
            break;
        
        frame_count++; // Increment frame count

        //! [apply]
        //update the background model
        pBackSub->apply(frame, fgMask);
        //! [apply]

        //! [display_frame_number]
        //get the frame number and write it on the current frame
        // Use frame_count instead of CAP_PROP_POS_FRAMES which can be unreliable
        rectangle(frame, cv::Point(10, 2), cv::Point(120,20),
                  cv::Scalar(255,255,255), -1);
        stringstream ss;
        ss << frame_count;
        string frameNumberString = "Frame: " + ss.str();
        putText(frame, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
        //! [display_frame_number]

        cvtColor(fgMask, bgrFgMask, cv::COLOR_GRAY2BGR);
        output.write(bgrFgMask);

        imshow("Original Frame", frame);
        imshow("FG Mask", fgMask);
        
        //! [show_and_write]

        //get the input from the keyboard
        int keyboard = waitKey(1); // Use waitKey(1) for faster video processing
        if (keyboard == 'q' || keyboard == 27) // 'q' or ESC
            break;
    }

    // The VideoWriter and VideoCapture objects will automatically be released
    // when they go out of scope, but it's good practice to show this.
    capture.release();
    output.release();
    destroyAllWindows();

    return 0;
}
