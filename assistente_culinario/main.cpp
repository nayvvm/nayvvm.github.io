#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <cstdlib> 
#include <cstdio> 
#include <memory>
#include <stdexcept>
#include <array>
#include <cmath> // Para std::abs

// OpenCV e SFML
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <SFML/Audio.hpp> 

// --- CONFIGURAÇÕES ---
const std::string ARQUIVO_VIDEO = "temp_video.mp4";
const std::string ARQUIVO_AUDIO = "temp_audio.ogg";
const std::string TITULO_WEBCAM = "Assistente Culinario (Webcam)";
const std::string TITULO_RECEITA = "Assistente Culinario (Receita)";

// Enum e Constantes
enum Gesture { NONE, ADVANCE, REWIND, HAND_STILL };
const cv::Rect ROI_GESTO(30, 80, 280, 280); 
const int PAUSE_CONFIRMATION_FRAMES = 5; // Frames de confirmação (para mão parada)
const int SWIPE_THRESHOLD = 50;

// --- Variáveis Globais ---
bool is_paused = false;
int gesture_cooldown = 0;
int last_x = 0;
int pause_persistence_counter = 0; // Reutilizado para o gesto de "mão parada"
int g_total_frames = 0;
double g_fps = 0;

// Variáveis para "Mão Parada"
int last_stable_x = 0;
int last_stable_y = 0;
const int STILLNESS_THRESHOLD = 15; // Quão "parada" a mão precisa estar (em pixels)

// --- FUNÇÕES AUXILIARES ---

std::string exec_cmd(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() falhou!");
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

bool prepararMidia() {
    std::cout << "--- INICIANDO FRONTEND ---" << std::endl;
    std::string url;
    try {
        url = exec_cmd("zenity --entry --title='Assistente Culinario' --text='Cole a URL do YouTube:' --width=400");
    } catch (...) {}
    if (url.empty()) return false;

    std::cout << "--- BAIXANDO E CONVERTENDO (Max 480p) ---" << std::endl;
    
    // Forçando 480p (height<=480) para performance máxima
    std::string cmd_download = "yt-dlp -f \"bestvideo[height<=480][ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best\" --force-overwrites -o \"" + ARQUIVO_VIDEO + "\" \"" + url + "\"";
    
    if (std::system(cmd_download.c_str()) != 0) {
        exec_cmd("zenity --error --text='Erro ao baixar video.'");
        return false;
    }

    std::string cmd_convert = "ffmpeg -i \"" + ARQUIVO_VIDEO + "\" -vn -acodec libvorbis -y \"" + ARQUIVO_AUDIO + "\" -loglevel quiet";
    if (std::system(cmd_convert.c_str()) != 0) {
        exec_cmd("zenity --error --text='Erro ao converter audio.'");
        return false;
    }
    return true;
}

// --- VISÃO COMPUTACIONAL ---
int encontrarMao(const cv::Mat& mask, std::vector<cv::Point>& contour) {
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    if (contours.empty()) return -1;
    double max_area = 0;
    int max_area_idx = -1;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = cv::contourArea(contours[i]);
        if (area > max_area) { max_area = area; max_area_idx = i; }
    }
    if (max_area < 5000) return -1; // Ignora ruído
    contour = contours[max_area_idx];
    return max_area_idx;
}

// Função de Gesto Otimizada 
Gesture processarGestos(cv::Mat& frame, const std::vector<cv::Point>& hand_contour) {
    
    if (hand_contour.empty()) {
        last_stable_x = 0; // Mão sumiu, reseta
        last_x = 0;
        return Gesture::NONE;
    }
    
    cv::Moments m = cv::moments(hand_contour);
    if (m.m00 == 0) return Gesture::NONE; // Evita divisão por zero
    
    int cX = (int)(m.m10 / m.m00); // Centro X da mão
    int cY = (int)(m.m01 / m.m00); // Centro Y da mão

    // 1. Gesto de Deslizar (Swipe)
    if (last_x != 0) {
        int delta_x = cX - last_x;
        last_x = 0; // Reseta swipe
        
        if (delta_x > SWIPE_THRESHOLD) { 
            last_stable_x = 0; // Reseta "parado"
            return Gesture::ADVANCE; 
        }
        if (delta_x < -SWIPE_THRESHOLD) { 
            last_stable_x = 0; // Reseta "parado"
            return Gesture::REWIND; 
        }
    }
    last_x = cX;

    // 2. Gesto de Pausa (Mão Parada)
    if (last_stable_x == 0) { // É a primeira vez que vemos a mão?
        last_stable_x = cX; 
        last_stable_y = cY;
        return Gesture::NONE;
    }

    // Calcula o quanto a mão se moveu do último ponto "estável"
    int delta_still_x = std::abs(cX - last_stable_x);
    int delta_still_y = std::abs(cY - last_stable_y);

    if (delta_still_x > STILLNESS_THRESHOLD || delta_still_y > STILLNESS_THRESHOLD) {
        // Mão se moveu demais, não está parada. Reseta o ponto estável.
        last_stable_x = cX; 
        last_stable_y = cY;
        return Gesture::NONE;
    } 
    
    // Se chegou aqui, a mão está parada
    return Gesture::HAND_STILL;
}

std::string formatTime(int total_seconds) {
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;
    return ss.str();
}

// --- MAIN ---
int main(int argc, char** argv) {
    
    if (!prepararMidia()) return -1;

    cv::VideoCapture cap_webcam(0);
    cap_webcam.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap_webcam.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    cv::VideoCapture cap_receita(ARQUIVO_VIDEO); 
    sf::Music music_receita;

    if (!music_receita.openFromFile(ARQUIVO_AUDIO) || !cap_webcam.isOpened() || !cap_receita.isOpened()) {
        std::cerr << "Erro critico na midia." << std::endl;
        return -1;
    }

    cv::namedWindow(TITULO_WEBCAM, cv::WINDOW_AUTOSIZE);
    cv::namedWindow(TITULO_RECEITA, cv::WINDOW_AUTOSIZE);

    g_fps = cap_receita.get(cv::CAP_PROP_FPS); 
    g_total_frames = cap_receita.get(cv::CAP_PROP_FRAME_COUNT);
    if (g_fps <= 0) g_fps = 30.0;
    if (g_total_frames <= 0) g_total_frames = 10000; 
    int jump_frames_slow = 30 * g_fps; 

    cv::Mat frame_webcam, frame_receita, hsv_frame, skin_mask;
    frame_receita = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);

    int gesture_skip_counter = 0;
    const int GESTURE_SKIP_RATE = 2; // Analisa gestos 1 a cada 3 frames

    music_receita.play();

    while (true) {
        // 1. WEBCAM (Roda sempre)
        cap_webcam >> frame_webcam;
        if (frame_webcam.empty()) break;
        cv::flip(frame_webcam, frame_webcam, 1);
        cv::rectangle(frame_webcam, ROI_GESTO, cv::Scalar(255, 0, 0), 2);

        // 2. SINCRONIA A/V (Roda sempre)
        if (!is_paused) {
            double audio_time = music_receita.getPlayingOffset().asSeconds();
            double video_time = cap_receita.get(cv::CAP_PROP_POS_FRAMES) / g_fps;
            double diff = audio_time - video_time;

            if (diff > 0.4) { 
                cap_receita.set(cv::CAP_PROP_POS_FRAMES, audio_time * g_fps);
                cap_receita.read(frame_receita);
            } 
            else if (diff > 0.03) { 
                cap_receita.read(frame_receita); 
            }
        }
        if (!frame_receita.empty() && !is_paused) {
             if (cap_receita.get(cv::CAP_PROP_POS_FRAMES) >= g_total_frames - 5) {
                music_receita.setPlayingOffset(sf::Time::Zero);
                cap_receita.set(cv::CAP_PROP_POS_FRAMES, 0);
             }
        }

        // 3. GESTOS (Roda interacalado)
        gesture_skip_counter++;
        
        if (gesture_skip_counter > GESTURE_SKIP_RATE) {
            gesture_skip_counter = 0; // Reseta o contador do "skip"
            
            // Inicializa o gesto detectado *dentro* deste bloco
            Gesture detected_gesture = Gesture::NONE; 
            
            cv::Mat roi = frame_webcam(ROI_GESTO);
            cv::cvtColor(roi, hsv_frame, cv::COLOR_BGR2HSV);
            cv::inRange(hsv_frame, cv::Scalar(0, 48, 80), cv::Scalar(20, 255, 255), skin_mask);
            cv::erode(skin_mask, skin_mask, cv::Mat(), cv::Point(-1, -1), 1);
            cv::dilate(skin_mask, skin_mask, cv::Mat(), cv::Point(-1, -1), 1);

            std::vector<cv::Point> hand_contour;
            if (encontrarMao(skin_mask, hand_contour) != -1) {
                // Mão encontrada
                cv::drawContours(roi, std::vector<std::vector<cv::Point>>{hand_contour}, -1, cv::Scalar(0, 255, 0), 2);
                if (gesture_cooldown == 0) {
                    detected_gesture = processarGestos(roi, hand_contour);
                }
            } else {
                // Mão sumiu, reseta tudo
                last_x = 0; 
                pause_persistence_counter = 0;
                last_stable_x = 0;
            }

            // AÇÃO DOS GESTOS 
            if (gesture_cooldown == 0 && detected_gesture != Gesture::NONE) {
                double current_pos = cap_receita.get(cv::CAP_PROP_POS_FRAMES);
                bool seek = false;
                std::string msg = "";

                switch (detected_gesture) {
                    case Gesture::ADVANCE:
                        current_pos += jump_frames_slow; msg = "AVANCAR"; seek = true; 
                        pause_persistence_counter = 0; // Reseta "parado"
                        break;
                    case Gesture::REWIND:
                        current_pos -= jump_frames_slow; msg = "VOLTAR"; seek = true; 
                        pause_persistence_counter = 0; // Reseta "parado"
                        break;
                    
                    case Gesture::HAND_STILL:
                        pause_persistence_counter++; // Incrementa
                        if (pause_persistence_counter >= PAUSE_CONFIRMATION_FRAMES) {
                            is_paused = !is_paused;
                            if (is_paused) music_receita.pause(); else music_receita.play();
                            gesture_cooldown = 45; 
                            pause_persistence_counter = 0; 
                            last_x = 0;
                            last_stable_x = 0; 
                        }
                        break;
                    default: break;
                }

                if (seek) {
                    current_pos = std::max(0.0, std::min(current_pos, (double)g_total_frames - 1));
                    cap_receita.set(cv::CAP_PROP_POS_FRAMES, current_pos);
                    music_receita.setPlayingOffset(sf::seconds(current_pos / g_fps));
                    cv::putText(frame_webcam, msg, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 255), 2);
                    gesture_cooldown = 30; 
                }
            }
            
            // RESET DO CONTADOR 
            // Se o gesto detectado foi "NENHUM" (mão se movendo), reseta o contador
            if (detected_gesture == Gesture::NONE) {
                pause_persistence_counter = 0;
            }
        }
        
        // 4. DISPLAY (Roda sempre)
        
        // Feedback visual de Pausa
        if (is_paused) cv::putText(frame_webcam, "PAUSADO", cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
        
        // Decrementa o Cooldown
        if (gesture_cooldown > 0) gesture_cooldown--;

        // Mostra o vídeo da Receita
        if (!frame_receita.empty()) {
             int s_atual = (int)(cap_receita.get(cv::CAP_PROP_POS_FRAMES) / g_fps);
             int s_total = (int)(g_total_frames / g_fps);
             std::string time_str = formatTime(s_atual) + " / " + formatTime(s_total);
             cv::putText(frame_receita, time_str, cv::Point(30, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0,0,255), 2);
             cv::imshow(TITULO_RECEITA, frame_receita);
        }
        
        // Mostra a Webcam
        cv::imshow(TITULO_WEBCAM, frame_webcam);

        // Saída
        if ((cv::waitKey(1) & 0xFF) == 27) break;
    }

    cap_webcam.release(); cap_receita.release(); music_receita.stop();
    cv::destroyAllWindows();
    std::remove(ARQUIVO_VIDEO.c_str()); 
    std::remove(ARQUIVO_AUDIO.c_str());
    return 0;
}
