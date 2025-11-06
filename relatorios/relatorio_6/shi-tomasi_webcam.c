#include <iostream>
#include <vector>
#include <string>

// Headers principais do OpenCV
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/videoio.hpp> 

// Use os namespaces
using namespace cv;
using namespace std;

// --- Funções de Detecção (Idênticas ao código anterior) ---

void detectarShiTomasi_Webcam(Mat& img, Mat& imgGray)
{
    vector<Point2f> cantos;
    int maxCantos = 150;
    double qualityLevel = 0.01;
    double minDistance = 10;
    int blockSize = 3;
    goodFeaturesToTrack(imgGray, cantos, maxCantos, qualityLevel, minDistance, Mat(), blockSize, false);
    for (size_t i = 0; i < cantos.size(); i++)
    {
        circle(img, cantos[i], 4, Scalar(0, 255, 0), 2, LINE_AA);
    }
}

void detectarORB_Webcam(Mat& img, Mat& imgGray, Ptr<ORB>& orbDetector)
{
    vector<KeyPoint> keypoints;
    orbDetector->detect(imgGray, keypoints);
    drawKeypoints(img, keypoints, img, Scalar(0, 0, 255), DrawMatchesFlags::DEFAULT);
}


// --- Função Principal ---

int main()
{
    // --- 1. Abrir a Webcam ---
    VideoCapture cap(0); 

    if (!cap.isOpened())
    {
        cout << "Erro: Nao foi possivel abrir a webcam." << endl;
        return -1;
    }

    // --- Instruções Atualizadas ---
    cout << "Webcam aberta com sucesso." << endl;
    cout << "Pressione '1' para Shi-Tomasi." << endl;
    cout << "Pressione '2' para ORB." << endl;
    cout << "-----------------------------------" << endl;
    cout << "Pressione 's' para Salvar Snapshot (foto)." << endl;
    cout << "Pressione 'h' para Iniciar Gravacao." << endl;
    cout << "Pressione 'k' para Parar Gravacao." << endl;
    cout << "-----------------------------------" << endl;
    cout << "Pressione 'q' ou ESC para sair." << endl;

    // --- 2. Inicializar Recursos ---
    Ptr<ORB> orb = ORB::create(500);
    char modo = '1';
    Mat frame, frameCinza;

    // --- NOVAS Variáveis para Gravação e Snapshot ---
    VideoWriter videoWriter;
    bool isRecording = false;
    int snapshotCounter = 0;
    int videoCounter = 0;
    Size frameSize; // Vamos pegar o tamanho do frame quando a gravação começar
    double fpsGravacao = 20.0; // FPS desejado para o vídeo salvo

    // --- 3. Loop de Captura e Processamento ---
    while (true)
    {
        cap.read(frame);
        if (frame.empty())
        {
            cout << "Erro: Frame vazio." << endl;
            break;
        }

        // Converte para escala de cinza ANTES de desenhar
        cvtColor(frame, frameCinza, COLOR_BGR2GRAY);

        string textoModo;

        // --- 4. Aplicar Detector ---
        if (modo == '1')
        {
            detectarShiTomasi_Webcam(frame, frameCinza);
            textoModo = "Modo: [1] Shi-Tomasi";
        }
        else if (modo == '2')
        {
            detectarORB_Webcam(frame, frameCinza, orb);
            textoModo = "Modo: [2] ORB";
        }

        // --- 5. Exibir Informações na Tela ---
        putText(frame, textoModo, Point(10, 25), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 2);
        putText(frame, "Pressione 'q' para sair", Point(10, 50), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(255, 255, 0), 2);

        // --- NOVO: Indicador Visual de Gravação ---
        if (isRecording)
        {
            // Pisca um círculo vermelho no canto superior direito
            // (int)(getTickCount() / getTickFrequency()) % 2 calcula (segundos % 2)
            bool blink = ((int)(getTickCount() / getTickFrequency()) % 2) == 0;
            if (blink)
            {
                circle(frame, Point(frame.cols - 30, 30), 10, Scalar(0, 0, 255), -1); // -1 = preenchido
            }
            putText(frame, "GRAVANDO", Point(frame.cols - 130, 35), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2);
        }

        // --- 6. Mostrar o Resultado ---
        imshow("Webcam - Lab 6 (Experimento 2)", frame);

        // --- 7. Aguardar Interação do Usuário ---
        int key = waitKey(1);
        if (key == 'q' || key == 27)
        {
            break; // Sai do loop principal
        }
        else if (key == '1') { modo = '1'; }
        else if (key == '2') { modo = '2'; }

        // --- NOVOS Comandos de Tecla ---

        else if (key == 's') // 's' para Salvar Snapshot
        {
            string filename = "snapshot_" + to_string(snapshotCounter) + ".png";
            imwrite(filename, frame);
            cout << "Snapshot salvo: " << filename << endl;
            snapshotCounter++;
        }
        else if (key == 'h') // 'h' para Iniciar Gravação (Start)
        {
            if (!isRecording)
            {
                frameSize = frame.size();
                string videoFilename = "gravacao_" + to_string(videoCounter) + ".avi";
                
                // Abre o VideoWriter. Usamos 'MJPG' como codec para .avi
                videoWriter.open(videoFilename, VideoWriter::fourcc('M', 'J', 'P', 'G'), fpsGravacao, frameSize, true);

                if (videoWriter.isOpened())
                {
                    isRecording = true;
                    videoCounter++;
                    cout << "Gravacao iniciada: " << videoFilename << endl;
                }
                else
                {
                    cout << "Erro: Nao foi possivel iniciar a gravacao." << endl;
                }
            }
            else
            {
                cout << "A gravacao ja esta em andamento." << endl;
            }
        }
        else if (key == 'k') // 'k' para Parar Gravação
        {
            if (isRecording)
            {
                isRecording = false;
                videoWriter.release();
                cout << "Gravacao parada. Arquivo salvo." << endl;
            }
        }

        // --- NOVO: Escrever frame no vídeo se estiver gravando ---
        if (isRecording)
        {
            videoWriter.write(frame);
        }
    }

    // --- 8. Limpeza ---
    // Garante que o vídeo seja salvo se o usuário sair (com 'q') enquanto grava
    if (isRecording)
    {
        videoWriter.release();
        cout << "Gravacao parada (programa finalizado)." << endl;
    }
    
    cap.release();
    destroyAllWindows();

    return 0;
}
