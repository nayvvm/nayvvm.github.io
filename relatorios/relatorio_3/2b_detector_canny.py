import cv2
import numpy as np

# Função de callback vazia para os trackbars
def nada(x):
    pass

# Inicializa a captura de vídeo da webcam
cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Erro: Não foi possível abrir a câmera.")
    exit()

# Cria uma janela para os trackbars de controle do Canny
cv2.namedWindow("Trackbars Canny")
cv2.createTrackbar("Threshold 1", "Trackbars Canny", 50, 255, nada)
cv2.createTrackbar("Threshold 2", "Trackbars Canny", 150, 255, nada)

print("\nAnálise do Detector Canny (Item 2.b)")
print("Pressione 'q' para sair.")

while True:
    # Captura um frame da webcam
    ret, frame = cap.read()
    if not ret:
        print("Não foi possível receber o frame. Encerrando...")
        break

    # Aplica o filtro Gaussiano ao frame original (BGR)
    frame_filtrado = cv2.GaussianBlur(frame, (5, 5), 0)

    # (2.b) Converte o frame filtrado para escala de cinza para usar no detector Canny
    # O Canny opera em imagens de um único canal (intensidade) [7]
    gray_filtrado = cv2.cvtColor(frame_filtrado, cv2.COLOR_BGR2GRAY)

    # Obtém os valores dos limiares (thresholds) dos trackbars
    t1 = cv2.getTrackbarPos("Threshold 1", "Trackbars Canny")
    t2 = cv2.getTrackbarPos("Threshold 2", "Trackbars Canny")

    # (2.b) Aplica o detector de bordas Canny
    canny_edges = cv2.Canny(gray_filtrado, t1, t2)

    # Para visualização, converte a imagem Canny (1 canal) para BGR (3 canais)
    # para que possa ser empilhada com a imagem filtrada colorida
    canny_edges_bgr = cv2.cvtColor(canny_edges, cv2.COLOR_GRAY2BGR)

    # (2.b) Cria a janela adicional para mostrar a imagem filtrada e a do detector Canny
    comparacao_canny = np.hstack((frame_filtrado, canny_edges_bgr))

    # Exibe as janelas
    cv2.imshow("Original", frame)
    cv2.imshow("Filtrado vs. Canny", comparacao_canny)

    # Aguarda a tecla 'q' ser pressionada para sair do loop
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libera os recursos
cap.release()
cv2.destroyAllWindows()

