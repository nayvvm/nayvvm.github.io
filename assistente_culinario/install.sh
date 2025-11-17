#!/bin/bash

echo "-----------------------------------------"
echo " Assistente de Instalação Culinária"
echo "-----------------------------------------"
echo "Vou precisar da sua senha (sudo) para instalar"
echo "as dependências do sistema. (OpenCV, SFML, FFmpeg, etc.)"
echo ""

# Pede sudo logo no início
sudo echo "Permissão de administrador OK. Iniciando instalação..."

if [ $? -ne 0 ]; then
    echo "Falha ao obter permissão sudo. Saindo."
    exit 1
fi

# 1. Dependências do APT (Compilação e Execução)
sudo apt update
sudo apt install -y build-essential pkg-config libopencv-dev libsfml-dev \
                    libopenal-dev zenity ffmpeg curl

echo ""
echo "-----------------------------------------"
echo " Instalando yt-dlp (Baixador de vídeos)..."
echo "-----------------------------------------"

# 2. Instala o yt-dlp (binário global)
sudo curl -L https://github.com/yt-dlp/yt-dlp/releases/latest/download/yt-dlp -o /usr/local/bin/yt-dlp
sudo chmod a+rx /usr/local/bin/yt-dlp

echo ""
echo "-----------------------------------------"
echo "🎉 Instalação Concluída! 🎉"
echo "-----------------------------------------"
echo "Agora, execute o arquivo 'run.sh' para iniciar o programa."
echo ""
