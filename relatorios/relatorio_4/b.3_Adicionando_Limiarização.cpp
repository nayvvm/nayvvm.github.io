#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap(0);
    if (!cap.isOpened()) {
        cout << "Erro ao abrir a webcam" << endl;
        return -1;
    }

    Mat frame, gray, equalized;
    Mat binary_sem_eq, binary_com_eq;
    
    // Valor do limiar (threshold), pode ser ajustado
    double thresh_value = 128.0; 
    double max_value = 255.0;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cvtColor(frame, gray, COLOR_BGR2GRAY);
        equalizeHist(gray, equalized);

        // Binarização SEM equalização (baseada na img 'gray')
        threshold(gray, binary_sem_eq, thresh_value, max_value, THRESH_BINARY);

        // Binarização COM equalização (baseada na img 'equalized')
        threshold(equalized, binary_com_eq, thresh_value, max_value, THRESH_BINARY);

        // Mostrar resultados
        imshow("Webcam - Equalizada", equalized);
        imshow("Binaria (Sem EQ)", binary_sem_eq);
        imshow("Binaria (Com EQ)", binary_com_eq);

        if (waitKey(1) == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
