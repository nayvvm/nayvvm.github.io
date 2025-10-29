#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

/**
 * @function drawHistogram
 * @brief Desenha o histograma de uma imagem em tons de cinza.
 */
Mat drawHistogram(Mat& gray_image) {
    int histSize = 256; // 256 bins
    float range[] = { 0, 256 }; // O range (não inclusivo)
    const float* histRange = { range };
    bool uniform = true, accumulate = false;
    Mat hist;

    // Calcula o histograma
    calcHist(&gray_image, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

    int hist_w = 512, hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);

    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));

    // Normaliza o resultado para [ 0, histImage.rows ]
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());

    // Desenha o histograma
    for (int i = 1; i < histSize; i++) {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
            Scalar(255, 255, 255), 2, 8, 0);
    }

    return histImage;
}

int main() {
    // Carregue sua imagem (substitua pelo caminho correto)
    Mat src = imread("captura_original_3.png");
    if (src.empty()) {
        cout << "Erro ao carregar a imagem" << endl;
        return -1;
    }

    // 1. Converter para tons de cinza
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // 2. Calcular histograma ANTES da equalização
    Mat hist_before = drawHistogram(gray);

    // 3. Realizar a equalização do histograma
    Mat equalized;
    equalizeHist(gray, equalized);

    // 4. Calcular histograma DEPOIS da equalização
    Mat hist_after = drawHistogram(equalized);

    // 5. Mostrar resultados
    imshow("Imagem em Cinza", gray);
    imshow("Imagem Equalizada", equalized);
    imshow("Histograma (Antes)", hist_before);
    imshow("Histograma (Depois)", hist_after);

    cout << "Pressione qualquer tecla para salvar as imagens e sair..." << endl;
    int k = waitKey(0); // Espera por uma tecla

    // 6. Salvar imagens
    if (k != -1) { // Se qualquer tecla for pressionada
        imwrite("output_cinza.png", gray);
        imwrite("output_equalizada.png", equalized);
        imwrite("output_hist_antes.png", hist_before);
        imwrite("output_hist_depois.png", hist_after);
        cout << "Imagens salvas!" << endl;
    }

    return 0;
}
