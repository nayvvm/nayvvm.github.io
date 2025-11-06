#include <iostream>
#include <vector>

// Headers principais do OpenCV
#include <opencv2/core.hpp>       // Estruturas de dados básicas (Mat)
#include <opencv2/highgui.hpp>    // Funções de UI (imread, imshow, waitKey)
#include <opencv2/imgproc.hpp>    // Processamento de imagem (cvtColor, circle)
#include <opencv2/features2d.hpp> // Detectores de features (ORB, drawKeypoints)

// Use os namespaces para facilitar
using namespace cv;
using namespace std;

// Protótipos das funções que vamos criar
void detectarShiTomasi(Mat& img, Mat& imgGray);
void detectarORB(Mat& img, Mat& imgGray);

int main()
{
    // --- 1. Carregar a Imagem ---
    // !!! ATENÇÃO: Troque o arquivo jpg pelo caminho da sua imagem !!!
    string nomeArquivo = "messi5.jpg"; 
    Mat imagem = imread(nomeArquivo, IMREAD_COLOR);

    if (imagem.empty())
    {
        cout << "Erro: Nao foi possivel carregar a imagem: " << nomeArquivo << endl;
        return -1;
    }

    // --- 2. Preparar Imagem ---
    // Converter para escala de cinza, que é necessária para a maioria dos detectores
    Mat imagemCinza;
    cvtColor(imagem, imagemCinza, COLOR_BGR2GRAY);

    // --- 3. Executar Experimentos ---
    
    // (A) Shi-Tomasi Corner Detector
    // Criamos uma cópia da imagem original para desenhar nela
    Mat imagemShiTomasi = imagem.clone();
    detectarShiTomasi(imagemShiTomasi, imagemCinza);

    // (B) Feature Detector (ORB)
    // Criamos outra cópia para desenhar o resultado do ORB
    Mat imagemORB = imagem.clone();
    detectarORB(imagemORB, imagemCinza);

    // --- 4. Exibir Resultados ---
    imshow("Resultado (A) Shi-Tomasi", imagemShiTomasi);
    imshow("Resultado (B) ORB", imagemORB);

    cout << "Resultados salvos como 'resultado_shi-tomasi.jpg' e 'resultado_orb.jpg'" << endl;
    cout << "Pressione qualquer tecla para fechar as janelas..." << endl;

    // Espera o usuário pressionar uma tecla
    waitKey(0); 

    return 0;
}

/**
 * @brief Aplica o detector Shi-Tomasi (Good Features to Track)
 * Tutorial (A): https://docs.opencv.org/4.x/d8/dd8/tutorial_good_features_to_track.html
 */
void detectarShiTomasi(Mat& img, Mat& imgGray)
{
    // Vetor para armazenar os cantos detectados (pontos 2D)
    vector<Point2f> cantos;

    // Parâmetros do detector
    int maxCantos = 100;        // Número máximo de cantos a detectar
    double qualityLevel = 0.01; // Nível de qualidade (0-1). Valores maiores = cantos "melhores"
    double minDistance = 10;    // Distância euclidiana mínima entre os cantos detectados
    Mat mask;                   // Máscara (não usada aqui)
    int blockSize = 3;          // Tamanho do bloco para calcular a matriz de covariância
    bool useHarrisDetector = false; // false = usa Shi-Tomasi, true = usa Harris
    double k = 0.04;            // Parâmetro livre do detector Harris (ignorado se useHarrisDetector=false)

    // Chama a função principal
    goodFeaturesToTrack(imgGray,         // Imagem de entrada (escala de cinza)
                        cantos,          // Vetor de saída para os cantos
                        maxCantos,       // Parâmetros
                        qualityLevel,
                        minDistance,
                        mask,
                        blockSize,
                        useHarrisDetector,
                        k);

    cout << "(A) Shi-Tomasi: Detectados " << cantos.size() << " cantos." << endl;

    // --- Desenhar os cantos na imagem ---
    int raio = 4;
    for (size_t i = 0; i < cantos.size(); i++)
    {
        // Desenha um círculo em cada canto detectado
        circle(img,                 // Imagem onde desenhar (colorida)
               cantos[i],          // Posição (centro) do círculo
               raio,               // Raio do círculo
               Scalar(0, 255, 0),  // Cor (Verde)
               2,                  // Espessura
               LINE_AA);           // Linha com anti-aliasing
    }

    // --- Salvar o resultado ---
    imwrite("resultado_shi-tomasi.jpg", img);
}

/**
 * @brief Aplica o detector ORB (Oriented FAST and Rotated BRIEF)
 * Tutorial (B): https://docs.opencv.org/4.x/d7/d66/tutorial_feature_detection.html
 */
void detectarORB(Mat& img, Mat& imgGray)
{
    // Vetor para armazenar os "keypoints"
    // KeyPoint armazena mais que um Ponto (posição, tamanho, ângulo, etc.)
    vector<KeyPoint> keypoints;

    // 1. Inicializar o detector ORB
    // Podemos passar parâmetros, como o número de features desejadas
    int nFeatures = 500; // Número máximo de features a encontrar
    Ptr<ORB> orb = ORB::create(nFeatures);

    // 2. Detectar os keypoints
    // (O ORB também pode calcular "descritores", mas o tutorial (B) foca na detecção)
    orb->detect(imgGray, keypoints);

    cout << "(B) ORB: Detectados " << keypoints.size() << " keypoints." << endl;
    
    // --- Desenhar os keypoints na imagem ---
    Mat imgComKeypoints;
    drawKeypoints(img,                     // Imagem original (colorida)
                  keypoints,               // Vetor de keypoints
                  imgComKeypoints,         // Imagem de saída
                  Scalar(0, 0, 255),       // Cor (Vermelho)
                  DrawMatchesFlags::DEFAULT); // Flags de desenho

    // Como drawKeypoints cria uma nova imagem de saída (imgComKeypoints),
    // copiamos ela de volta para 'img' para que a função main possa exibi-la.
    imgComKeypoints.copyTo(img);

    // --- Salvar o resultado ---
    imwrite("resultado_orb.jpg", img);
}
