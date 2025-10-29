#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap(0); // Abre a webcam padrão (índice 0)
    if (!cap.isOpened()) {
        cout << "Erro ao abrir a webcam" << endl;
        return -1;
    }

    Mat frame, gray, equalized;

    while (true) {
        cap >> frame; // Captura um novo frame
        if (frame.empty()) {
            break; // Sai do loop se o frame estiver vazio
        }

        // Converter para tons de cinza
        cvtColor(frame, gray, COLOR_BGR2GRAY);

        // Realizar a equalização
        equalizeHist(gray, equalized);

        // Mostrar janelas ao vivo
        imshow("Webcam - Cinza", gray);
        imshow("Webcam - Equalizada", equalized);

        // Espera 1ms pela tecla 'ESC' (código 27) para sair
        if (waitKey(1) == 27) {
            break;
        }
    }

    cap.release(); // Libera a webcam
    destroyAllWindows(); // Fecha todas as janelas
    return 0;
}
