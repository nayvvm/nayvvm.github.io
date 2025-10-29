import cv2
import numpy as np
import time

# Inicializa a captura de vídeo da webcam
cap = cv2.VideoCapture(0)
if not cap.isOpened():
    print("Erro: Não foi possível abrir a câmera.")
    exit()

# --- Configurações para Gravação de Vídeo ---
# Obtém as dimensões do frame dinamicamente para evitar erros [13]
frame_width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
frame_height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
fps = 20.0

# Define o codec FourCC para o vídeo [11, 12]
fourcc = cv2.VideoWriter_fourcc(*'XVID') # 'XVID' é um codec comum para.avi no Windows

# Inicializa os objetos VideoWriter como None
video_writer_original = None
video_writer_comparacao = None

# Variáveis de estado
recording = False
img_counter = 0

print("\nFuncionalidades de Mídia (Item 2.c)")
print("Pressione [s] para salvar uma imagem.")
print("Pressione [k] para INICIAR a gravação.")
print("Pressione [h] para PARAR a gravação.")
print("Pressione [q] para sair.")

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # --- Pipeline de Processamento (igual ao item 2.b) ---
    frame_filtrado = cv2.GaussianBlur(frame, (5, 5), 0)
    gray_filtrado = cv2.cvtColor(frame_filtrado, cv2.COLOR_BGR2GRAY)
    canny_edges = cv2.Canny(gray_filtrado, 50, 150)
    canny_edges_bgr = cv2.cvtColor(canny_edges, cv2.COLOR_GRAY2BGR)
    comparacao_canny = np.hstack((frame_filtrado, canny_edges_bgr))

    # Exibe as janelas
    cv2.imshow("Original", frame)
    cv2.imshow("Filtrado vs. Canny", comparacao_canny)

    # Captura a tecla pressionada
    key = cv2.waitKey(1) & 0xFF

    # --- Lógica de Controle ---
    if key == ord('q'):
        break
    
    # (2.c) Salvar imagem ao pressionar 's'
    elif key == ord('s'):
        # Gera um nome de arquivo único usando um contador
        img_name_orig = f"captura_original_{img_counter}.png"
        img_name_comp = f"captura_comparacao_{img_counter}.png"
        
        # Salva as imagens das janelas [10, 14]
        cv2.imwrite(img_name_orig, frame)
        cv2.imwrite(img_name_comp, comparacao_canny)
        
        print(f"Imagens salvas: {img_name_orig} e {img_name_comp}")
        img_counter += 1

    # (2.c) Iniciar gravação ao pressionar 'k'
    elif key == ord('k'):
        if not recording:
            recording = True
            # Gera nomes de arquivo únicos com timestamp
            timestamp = time.strftime("%Y%m%d-%H%M%S")
            video_name_orig = f"video_original_{timestamp}.avi"
            video_name_comp = f"video_comparacao_{timestamp}.avi"
            
            # Cria os objetos VideoWriter
            # O tamanho para a janela de comparação é (largura * 2, altura)
            video_writer_original = cv2.VideoWriter(video_name_orig, fourcc, fps, (frame_width, frame_height))
            video_writer_comparacao = cv2.VideoWriter(video_name_comp, fourcc, fps, (frame_width * 2, frame_height))
            
            print(f"Iniciando gravação... Salvando em {video_name_orig} e {video_name_comp}")

    # (2.c) Parar gravação ao pressionar 'h'
    elif key == ord('h'):
        if recording:
            recording = False
            # Libera os objetos de gravação
            if video_writer_original:
                video_writer_original.release()
                video_writer_original = None
            if video_writer_comparacao:
                video_writer_comparacao.release()
                video_writer_comparacao = None
            print("Gravação encerrada.")

    # Se a gravação estiver ativa, escreve os frames no arquivo
    if recording:
        if video_writer_original:
            video_writer_original.write(frame)
        if video_writer_comparacao:
            video_writer_comparacao.write(comparacao_canny)

# Libera os recursos
# Garante que os gravadores de vídeo sejam liberados se o loop for interrompido durante a gravação
if recording:
    if video_writer_original:
        video_writer_original.release()
    if video_writer_comparacao:
        video_writer_comparacao.release()

cap.release()
cv2.destroyAllWindows()

