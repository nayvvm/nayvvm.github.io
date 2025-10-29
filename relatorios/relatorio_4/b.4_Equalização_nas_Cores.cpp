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

    Mat frame;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        // 1. Separar os canais de cor (B, G, R)
        vector<Mat> channels;
        split(frame, channels);
        
        // 'channels[0]' é Blue
        // 'channels[1]' é Green
        // 'channels[2]' é Red

        // 2. Equalizar cada canal separadamente
        equalizeHist(channels[0], channels[0]);
        equalizeHist(channels[1], channels[1]);
        equalizeHist(channels[2], channels[2]);

        // 3. Juntar os canais de volta em uma imagem colorida
        Mat equalized_color;
        merge(channels, equalized_color);

        // 4. Mostrar resultados
        imshow("Webcam - Original", frame);
        imshow("Webcam - Cor Equalizada (RGB)", equalized_color);

        if (waitKey(1) == 27) break;
    }

    cap.release();
    destroyAllWindows();
    return 0;
}
