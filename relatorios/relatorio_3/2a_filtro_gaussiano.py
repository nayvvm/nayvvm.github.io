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

# Cria uma janela para os trackbars de controle HSV
cv2.namedWindow("Trackbars")
cv2.createTrackbar("HUE Min", "Trackbars", 0, 179, nada)
cv2.createTrackbar("HUE Max", "Trackbars", 179, 179, nada)
cv2.createTrackbar("SAT Min", "Trackbars", 0, 255, nada)
cv2.createTrackbar("SAT Max", "Trackbars", 255, 255, nada)
cv2.createTrackbar("VALUE Min", "Trackbars", 0, 255, nada)
cv2.createTrackbar("VALUE Max", "Trackbars", 255, 255, nada)

print("\nAnálise do Filtro Gaussiano (Item 2.a)")
print("Pressione 'q' para sair.")

while True:
    # Captura um frame da webcam
    ret, frame = cap.read()
    if not ret:
        print("Não foi possível receber o frame. Encerrando...")
        break

    # (2.a) Aplica o filtro Gaussiano ao frame original (BGR)
    # O filtro é aplicado antes da conversão de cor para evitar artefatos no canal HUE [4]
    frame_filtrado = cv2.GaussianBlur(frame, (5, 5), 0)

    # Converte o frame filtrado de BGR para HSV
    hsv = cv2.cvtColor(frame_filtrado, cv2.COLOR_BGR2HSV)

    # Obtém os valores atuais dos trackbars
    h_min = cv2.getTrackbarPos("HUE Min", "Trackbars")
    h_max = cv2.getTrackbarPos("HUE Max", "Trackbars")
    s_min = cv2.getTrackbarPos("SAT Min", "Trackbars")
    s_max = cv2.getTrackbarPos("SAT Max", "Trackbars")
    v_min = cv2.getTrackbarPos("VALUE Min", "Trackbars")
    v_max = cv2.getTrackbarPos("VALUE Max", "Trackbars")

    # Define os limites inferior e superior da cor no espaço HSV
    lower_range = np.array([h_min, s_min, v_min])
    upper_range = np.array([h_max, s_max, v_max])

    # Cria uma máscara binária com os pixels que estão dentro da faixa de cor [5, 6]
    mask = cv2.inRange(hsv, lower_range, upper_range)

    # Aplica a máscara ao frame original para isolar os objetos coloridos
    resultado = cv2.bitwise_and(frame, frame, mask=mask)

    # (2.a) Cria a janela adicional para mostrar a imagem não-filtrada e a filtrada lado a lado
    # np.hstack empilha as imagens horizontalmente
    comparacao_filtros = np.hstack((frame, frame_filtrado))

    # Exibe as janelas
    cv2.imshow("Original vs. Gaussiano", comparacao_filtros)
    cv2.imshow("Mascara de Cor", mask)
    cv2.imshow("Resultado da Deteccao", resultado)

    # Aguarda a tecla 'q' ser pressionada para sair do loop
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libera os recursos
cap.release()
cv2.destroyAllWindows()

