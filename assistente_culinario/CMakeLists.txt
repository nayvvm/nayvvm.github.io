cmake_minimum_required(VERSION 3.10)
project(AssistenteCulinario)

# Encontra o pacote OpenCV
# O 'REQUIRED' garante que ele pare se o OpenCV não for encontrado
find_package(OpenCV REQUIRED)

# Adiciona o diretório de includes do OpenCV
include_directories(${OpenCV_INCLUDE_DIRS})

# Define o nome do executável final e quais arquivos .cpp ele usa
add_executable(assistente_culinario main.cpp)

# Linka o executável com as bibliotecas do OpenCV
target_link_libraries(assistente_culinario ${OpenCV_LIBRARIES})
