#!/bin/bash

echo "-----------------------------------------"
echo " Compilando o Assistente Culinário..."
echo "-----------------------------------------"

# Comando de compilação
g++ main.cpp -o assistente_final \
    $(pkg-config --cflags --libs opencv4) \
    $(pkg-config --cflags --libs sfml-audio sfml-system)

# Verifica se a compilação falhou
if [ $? -ne 0 ]; then
    echo ""
    echo "❌ ERRO: A compilação falhou!"
    echo "Por favor, rode o 'install.sh' primeiro."
    read -p "Pressione Enter para sair..."
    exit 1
fi

echo "Compilação OK."
echo ""
echo "🚀 Iniciando o programa!"
echo "-----------------------------------------"

# Executa o programa
./assistente_final

echo "-----------------------------------------"
echo "Programa finalizado."
