# Como Executar o Assistente de Culinária Interativo

Este guia descreve os passos necessários para compilar e executar o projeto em um ambiente Linux (Ubuntu/Debian).

### 1\. Download do Código Fonte

1.  Acesse o repositório do projeto: [https://github.com/nayvvm/nayvvm.github.io](https://github.com/nayvvm/nayvvm.github.io).
2.  Clique no botão verde **Code** e selecione **Download ZIP**.
3.  Vá até a sua pasta de `Downloads` e extraia o arquivo `.zip` baixado.

### 2\. Instalação das Dependências

Abra o terminal (`Ctrl+Alt+T`) e execute os comandos abaixo, um bloco por vez, para instalar as bibliotecas necessárias (OpenCV, SFML, FFmpeg, etc.):

**Passo 2.1: Atualizar o sistema e instalar bibliotecas C++**

```bash
sudo apt update
sudo apt install build-essential pkg-config libopencv-dev libsfml-dev libopenal-dev zenity ffmpeg curl
```

**Passo 2.2: Instalar o gerenciador de downloads do YouTube (yt-dlp)**
*Para evitar conflitos com o Python do sistema, utilizaremos o `pipx`.*

```bash
sudo apt install pipx
pipx ensurepath
pipx install yt-dlp
```

*(Dica: Se o terminal pedir, feche-o e abra novamente após o comando `pipx ensurepath` para que as alterações tenham efeito).*

### 3\. Compilação do Projeto

Navegue até a pasta onde o código foi extraído e compile o programa.

**Passo 3.1: Entrar na pasta do projeto**
*(Este comando assume que você extraiu o ZIP na pasta Downloads)*

```bash
cd ~/Downloads/nayvvm.github.io-main/assistente_culinario
```

**Passo 3.2: Compilar o código**
Copie e cole este comando inteiro para gerar o executável:

```bash
g++ main.cpp -o assistente_culinario $(pkg-config --cflags --libs opencv4) -lsfml-audio -lsfml-system
```

### 4\. Execução

Com o programa compilado, inicie o assistente:

```bash
./assistente_culinario
```

-----

**Observações:**

  * Certifique-se de que a webcam está conectada antes de iniciar.
  * Ao rodar o programa, uma janela se abrirá pedindo a URL do vídeo do YouTube.
  * Para fechar o programa, pressione a tecla `ESC` ou `q` com a janela da câmera selecionada.
