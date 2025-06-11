# Labirinto das Sombras

"Labirinto das Sombras" é um jogo dinâmico desenvolvido em C++ utilizando a biblioteca Raylib, focado em demonstrar os princípios da Programação Orientada a Objetos (POO). O jogo desafia o jogador a navegar por labirintos progressivamente mais complexos, desviando de obstáculos móveis e armadilhas, coletando moedas e utilizando power-ups estrategicamente para alcançar a saída antes que o tempo se esgote.

## Funcionalidades Principais

* **Labirintos Dinâmicos**: O jogo apresenta labirintos com paredes móveis e elementos que aparecem e desaparecem, com a complexidade e velocidade dos obstáculos aumentando a cada nível.
* **Coleta de Moedas**: Moedas estão espalhadas pelas fases; coletá-las é essencial para a pontuação e para habilitar o objetivo final (a saída do labirinto). Cada moeda coletada adiciona 100 pontos à sua pontuação total.
* **Power-ups Estratégicos**: Itens coletáveis que concedem vantagens temporárias ao jogador, adicionando dinamismo às partidas.
    * **Power-up de Velocidade**: Aumenta a velocidade base do jogador em 50% por 4 segundos. A funcionalidade é gerenciada na classe `Player` (variáveis `baseSpeed` e `currentSpeed`) e na classe `PowerUp` (variável `speedBoostTimer`).
    * **Power-up de Proteção (Escudo)**: Concede 5 segundos de invulnerabilidade contra paredes, ideal para atravessar obstáculos sem perder vidas. A duração é controlada pela variável `shieldTimer` na classe `Player`.
    * **Power-up de Vida**: Recupera uma vida, até um máximo de três.
* **Obstáculos (Paredes)**: Principais desafios do jogo, exigindo agilidade para evitar colisões.
    * **Paredes Comuns**: Blocos escuros e móveis. Colidir com elas resulta em perda de vida, a menos que o jogador tenha invencibilidade ou escudo.
    * **Paredes Especiais**: Paredes roxas e pulsantes, mais rápidas e perigosas, podendo causar dano e ter movimentos mais complexos.
    * A dinâmica das paredes é gerenciada principalmente pela classe `Wall` (base para todas as paredes, definindo posição, tamanho e velocidade) e `ParedeEspecial` (herda de `Wall` e sobrescreve o desenho para um efeito visual pulsante).
* **Sistema de Vidas**: O jogador começa com três vidas (`Player::lives`). Colisões sem proteção resultam na perda de uma vida.
* **Sistema de Pontuação**: Mede o progresso do jogador.
    * Cada moeda coletada vale 100 pontos.
    * Ao terminar um nível, é acrescido um bônus de 5.000 pontos.
* **Temporizador**: Um fator crucial para a vitória. O jogo possui um tempo total limite (180 segundos para todas as fases) que não é renovado a cada nível. Se o tempo se esgota, o jogo é encerrado com "Game Over".
* **Arquitetura Orientada a Objetos**: O projeto foi pensado sob os princípios da Programação Orientada a Objetos, resultando em um código modular, de fácil leitura e adaptável. Classes como `Player`, `Wall`, `Coin`, `PowerUp` e `Objetivo` possuem responsabilidades e comportamentos bem definidos.

## Telas do Jogo

O jogo possui diversas telas para guiar a experiência do usuário:

* **Tela de Menu Principal**: Ponto de entrada do jogo, exibindo o título "LABIRINTO DAS SOMBRAS" e opções "INICIAR JOGO" e "SAIR DO JOGO".
* **Cena do Jogo (Fase)**: Exibe o jogador (quadrado azul), paredes móveis, moedas e o HUD com status de vidas, tempo e pontuação.
* **Tela de Transição de Nível**: Mostrada ao completar uma fase, indicando "LEVEL COMPLETO!" e qual a próxima fase a ser iniciada.
* **Tela de Game Over**: Exibida quando o tempo esgota ou as vidas chegam a zero, mostrando "FIM DE JOGO" e a pontuação final.
* **Tela de Vitória**: Apresentada ao completar as três fases propostas, exibindo "YOU WIN!" e a pontuação total.

## Estrutura do Projeto

O projeto "Labirinto das Sombras" é organizado em classes que representam os diferentes elementos e a lógica do jogo.

### Classes Principais:

* **`Entidade`**: Classe base abstrata para todos os objetos interativos, definindo propriedades comuns como a posição (`Rectangle`) e métodos fundamentais (`Update()`, `Draw()`).
* **`Player`**: Representa o personagem controlável pelo jogador. Gerencia movimento via teclado (WASD ou setas), vidas (`lives`), invencibilidade (`invincibilityTimer`), escudo (`shieldTimer`) e velocidade (`currentSpeed`).
* **`Wall`**: Representa os obstáculos básicos do labirinto. Define posição, tamanho e velocidade inicial, e movimenta a parede invertendo sua direção ao colidir com as bordas da tela.
* **`ParedeEspecial`**: Herda de `Wall`, introduzindo um tipo de obstáculo que causa dano ao contato. Destaca-se por um efeito visual pulsante e translúcido.
* **`Coin`**: Representa os itens coletáveis. Ao colidir com o jogador, o método `TryCollect()` marca a moeda como inativa (`active = false`), concede pontos e reproduz um som. O principal propósito das moedas é habilitar a saída do labirinto.
* **`PowerUp`**: Oferece bônus temporários ao jogador. Utiliza uma enumeração (`PowerUpType`) para diferenciar entre `LIFE`, `SHIELD` e `SPEED`. O método `TryCollect()` é chamado quando o jogador colide e coleta um power-up.
* **`Objetivo`**: Marca o ponto de saída do labirinto. Começa desabilitado (`enabled = false`) e só é ativado quando todas as moedas da fase são coletadas.
* **`ParticleSystem`**: Gerencia a emissão e atualização de partículas para efeitos visuais dinâmicos, como explosões ao coletar itens ou impactos de colisões.
* **`Button`**: Facilita a interação com a interface do usuário para elementos de menu e pause.
* **`Game`**: A classe central que orquestra o jogo. Gerencia os diferentes estados do jogo (MENU, PLAYING, PAUSED, LEVEL_TRANSITION, VICTORY, GAMEOVER), coordena a atualização e o desenho de todas as entidades, e lida com a detecção de colisões e o ciclo de vida do jogo.

## Desenvolvimento

O jogo foi desenvolvido como um projeto acadêmico do Instituto Federal de Educação, Ciência e Tecnologia de São Paulo (IFSP) – Câmpus Campos do Jordão, para a disciplina de Programação Orientada a Objetos.

* **Linguagem**: C++.
* **Biblioteca Gráfica**: Raylib.
* **Ambiente de Desenvolvimento Integrado (IDE)**: CLion.
* **Sistema de Controle de Versão**: Git, integrado com repositórios no GitHub (um público para publicação e outro privado para gerenciar o código-fonte e assets).

## Como Compilar e Executar

Para compilar e executar o projeto, você precisará de um ambiente de desenvolvimento C++ configurado com a biblioteca Raylib. As configurações `.vscode` (`c_cpp_properties.json`, `launch.json`, `settings.json`, `tasks.json`) fornecidas podem auxiliar na configuração do ambiente no VS Code.

### Pré-requisitos:

* Compilador C++ (ex: g++)
* Raylib (instalada e configurada)
* CMake (opcional, para gerenciamento de projeto)

