// Include Libraries
#include<opencv2/opencv.hpp>
#include<iostream>

// Namespace to nullify use of cv::function(); syntax
using namespace std;
using namespace cv;

int BEST_KERNEL_LENGTH = 7;
int WORST_KERNEL_LENGTH = 3;

Mat src; Mat dst;
char window_name_best[] = "Gaussian Blur Demo";
char window_name_worst[] = "Bilinear filter Demo";

int main()
{
    // initialize a video capture object, arg 0 means webcamera. 
    //To use stream from more cameras, just pass 1,2 and so on..
	VideoCapture vid_capture(0);

    // Acquire frame width and height with the help of get() method
    // You can replace 3 and 4 with CAP_PROP_FRAME_WIDTH and CAP_PROP_FRAME_HEIGHT
    // They are just enumerations
	int frame_width = static_cast<int>(vid_capture.get(3));
	int frame_height = static_cast<int>(vid_capture.get(4));

	//define frame_size to be used in VideoWriter() argument
    Size frame_size(frame_width, frame_height);
	int fps = 20;
    
    // Initialize video writer object
    //VideoWriter output("output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), fps, frame_size);
	
    while (vid_capture.isOpened())
    {
        // Initialize frame matrix to store frames
        Mat frame;
        Mat blurredFrame;
        Mat bilinearFrame;

        //Initialize a boolean to check whether frames are present or not
        bool isSuccess = vid_capture.read(frame);

        // If frames are not there, close it
        if (isSuccess == false)
        {
            cout << "Web camera is disconnected" << endl;
            break;
        }
        // If frames are present
        if(isSuccess == true)
        {
            blur( frame, blurredFrame, Size( WORST_KERNEL_LENGTH, WORST_KERNEL_LENGTH ), Point(-1,-1) );
            imshow(window_name_worst, blurredFrame);
            
            bilateralFilter ( frame, bilinearFrame, BEST_KERNEL_LENGTH, BEST_KERNEL_LENGTH*2, BEST_KERNEL_LENGTH/2 );
            imshow(window_name_best, bilinearFrame);
            

            // wait for 20 ms between successive frames and break the loop if key q is pressed
            int key = waitKey(20);
            if (key == 's')
            {
                imwrite("saved_frame_blur.jpg", blurredFrame);
                imwrite("saved_frame_bilinear.jpg", bilinearFrame);
            }
            if (key == 'q')
            {
                cout << "Key q is pressed by the user. Stopping the video" << endl;
                break;
            }
        }

    }
   destroyAllWindows();
   vid_capture.release();
   return 0;
}
