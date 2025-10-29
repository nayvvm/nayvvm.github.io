import cv2
import numpy as np

print("Iniciando item (3)... Pressione 'q' para sair.")

cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Erro: Não foi possível abrir a webcam.")
    exit()

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Aplicar um leve blur para reduzir ruído
    filtered = cv2.GaussianBlur(frame, (5, 5), 0)
    
    # Converter para HSV
    hsv = cv2.cvtColor(filtered, cv2.COLOR_BGR2HSV)

    # --- DEFINIÇÃO DAS FAIXAS (AJUSTE ESTES VALORES!) ---

    # 1. Faixa Azul
    lower_blue = np.array([100, 150, 50])
    upper_blue = np.array([130, 255, 255])
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)

    # 2. Faixa Verde
    lower_green = np.array([40, 100, 50]) # H [40-80] é tipicamente verde
    upper_green = np.array([80, 255, 255])
    mask_green = cv2.inRange(hsv, lower_green, upper_green)

    # 3. Faixa Vermelha (requer duas máscaras)
    # Faixa 1 (0-10)
    lower_red1 = np.array([0, 150, 50])
    upper_red1 = np.array([10, 255, 255])
    mask_red1 = cv2.inRange(hsv, lower_red1, upper_red1)
    
    # Faixa 2 (170-180)
    lower_red2 = np.array([170, 150, 50])
    upper_red2 = np.array([180, 255, 255])
    mask_red2 = cv2.inRange(hsv, lower_red2, upper_red2)

    # Combinar as duas máscaras vermelhas
    mask_red = cv2.bitwise_or(mask_red1, mask_red2)

    # --- COMBINAR TODAS AS MÁSCARAS ---
    # Primeiro combina azul e verde
    mask_bg = cv2.bitwise_or(mask_blue, mask_green)
    # Depois adiciona o vermelho
    mask_total = cv2.bitwise_or(mask_bg, mask_red)

    # (3) "Extrair" os objetos coloridos da imagem original
    result = cv2.bitwise_and(frame, frame, mask=mask_total)

    # --- Janelas ---
    cv2.imshow("Original", frame)
    cv2.imshow("Mascara Total (RGB)", mask_total)
    cv2.imshow("Objetos Extraidos (3)", result)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

print("Encerrando...")
cap.release()
cv2.destroyAllWindows()
