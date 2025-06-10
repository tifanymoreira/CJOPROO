// --> Projeto: Labirinto das Sombras
// --> Autor: Tifany Luiza
// --> Matéria: PROGRAMAÇÃO ORIENTADA a OBJETOS
// --> Curso: ANÁLISE E DESENVOLVIMENTO DE SISTEMAS
// ==============================================================================

// SOBRE O GAME:
// "Labirinto das Sombras" é um jogo de labirinto dinâmico onde o jogador
// deve atravessar fases progressivamente mais difíceis. O objetivo é desviar
// de paredes móveis e evanescentes, coletar moedas e power-ups, e alcançar
// a saída antes que o tempo se esgote. A cada nível, a complexidade do
// labirinto e a velocidade dos obstáculos aumentam, testando a agilidade e
// o raciocínio estratégico do jogador.
//
// SISTEMA DE PONTUAÇÃO E POWER-UPS:
// - Coletar uma moeda: +100 pontos
// - Coletar um power-up: Oferece vantagens como invencibilidade (pode ultrapassar
//   paredes sem perder uma vida), aumento de velocidade e vida.
// - Completar uma fase: +5000 pontos de bônus
// - Tempo: O jogo possui um tempo total limite que não se renova a cada fase.
//
// O jogo foi desenvolvido utilizando a biblioteca Raylib, com foco em
// princípios de Programação Orientada a Objetos.

// =============================================================================
//                                 INCLUDES
// =============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

#include "raylib.h"
#include "raymath.h"

// =============================================================================
//                          FUNÇÕES DE PONTE PARA RAYMATH
// =============================================================================

// Essas funções garantem que as chamadas a Vector2Add e Vector2Scale de raymath.h
// sejam resolvidas corretamente pelo compilador, especialmente em cenários
// de compilação complexos. Este método foi optado pois o compilador não estava reconhecendo
// as funções de raymath.h diretamente, resultando em erros de linkagem. Desta forma, após pesquisas
// e tentativas, esta foi a única solução encontrada.

static inline Vector2 MyVector2Add(Vector2 v1, Vector2 v2) {
    return Vector2Add(v1, v2);
}

static inline Vector2 MyVector2Scale(Vector2 v, float scale) {
    return Vector2Scale(v, scale);
}

// =============================================================================
//                            CONFIGURAÇÕES GLOBAIS
// =============================================================================
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 960;

// =============================================================================
//                             DEFINIÇÃO DE CORES
// =============================================================================

#define BACKGROUND_DEEPEST_DARK CLITERAL(Color){ 0, 0, 0, 255 }
#define BACKGROUND_SUBTLE_DARK CLITERAL(Color){ 10, 5, 15, 255 }
#define BACKGROUND_FOG_START CLITERAL(Color){ 20, 10, 30, 0 }
#define BACKGROUND_FOG_END CLITERAL(Color){ 10, 5, 15, 150 }

#define PLAYER_COLOR CLITERAL(Color){ 100, 180, 255, 255 }
#define PLAYER_INVINCIBLE_COLOR CLITERAL(Color){ 255, 100, 255, 150 }

#define WALL_COLOR CLITERAL(Color){ 20, 20, 25, 255 }
#define WALL_BORDER_COLOR CLITERAL(Color){ 5, 5, 10, 255 }

#define SPECIAL_WALL_COLOR CLITERAL(Color){ 200, 0, 200, 255 }
#define SPECIAL_WALL_BORDER_COLOR CLITERAL(Color){ 255, 50, 255, 255 }

#define GOAL_COLOR CLITERAL(Color){ 80, 255, 150, 255 }
#define GOAL_INNER_COLOR CLITERAL(Color){ 20, 50, 40, 255 }
#define GOAL_DISABLED_COLOR CLITERAL(Color){ 30, 30, 35, 100 }

#define COIN_COLOR CLITERAL(Color){ 255, 220, 50, 255 }
#define COIN_GLOW_COLOR CLITERAL(Color){ 255, 255, 150, 150 }

#define LIFE_POWERUP_COLOR CLITERAL(Color){ 255, 50, 50, 255 }
#define SHIELD_POWERUP_COLOR CLITERAL(Color){ 50, 200, 255, 255 }
#define SPEED_POWERUP_COLOR CLITERAL(Color){ 0, 255, 255, 255 }

// Cores da Interface do Usuário
#define UI_TEXT_COLOR CLITERAL(Color){ 150, 150, 150, 255 }
#define UI_HIGHLIGHT_COLOR CLITERAL(Color){ 240, 240, 240, 255 }
#define UI_ACCENT_COLOR CLITERAL(Color){ 100, 180, 255, 255 }
#define UI_WARNING_COLOR CLITERAL(Color){ 255, 60, 60, 255 }

// =============================================================================
//                               ENUMERAÇÕES DO JOGO
// =============================================================================
// Controla o fluxo do jogo entre diferentes telas e lógicas.
enum class GameState { MENU, PLAYING, PAUSED, LEVEL_TRANSITION, VICTORY, GAMEOVER };

// Define os diferentes bônus que o jogador pode coletar.
enum class PowerUpType { LIFE, SHIELD, SPEED };

// =============================================================================
//                             CLASSES DE ENTIDADES
// =============================================================================

// Classe base abstrata para todos os elementos interativos no jogo.
// Garante que todas as entidades possuam uma representação retangular e
// métodos de atualização e desenho.
class Entidade {
public:
    Rectangle rect;
    virtual ~Entidade() = default;
    virtual void Update(float delta) {}
    virtual void Draw() const { DrawRectangleRec(rect, GRAY); }
};

// Classe do Jogador
class Player : public Entidade {
public:
    int lives;
    float invincibilityTimer;
    float shieldTimer;
    float speedBoostTimer;
    float baseSpeed;
    float currentSpeed;

    // Construtor: Inicializa a velocidade base e reseta o estado do jogador
    Player() {
        baseSpeed = 250.0f;
        Reset();
    }

    // Reseta o jogador para o estado inicial da fase
    void Reset() {
        lives = 3;
        invincibilityTimer = 0.0f;
        shieldTimer = 0.0f;
        speedBoostTimer = 0.0f;
        currentSpeed = baseSpeed;
        // Posição inicial do jogador no canto esquerdo da tela
        rect = { (float)SCREEN_WIDTH / 20, (float)SCREEN_HEIGHT / 2.0f - 15, 30, 30 };
    }

    // Atualiza a lógica do jogador a cada frame.
    void Update(float delta) override {
        // Decrementa os timers de efeitos.
        if (invincibilityTimer > 0) invincibilityTimer -= delta;
        if (shieldTimer > 0) shieldTimer -= delta;
        if (speedBoostTimer > 0) {
            speedBoostTimer -= delta;
            if (speedBoostTimer <= 0) {
                currentSpeed = baseSpeed;
            }
        }

        // Processa a entrada do teclado para movimento.
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) rect.y -= currentSpeed * delta;
        if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) rect.y += currentSpeed * delta;
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) rect.x -= currentSpeed * delta;
        if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) rect.x += currentSpeed * delta;

        // Garante que o jogador permaneça dentro dos limites da tela.
        if (rect.x < 0) rect.x = 0;
        if (rect.x + rect.width > SCREEN_WIDTH) rect.x = SCREEN_WIDTH - rect.width;
        if (rect.y < 0) rect.y = 0;
        if (rect.y + rect.height > SCREEN_HEIGHT) rect.y = SCREEN_HEIGHT - rect.height;
    }

    // Desenha o jogador na tela
    void Draw() const override {
        // Efeito de invencibilidade: alteração de cor e borda pulsante do player.
        if (invincibilityTimer > 0) {
            float blinkAlpha = (sinf(GetTime() * 30.0f) + 1.0f) / 2.0f;
            DrawRectangleRec(rect, Fade(PLAYER_INVINCIBLE_COLOR, 0.5f + blinkAlpha * 0.5f));
            DrawRectangleLinesEx(rect, 3, Fade(UI_WARNING_COLOR, blinkAlpha * 1.0f));
        } else {
            DrawRectangleRec(rect, PLAYER_COLOR);
            DrawRectangleLinesEx(rect, 2, WALL_BORDER_COLOR);
        }

        // Desenha um círculo de escudo se o power-up estiver ativo
        if (shieldTimer > 0) {
            Color shieldEffectColor = Fade(SHIELD_POWERUP_COLOR, 0.5f + 0.3f * sinf(GetTime() * 15));
            DrawCircle((int)(rect.x + rect.width / 2), (int)(rect.y + rect.height / 2), 35.0f, shieldEffectColor);
        }
    }

    // Aplica lógica de dano ao jogador.
    void TakeDamage() {
        // Só leva dano se não estiver invencível ou com escudo
        if (invincibilityTimer <= 0 && shieldTimer <= 0) {
            lives--;
            invincibilityTimer = 1.5f;
        }
    }
};

// Classe para Paredes 
class Wall : public Entidade {
public:
    Vector2 velocity;
    // Construtor: Inicializa a posição, dimensões e velocidade da parede.
    Wall(float x, float y, float w, float h, float vx, float vy) {
        rect = { x, y, w, h };
        velocity = { vx, vy };
    }
    // Atualiza a posição da parede e inverte a direção se ela atingir as bordas da tela.
    void Update(float delta) override {
        rect.x += velocity.x * delta;
        rect.y += velocity.y * delta;

        if (rect.x < 0 || rect.x + rect.width > SCREEN_WIDTH) velocity.x *= -1;
        if (rect.y < 0 || rect.y + rect.height > SCREEN_HEIGHT) velocity.y *= -1;
    }
    // Desenha a parede na tela.
    void Draw() const override {
        DrawRectangleRec(rect, WALL_COLOR);
        DrawRectangleLinesEx(rect, 2, WALL_BORDER_COLOR);
    }
};

// Classe para Paredes Especiais
class ParedeEspecial : public Wall {
public:
    // Construtor: Chama o construtor da classe base Wall.
    ParedeEspecial(float x, float y, float w, float h, float vx, float vy) : Wall(x, y, w, h, vx, vy) {}
    // Desenha a parede com um efeito de pulsação de opacidade e design diferente das paredes normais
    void Draw() const override {
        float alpha = (sinf(GetTime() * 8.0f) + 1.0f) / 2.0f;
        DrawRectangleRec(rect, Fade(SPECIAL_WALL_COLOR, 0.4f + alpha * 0.6f));
        DrawRectangleLinesEx(rect, 4, Fade(SPECIAL_WALL_BORDER_COLOR, alpha * 0.9f));

        for(int i = 0; i <= rect.width; i += 15) DrawLine((int)rect.x + i, (int)rect.y, (int)rect.x + i, (int)(rect.y + rect.height), Fade(UI_HIGHLIGHT_COLOR, ( (sinf(GetTime() * 12.0f) + 1.0f) / 2.0f) * 0.3f));
        for(int i = 0; i <= rect.height; i += 15) DrawLine((int)rect.x, (int)rect.y + i, (int)(rect.x + rect.width), (int)rect.y + i, Fade(UI_HIGHLIGHT_COLOR, ( (sinf(GetTime() * 12.0f) + 1.0f) / 2.0f) * 0.3f));
    }
};

// Classe do Objetivo Final 
class Objetivo : public Entidade {
public:
    bool enabled;
    float enableTimer;
    Vector2 initialSize;

    // Construtor: Inicializa a posição e tamanho do objetivo.
    Objetivo(float x, float y) : enabled(false), enableTimer(0.0f) {
        rect = { x, y, 40, 40 };
        initialSize = { 40, 40 };
    }

    // Atualiza o objetivo para sua animação de surgimento.
    void Update(float delta) override {
        if (enabled && enableTimer < 1.0f) {
            enableTimer += delta * 2.0f;
            if (enableTimer > 1.0f) enableTimer = 1.0f;
        }
    }

    // Desenha o objetivo com base no estado 'enabled' e na animação.
    void Draw() const override {
        if (!enabled) {
            DrawRectangleRec(rect, GOAL_DISABLED_COLOR);
            DrawRectangleLinesEx(rect, 3, Fade(WALL_BORDER_COLOR, 0.5f));
        } else {
            // Objetivo habilitado
            float currentSize = initialSize.x * (0.5f + 0.5f * enableTimer);
            if (enableTimer >= 1.0f) currentSize = initialSize.x;

            Rectangle currentRect = {
                rect.x + (initialSize.x - currentSize) / 2.0f,
                rect.y + (initialSize.y - currentSize) / 2.0f,
                currentSize,
                currentSize
            };

            float alphaPulse = (sinf(GetTime() * 7.0f) + 1.0f) / 2.0f;
            // Desenhaa o brilho pulsante ao redor do objetivo.
            DrawCircleV({currentRect.x + currentRect.width/2, currentRect.y + currentRect.height/2}, currentRect.width * (0.8f + alphaPulse * 0.3f), Fade(COIN_GLOW_COLOR, 0.5f + alphaPulse * 0.5f));

            // Desenha o objetivo principal.
            DrawRectangleRec(currentRect, GOAL_COLOR);
            DrawRectangleLinesEx(currentRect, 3, WALL_BORDER_COLOR);

            // Efeito de portal interno.
            DrawCircle((int)(currentRect.x + currentRect.width / 2), (int)(currentRect.y + currentRect.height / 2), (int)(currentRect.width / 2 * (0.7f + alphaPulse * 0.1f)), GOAL_INNER_COLOR);
            DrawCircle((int)(currentRect.x + currentRect.width / 2), (int)(currentRect.y + currentRect.height / 2), (int)(currentRect.width / 2 * (0.5f + alphaPulse * 0.2f)), Fade(GOAL_COLOR, 0.4f));
        }
    }

    // Habilita o objetivo e inicia a animação.
    void Enable() {
        if (!enabled) {
            enabled = true;
            enableTimer = 0.0f;
        }
    }
};

// =============================================================================
//                      CLASSES DE ITENS COLETÁVEIS E POWER-UPS
// =============================================================================

// Classe para Moedas do jogo
class Coin : public Entidade {
public:
    bool active;
    float pulse;
    // Construtor: Inicializa a posição e tamanho da moeda.
    Coin(float x, float y) {
        rect = { x, y, 15, 15 };
        active = true;
        pulse = (float)GetRandomValue(0, 100);
    }
    // Desenha a moeda se estiver ativa, com um efeito de pulsação de tamanho.
    void Draw() const override {
        if (active) {
            float sizeFactor = 1.0f + 0.15f * sinf(GetTime() * 5.0f + pulse);
            float glowAlpha = 0.4f + 0.4f * sinf(GetTime()*10.0f + pulse);

            // Brilho externo da moeda.
            DrawCircle((int)(rect.x + rect.width / 2), (int)(rect.y + rect.height / 2), (rect.width / 2) * sizeFactor * 1.8f, Fade(COIN_GLOW_COLOR, glowAlpha));

            DrawCircle((int)(rect.x + rect.width / 2), (int)(rect.y + rect.height / 2), (rect.width / 2) * sizeFactor, COIN_COLOR);

            // Efeito de orbe/energia no centro da moeda.
            DrawCircle((int)(rect.x + rect.width / 2), (int)(rect.y + rect.height / 2), (rect.width / 2) * sizeFactor * 0.4f, Fade(UI_HIGHLIGHT_COLOR, glowAlpha * 0.8f));
        }
    }

    // Coletar a moeda se o jogador colidir com ela.
    bool TryCollect(Player& player) {
        if (active && CheckCollisionRecs(player.rect, rect)) {
            active = false;
            return true;
        }
        return false;
    }
};

// Classe para Power-Ups do jogo 
class PowerUp : public Entidade {
public:
    bool active;
    float spawnTimer;
    PowerUpType type;

    // Construtor: Inicializa o tipo de power-up e define seu estado inicial.
    PowerUp(PowerUpType t) : type(t) {
        active = false;
        spawnTimer = (float)GetRandomValue(8, 15);
        rect = { 0, 0, 25, 25 };
    }

    // Atualiza a lógica do power-up, incluindo seu reaparecimento.
    void Update(float delta) override {
        if (!active) {
            spawnTimer -= delta;
            if (spawnTimer <= 0) {
                active = true;
                // Define uma posição aleatória na tela, com margem das bordas.
                rect.x = (float)GetRandomValue(100, SCREEN_WIDTH - 100);
                rect.y = (float)GetRandomValue(100, SCREEN_HEIGHT - 100);
                spawnTimer = (float)GetRandomValue(15, 25);
            }
        }
    }

    // Desenha o power-up na tela acordando com o seu tipo.
    void Draw() const override {
        if (active) {
            Color c;
            const char* t;
            Color glowColor = UI_HIGHLIGHT_COLOR;

            if (type == PowerUpType::LIFE) { c = LIFE_POWERUP_COLOR; t = "L"; glowColor = LIFE_POWERUP_COLOR; }
            else if (type == PowerUpType::SHIELD) { c = SHIELD_POWERUP_COLOR; t = "S"; glowColor = SHIELD_POWERUP_COLOR; }
            else { c = SPEED_POWERUP_COLOR; t = "V"; glowColor = SPEED_POWERUP_COLOR; }

            float pulseAlpha = 0.2f + 0.2f * sinf(GetTime() * 12.0f);

            // Brilho de fundo pulsante.
            DrawCircle((int)(rect.x + rect.width / 2), (int)(rect.y + rect.height / 2), rect.width * 1.5f, Fade(glowColor, pulseAlpha));

            // Desenha o fundo e a borda do power-up.
            DrawRectangleRec(rect, c);
            DrawRectangleLinesEx(rect, 2, Fade(UI_HIGHLIGHT_COLOR, 0.7f + 0.3f * sinf(GetTime() * 10.0f)));

            // Desenha o ícone/texto dentro do power-up.
            Vector2 textSize = MeasureTextEx(GetFontDefault(), t, 20, 1);
            DrawText(t, (int)(rect.x + (rect.width - textSize.x) / 2), (int)(rect.y + (rect.height - textSize.y) / 2), 20, UI_HIGHLIGHT_COLOR);
        }
    }

    // Tenta aplicar o efeito do power-up ao jogador se houver colisão.
    bool TryCollect(Player& player) {
        if (active && CheckCollisionRecs(player.rect, this->rect)) {
            active = false;
            // Aplica o efeito correspondente ao tipo de power-up.
            if (type == PowerUpType::LIFE && player.lives < 3) player.lives++;
            if (type == PowerUpType::SHIELD) player.shieldTimer = 5.0f;
            if (type == PowerUpType::SPEED) {
                player.speedBoostTimer = 4.0f;
                player.currentSpeed = player.baseSpeed * 1.5f;
            }
            return true;
        }
        return false;
    }
};

// =============================================================================
//                              SISTEMA DE PARTÍCULAS
// =============================================================================

struct Particle {
    Vector2 pos, vel;
    float life;
    Color color;
    float size;
};

class ParticleSystem {
private:
    std::vector<Particle> particles;
public:
    // Emite um número específico de partículas de uma determinada posição com
    // características definidas.
    void Emit(Vector2 pos, int count, Color color, float speed, float size_min = 2.0f, float size_max = 5.0f) {
        for (int i = 0; i < count; ++i) {
            float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
            Vector2 vel = { sinf(angle) * speed, cosf(angle) * speed };
            float life = (float)GetRandomValue(3, 8) / 10.0f;
            float size = (float)GetRandomValue((int)(size_min * 10), (int)(size_max * 10)) / 10.0f;
            particles.push_back({ pos, vel, life, color, size });
        }
    }

    // Atualiza a posição e o tempo de vida de todas as partículas.
    void Update(float delta) {
        for (auto& p : particles) {
            p.pos = MyVector2Add(p.pos, MyVector2Scale(p.vel, delta));
            p.life -= delta;
        }
        //remove as partículas que expiraram.
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p) {
            return p.life <= 0;
        }), particles.end());
    }

    // Desenha todas as partículas ativas na tela, com efeitos de fade-out e diminuição de tamanho.
    void Draw() const {
        for (const auto& p : particles) {
            DrawCircleV(p.pos, p.size * (p.life / 0.8f), Fade(p.color, p.life / 0.8f));
        }
    }
};

// =============================================================================
//                        CLASSE PARA BOTÕES INTERATIVOS
// =============================================================================
// Cria interface.
class Button {
public:
    Rectangle rect;
    std::string text;
    bool hovered = false;
    float fontSize;

    // Construtor: Inicializa a posição, tamanho, texto e tamanho da fonte do botão.
    Button(float x, float y, float w, float h, const char* t, float fs = 30) : rect{ x,y,w,h }, text(t), fontSize(fs) {}

    void Update() {
        hovered = CheckCollisionPointRec(GetMousePosition(), rect);
    }

    // Desenha o botão na tela, com efeitos visuais para o estado de "hover".
    void Draw(Font& font) const {
        Color bgColor = hovered ? WALL_COLOR : BACKGROUND_DEEPEST_DARK;
        Color textColor = hovered ? UI_HIGHLIGHT_COLOR : UI_TEXT_COLOR;
        DrawRectangleRec(rect, bgColor);
        DrawRectangleLinesEx(rect, 2, UI_TEXT_COLOR);
        Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, 1);
        float textYOffset = (rect.height - textSize.y) / 2.0f;
        DrawTextEx(font, text.c_str(), { rect.x + (rect.width - textSize.x) / 2.0f, rect.y + textYOffset }, fontSize, 1, textColor);
    }

    // Verifica se o botão foi clicado (mouse sobre ele e botão esquerdo pressionado).
    bool IsClicked() { return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT); }
};

// =============================================================================
//                          CLASSE PRINCIPAL DO JOGO (Game)
// =============================================================================
class Game {
private:
    GameState state;
    Player player;
    std::vector<Wall*> walls;
    std::vector<Coin> coins;
    std::vector<PowerUp> powerups;
    Objetivo* objetivo = nullptr;
    int currentLevel;
    const int maxLevels;
    float totalGameTime;
    const float initialGameTime = 180.0f;
    int score;

    ParticleSystem particles;
    Camera2D camera;
    float screenShakeAmount;
    Font customFont;
    Texture2D lifeTexture;

    // Botões para as telas de menu e pausa.
    Button startButton;
    Button resumeButton;
    Button exitButton;

    // Recursos de áudio (sons e música).
    Sound hitSound, victorySound, collectCoinSound, collectPowerupSound, spawnSound, clickSound, fieldEnable;
    Sound speedUpSound;
    Sound finalVictorySound;
    Music backgroundMusic;

public:
    // Construtor da classe Game. Inicializa a janela, áudio e botões.
    Game() :
        state(GameState::MENU),
        maxLevels(3),
        score(0),
        screenShakeAmount(0.0f),
        startButton(0, 0, 250, 60, "INICIAR JOGO", 30),
        resumeButton(0, 0, 250, 60, "RESUMIR JOGO", 30),
        exitButton(0, 0, 250, 60, "SAIR DO JOGO", 30)
    {
        InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Labirinto das Sombras - by Tifany");
        SetTargetFPS(60);
        InitAudioDevice();
        LoadResources();
        camera.zoom = 1.0f;
    }

    // Destrutor da classe Game. Libera todos os recursos alocados.
    ~Game() {
        UnloadResources();
        ClearLevel();
        CloseAudioDevice();
        CloseWindow();
    }

    // Loop principal do jogo.
    void Run() {
        while (!WindowShouldClose()) {
            Update(GetFrameTime());
            Draw();
        }
    }

private:
    // Carrega todos os recursos (fontes, sons, música) necessários para o jogo.
    void LoadResources() {
        customFont = LoadFontEx("assets/font.ttf", 64, nullptr, 0);
        if (customFont.texture.id == 0) {
            customFont = GetFontDefault();
            TraceLog(LOG_WARNING, "FONT: Falha ao carregar 'assets/font.ttf', usando a fonte padrão do próprio raylib.");
        } else {
            TraceLog(LOG_INFO, "FONT: 'assets/font.ttf' foi encontrada.");
        }

        lifeTexture = LoadTexture("assets/life.png");

        hitSound = LoadSound("assets/hit.mp3");
        victorySound = LoadSound("assets/victory.mp3");
        fieldEnable = LoadSound("assets/fieldEnable.mp3");
        collectCoinSound = LoadSound("assets/collect_coin.mp3");
        collectPowerupSound = LoadSound("assets/collect_powerup.mp3");
        spawnSound = LoadSound("assets/spawn.mp3");
        clickSound = LoadSound("assets/click.mp3");
        speedUpSound = LoadSound("assets/speed_up.mp3");
        finalVictorySound = LoadSound("assets/endVictory.mp3");
        backgroundMusic = LoadMusicStream("assets/music.mp3");
        PlayMusicStream(backgroundMusic);
    }

    // Descarrega todos os recursos carregados para liberar a memória.
    void UnloadResources() {
        if (customFont.texture.id != 0 && customFont.recs != GetFontDefault().recs) {
             UnloadFont(customFont);
             TraceLog(LOG_INFO, "FONT: Fonte personalizada descarregada.");
        }

        UnloadTexture(lifeTexture);

        UnloadSound(hitSound);
        UnloadSound(victorySound);
        UnloadSound(fieldEnable);
        UnloadSound(collectCoinSound);
        UnloadSound(collectPowerupSound);
        UnloadSound(spawnSound);
        UnloadSound(clickSound);
        UnloadSound(speedUpSound);
        UnloadSound(finalVictorySound);
        UnloadMusicStream(backgroundMusic);
    }

    // Limpa todas as entidades do nível atual para preparar um novo nível.
    void ClearLevel() {
        for (auto wall : walls) delete wall;
        walls.clear();
        coins.clear();
        powerups.clear();
        delete objetivo;
        objetivo = nullptr;
    }

    // Carrega a configuração específica de paredes, moedas e power-ups para um dado nível.
    void LoadLevel(int level) {
        ClearLevel();
        player.Reset();

        coins.clear();
        switch(level) {
            case 1:
                coins.emplace_back(SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.1f);
                coins.emplace_back(SCREEN_WIDTH - SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT * 0.08f);
                coins.emplace_back(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
                coins.emplace_back(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.1f);
                coins.emplace_back(SCREEN_WIDTH - SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.15f);
                break;
            case 2:
                coins.emplace_back(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.4f);
                coins.emplace_back(SCREEN_WIDTH * 0.475f, SCREEN_HEIGHT * 0.15f);
                coins.emplace_back(SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT * 0.6f);
                coins.emplace_back(SCREEN_WIDTH - SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.3f);
                coins.emplace_back(SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.05f);
                coins.emplace_back(SCREEN_WIDTH - SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.05f);
                coins.emplace_back(SCREEN_WIDTH / 2.0f + 50, SCREEN_HEIGHT / 2.0f + 50);
                break;
            case 3:
                coins.emplace_back(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.45f);
                coins.emplace_back(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.05f);
                coins.emplace_back(SCREEN_WIDTH - SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.25f);
                coins.emplace_back(SCREEN_WIDTH * 0.7f, SCREEN_HEIGHT * 0.7f);
                coins.emplace_back(SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.6f);
                coins.emplace_back(SCREEN_WIDTH / 2.0f + 100, SCREEN_HEIGHT * 0.3f);
                coins.emplace_back(SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.2f);
                coins.emplace_back(SCREEN_WIDTH - SCREEN_WIDTH * 0.05f, SCREEN_HEIGHT * 0.1f);
                coins.emplace_back(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT * 0.8f); // arrumar posição da moeda
                coins.emplace_back(SCREEN_WIDTH * 0.15f, SCREEN_HEIGHT * 0.15f);
                break;
        }

        objetivo = new Objetivo((float)SCREEN_WIDTH - 80, (float)SCREEN_HEIGHT / 2.0f - 20);
        objetivo->enabled = false;

        powerups.emplace_back(PowerUpType::LIFE);
        powerups.emplace_back(PowerUpType::SHIELD);
        if (level > 1) {
            powerups.emplace_back(PowerUpType::SPEED);
        }

        switch (level) { //posição das paredes é setada manualmente.
            case 1:
                walls.push_back(new Wall(SCREEN_WIDTH * 0.15f, 0, 20, SCREEN_HEIGHT * 0.25f, 0, 70));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.25f, 20, SCREEN_HEIGHT * 0.25f, 0, -90));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.45f, 0, 20, SCREEN_HEIGHT * 0.3f, 0, 100));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.6f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.35f, 20, SCREEN_HEIGHT * 0.35f, 0, -80));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.2f, 100, 20, 50, 0));
                walls.push_back(new Wall(SCREEN_WIDTH - SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.4f, 100, 20, -50, 0));
                break;
            case 2:
                walls.push_back(new Wall(SCREEN_WIDTH * 0.1f, 0, 20, SCREEN_HEIGHT * 0.3f, 0, 100));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.3f, 20, SCREEN_HEIGHT * 0.3f, 0, -120));
                walls.push_back(new ParedeEspecial(SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.05f, 20, SCREEN_HEIGHT * 0.4f, 0, 150));
                walls.push_back(new ParedeEspecial(SCREEN_WIDTH * 0.55f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.45f, 20, SCREEN_HEIGHT * 0.4f, 0, -140));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.25f, 150, 20, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH - SCREEN_WIDTH * 0.35f, SCREEN_HEIGHT * 0.35f, 150, 20, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH / 2.0f - 100, SCREEN_HEIGHT * 0.1f, 200, 20, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH / 2.0f - 100, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.12f, 200, 20, 0, 0));
                break;
            case 3:
                walls.push_back(new Wall(SCREEN_WIDTH * 0.1f, SCREEN_HEIGHT * 0.05f, 20, SCREEN_HEIGHT * 0.35f, 0, 180));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.25f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.4f, 20, SCREEN_HEIGHT * 0.35f, 0, -200));
                walls.push_back(new ParedeEspecial(SCREEN_WIDTH * 0.4f, 0, 20, SCREEN_HEIGHT * 0.5f, 0, 220));
                walls.push_back(new ParedeEspecial(SCREEN_WIDTH * 0.55f, SCREEN_HEIGHT - SCREEN_HEIGHT * 0.55f, 20, SCREEN_HEIGHT * 0.5f, 0, -210));
                walls.push_back(new Wall(SCREEN_WIDTH * 0.2f, SCREEN_HEIGHT * 0.2f, 200, 20, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH - SCREEN_WIDTH * 0.4f, SCREEN_HEIGHT * 0.4f, 200, 20, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH / 3.0f, 0, 20, 100, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH / 3.0f, SCREEN_HEIGHT - 100, 20, 100, 0, 0));
                walls.push_back(new Wall(20, SCREEN_HEIGHT / 3.0f, 80, 20, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH - 100, SCREEN_HEIGHT / 3.0f, 80, 20, 0, 0));
                walls.push_back(new Wall(SCREEN_WIDTH / 2.0f - 150, SCREEN_HEIGHT / 2.0f - 10, 300, 20, 0, 0));
                walls.push_back(new ParedeEspecial(SCREEN_WIDTH / 2.0f - 10, 50, 20, 150, 0, 100));
                walls.push_back(new ParedeEspecial(SCREEN_WIDTH / 2.0f - 10, SCREEN_HEIGHT - 200, 20, 150, 0, -100));
                break;
        }
    }

    // Inicia um novo jogo, redefinindo o estado e carregando o primeiro nível.
    void StartGame() {
        state = GameState::PLAYING;
        currentLevel = 1;
        totalGameTime = initialGameTime;
        score = 0;
        LoadLevel(currentLevel);
    }

    // Atualiza a lógica do jogo a cada frame, baseando-se no estado atual.
    void Update(float delta) {
        UpdateMusicStream(backgroundMusic);

        // Lógica para o efeito de tela tremendo quando há impacto.
        if (screenShakeAmount > 0) screenShakeAmount -= 5.0f * delta;
        camera.offset.x = GetRandomValue(-1, 1) * screenShakeAmount * 10;
        camera.offset.y = GetRandomValue(-1, 1) * screenShakeAmount * 10;

        // Gerencia a lógica com base no estado atual do jogo.
        switch (state) {
            case GameState::MENU:
                startButton.Update();
                if (startButton.IsClicked()) {
                    PlaySound(clickSound);
                    StartGame();
                }
                exitButton.Update();
                if (exitButton.IsClicked()) {
                    PlaySound(clickSound);
                    CloseWindow();
                }
                break;
            case GameState::PLAYING:
                UpdatePlaying(delta);
                break;
            case GameState::PAUSED:
                resumeButton.Update();
                if (resumeButton.IsClicked()) {
                    PlaySound(clickSound);
                    state = GameState::PLAYING;
                }
                exitButton.Update();
                if (exitButton.IsClicked()) {
                    PlaySound(clickSound);
                    CloseWindow();
                }
                break;
            case GameState::LEVEL_TRANSITION:
                // Toca win.mp3 na transição de nível
                if (!IsSoundPlaying(victorySound)) PlaySound(victorySound);
                if (IsKeyPressed(KEY_ENTER)) {
                    StopSound(victorySound);
                    state = GameState::PLAYING;
                    currentLevel++;
                    LoadLevel(currentLevel);
                }
                break;
            case GameState::VICTORY:
                // Toca finalVictorySound na vitória final
                if (!IsSoundPlaying(finalVictorySound)) PlaySound(finalVictorySound);
                if (IsKeyPressed(KEY_ENTER)) {
                    StopSound(finalVictorySound); // Para o som ao voltar para o menu
                    state = GameState::MENU;
                }
                break;
            case GameState::GAMEOVER:
                if (!IsSoundPlaying(hitSound)) PlaySound(hitSound);
                if (IsKeyPressed(KEY_ENTER)) {
                    StopSound(hitSound); // Para o som ao voltar para o menu
                    state = GameState::MENU;
                }
                break;
        }
        particles.Update(delta);
        if (objetivo) objetivo->Update(delta);
    }

    // Contém a lógica de jogo que é executada quando o estado é PLAYING.
    void UpdatePlaying(float delta) {
        if (IsKeyPressed(KEY_P)) state = GameState::PAUSED;

        totalGameTime -= delta;

        // Verifica se o tempo acabou.
        if (totalGameTime <= 0) {
            state = GameState::GAMEOVER;
            PlaySound(hitSound);
            return;
        }

        player.Update(delta);

        // Emite partículas de rastro do jogador quando ele se move.
        if (IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D) ||
            IsKeyDown(KEY_UP) || IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_RIGHT)) {
            particles.Emit({ player.rect.x + player.rect.width / 2, player.rect.y + player.rect.height / 2 }, 1, BACKGROUND_DEEPEST_DARK, 50, 1.0f, 3.0f);
        }

        // Contador de moedas coletadas.
        int collectedCoins = 0;
        for (auto& p : powerups) {
            bool was_inactive = !p.active;
            p.Update(delta);
            if (was_inactive && p.active) PlaySound(spawnSound);
            if (p.TryCollect(player)) {
                PlaySound(collectPowerupSound);
                Color particleColor = p.type == PowerUpType::LIFE ? LIFE_POWERUP_COLOR : (p.type == PowerUpType::SHIELD ? SHIELD_POWERUP_COLOR : SPEED_POWERUP_COLOR);
                particles.Emit({ p.rect.x + p.rect.width / 2, p.rect.y + p.rect.height / 2 }, 20, particleColor, 100, 3.0f, 6.0f);
                if (p.type == PowerUpType::SPEED) PlaySound(speedUpSound);
            }
        }

        for (auto& coin : coins) {
            if (coin.TryCollect(player)) {
                PlaySound(collectCoinSound);
                score += 100;
                particles.Emit({ coin.rect.x + coin.rect.width / 2, coin.rect.y + coin.rect.height / 2 }, 15, COIN_COLOR, 80, 2.0f, 4.0f);
            }
            if (!coin.active) collectedCoins++;
        }

        // Habilita o objetivo se todas as moedas foram coletadas.
        if (objetivo && !objetivo->enabled && collectedCoins == coins.size()) {
            objetivo->Enable();
            particles.Emit({ objetivo->rect.x + objetivo->rect.width / 2, objetivo->rect.y + objetivo->rect.height / 2 }, 50, GOAL_COLOR, 200, 5.0f, 10.0f);
            PlaySound(fieldEnable);
        }

        // Atualiza e verifica colisões com as paredes.
        for (auto wall : walls) {
            wall->Update(delta);
            if (CheckCollisionRecs(player.rect, wall->rect)) {
                if (player.invincibilityTimer <= 0 && player.shieldTimer <= 0) {
                    player.TakeDamage();
                    PlaySound(hitSound);
                    particles.Emit({ player.rect.x + player.rect.width / 2, player.rect.y + player.rect.height / 2 }, 30, UI_WARNING_COLOR, 120, 4.0f, 8.0f);
                    screenShakeAmount = 1.0f;
                }
            }
        }

        // Verifica a condição de Game Over (vidas esgotadas).
        if (player.lives <= 0) state = GameState::GAMEOVER;
        // Verifica se o jogador alcançou o objetivo final do nível, APENAS SE ESTIVER HABILITADO O CAMPO!
        if (objetivo && objetivo->enabled && CheckCollisionRecs(player.rect, objetivo->rect)) {
            score += 5000;
            if (currentLevel < maxLevels) {
                state = GameState::LEVEL_TRANSITION;
            } else {
                state = GameState::VICTORY;
                // O som de vitória final será tocado no Update()
            }
        }
    }

    // Função de desenho principal que renderiza todos os elementos na tela.
    void Draw() {
        BeginDrawing();
        ClearBackground(BACKGROUND_DEEPEST_DARK);

        BeginMode2D(camera);

        // Efeito de fundo de névoa.
        float fogSpeed = 20.0f;
        float timeOffset = fmod(GetTime() * fogSpeed, SCREEN_HEIGHT * 2);

        for(int i = 0; i < 2; i++) {
            float currentY = (float)SCREEN_HEIGHT - timeOffset + (i * SCREEN_HEIGHT);
            DrawRectangleGradientV(0, (int)currentY, SCREEN_WIDTH, SCREEN_HEIGHT, BACKGROUND_FOG_START, BACKGROUND_FOG_END);
        }

        for (int i = 0; i < 200; i++) {
            float randX = (float)GetRandomValue(0, SCREEN_WIDTH);
            float randY = (float)GetRandomValue(0, SCREEN_HEIGHT);
            float randAlpha = (float)GetRandomValue(10, 80) / 255.0f;
            float randSize = (float)GetRandomValue(1, 2);
            DrawCircleV({randX, randY}, randSize, Fade(UI_TEXT_COLOR, randAlpha));
        }

        // Desenha elementos específicos com base no estado atual do jogo.
        switch (state) {
            case GameState::MENU: DrawMenu(); break;
            case GameState::PLAYING: DrawPlaying(); break;
            case GameState::PAUSED:
                DrawPlaying();
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BACKGROUND_DEEPEST_DARK, 0.95f));
                DrawPausedScreen();
                break;
            case GameState::LEVEL_TRANSITION:
                DrawPlaying();
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BACKGROUND_DEEPEST_DARK, 0.95f));
                DrawLevelTransitionScreen();
                break;
            case GameState::VICTORY: DrawEndScreen("YOU WIN!", GOAL_COLOR); break;
            case GameState::GAMEOVER: DrawEndScreen("GAME OVER.", UI_WARNING_COLOR); break;
        }

        EndMode2D();
        EndDrawing();
    }

    // Desenha a tela inicial do menu.
    void DrawMenu() {
        const char* gameTitle = "LABIRINTO DAS SOMBRAS";
        float titleFontSize = 90;
        Vector2 titleSize = MeasureTextEx(customFont, gameTitle, titleFontSize, 2);
        float titleYPosition = SCREEN_HEIGHT * 0.25f - titleSize.y / 2.0f;
        DrawTextEx(customFont, gameTitle, { (SCREEN_WIDTH - titleSize.x) / 2.0f, titleYPosition }, titleFontSize, 2, UI_ACCENT_COLOR);

        float buttonWidth = 300;
        float buttonHeight = 75;
        float buttonFontSize = 40;
        float buttonSpacing = 35;
        float buttonOverallYOffset = SCREEN_HEIGHT * 0.08f;

        float totalButtonsHeight = (buttonHeight * 2) + buttonSpacing;
        float startY = (SCREEN_HEIGHT - totalButtonsHeight) / 2.0f + buttonOverallYOffset;

        startButton.rect.x = (SCREEN_WIDTH - buttonWidth) / 2.0f;
        startButton.rect.y = startY;
        startButton.rect.width = buttonWidth;
        startButton.rect.height = buttonHeight;
        startButton.fontSize = buttonFontSize;
        startButton.Draw(customFont);

        exitButton.rect.x = (SCREEN_WIDTH - buttonWidth) / 2.0f;
        exitButton.rect.y = startY + buttonHeight + buttonSpacing;
        exitButton.rect.width = buttonWidth;
        exitButton.rect.height = buttonHeight;
        exitButton.fontSize = buttonFontSize;
        exitButton.Draw(customFont);

        const char* signature = "POR TIFANY LUIZA";
        float signatureFontSize = 28;
        Vector2 signatureSize = MeasureTextEx(customFont, signature, signatureFontSize, 1);
        DrawTextEx(customFont, signature, { (SCREEN_WIDTH - signatureSize.x) / 2.0f, SCREEN_HEIGHT - 45 }, signatureFontSize, 1, UI_TEXT_COLOR);
    }

    // Desenha todos os elementos do jogo durante a partida (jogador, paredes, moedas, etc.).
    void DrawPlaying() {
        if (objetivo) objetivo->Draw();
        for (const auto& p : powerups) p.Draw();
        for (const auto& coin : coins) coin.Draw();
        for (const auto wall : walls) wall->Draw();
        particles.Draw();
        player.Draw();
        DrawHUD();
    }

    // Desenha a HUD do jogo, incluindo vidas, pontuação e tempo.
    void DrawHUD() {
        DrawTextEx(customFont, "VIDAS:", { 30, 20 }, 30, 1, UI_TEXT_COLOR);
        for (int i = 0; i < player.lives; i++) {
            DrawTextureEx(lifeTexture, { 150 + i * 30.0f, 25.0f }, 0.0f, 0.07f, WHITE);
        }

        std::string scoreText = TextFormat("PONTOS: %06d", score);
        float scoreFontSize = 30;
        Vector2 scoreTextSize = MeasureTextEx(customFont, scoreText.c_str(), scoreFontSize, 1);
        DrawTextEx(customFont, scoreText.c_str(), { SCREEN_WIDTH - scoreTextSize.x - 30, 20 }, scoreFontSize, 1, UI_TEXT_COLOR);

        std::string timeText = TextFormat("TEMPO: %03d", (int)totalGameTime);
        float timeFontSize = 30;
        Vector2 timeTextSize = MeasureTextEx(customFont, timeText.c_str(), timeFontSize, 1);
        Color timeColor = UI_TEXT_COLOR;
        if (totalGameTime < 10 && (int)(GetTime() * 5) % 2 == 0) {
            timeColor = UI_WARNING_COLOR;
        } else if (totalGameTime < 20) {
            timeColor = CLITERAL(Color){255, 140, 0, 255};
        } else {
            timeColor = UI_TEXT_COLOR;
        }
        DrawTextEx(customFont, timeText.c_str(), { (SCREEN_WIDTH - timeTextSize.x) / 2.0f, 20 }, timeFontSize, 1, timeColor);
    }

    // Desenha a tela de pausa.
    void DrawPausedScreen() {
        const char* pausedText = "PAUSADO";
        float pausedFontSize = 70;
        Vector2 pausedTextSize = MeasureTextEx(customFont, pausedText, pausedFontSize, 1);
        float pausedTextY = (SCREEN_HEIGHT / 2.0f - pausedTextSize.y / 2.0f) - 80;
        DrawTextEx(customFont, pausedText, { (SCREEN_WIDTH - pausedTextSize.x) / 2.0f, pausedTextY }, pausedFontSize, 1, UI_HIGHLIGHT_COLOR);

        float buttonWidth = 300;
        float buttonHeight = 75;
        float buttonFontSize = 40;
        float buttonSpacing = 30;
        float buttonOverallYOffset = 40.0f;

        float totalButtonsHeight = (buttonHeight * 2) + buttonSpacing;
        float startY = (SCREEN_HEIGHT - totalButtonsHeight) / 2.0f + buttonOverallYOffset;

        resumeButton.rect.x = (SCREEN_WIDTH - buttonWidth) / 2.0f;
        resumeButton.rect.y = startY;
        resumeButton.rect.width = buttonWidth;
        resumeButton.rect.height = buttonHeight;
        resumeButton.fontSize = buttonFontSize;
        resumeButton.Draw(customFont);

        exitButton.rect.x = (SCREEN_WIDTH - buttonWidth) / 2.0f;
        exitButton.rect.y = startY + buttonHeight + buttonSpacing;
        exitButton.rect.width = buttonWidth;
        exitButton.rect.height = buttonHeight;
        exitButton.fontSize = buttonFontSize;
        exitButton.Draw(customFont);
    }

    // Desenha a tela de transição de nível.
    void DrawLevelTransitionScreen() {
        const char* levelCompleteText = "LEVEL COMPLETO!";
        float completeFontSize = 70;
        Vector2 levelCompleteTextSize = MeasureTextEx(customFont, levelCompleteText, completeFontSize, 1);
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BACKGROUND_DEEPEST_DARK, 0.9f));

        float completeTextY = (SCREEN_HEIGHT / 2.0f - levelCompleteTextSize.y / 2.0f) - 50;
        DrawTextEx(customFont, levelCompleteText, { (SCREEN_WIDTH - levelCompleteTextSize.x) / 2.0f, completeTextY }, completeFontSize, 1, UI_HIGHLIGHT_COLOR);

        const char* nextLevelPrompt = TextFormat("Pressione ENTER para o LEVEL %d", currentLevel + 1);
        float promptFontSize = 35;
        Vector2 nextLevelPromptSize = MeasureTextEx(customFont, nextLevelPrompt, promptFontSize, 1);
        float promptTextY = (SCREEN_HEIGHT / 2.0f - nextLevelPromptSize.y / 2.0f) + 50;
        DrawTextEx(customFont, nextLevelPrompt, { (SCREEN_WIDTH - nextLevelPromptSize.x) / 2.0f, promptTextY }, promptFontSize, 1, UI_ACCENT_COLOR);
    }

    // Desenha as telas finais de Vitória ou Game Over.
    void DrawEndScreen(const char* title, Color color) {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BACKGROUND_DEEPEST_DARK, 0.9f));

        float titleFontSize = 80;
        Vector2 titleSize = MeasureTextEx(customFont, title, titleFontSize, 2);
        float titleY = (SCREEN_HEIGHT / 2.0f - titleSize.y / 2.0f) - 120;
        DrawTextEx(customFont, title, { (SCREEN_WIDTH - titleSize.x) / 2.0f, titleY }, titleFontSize, 2, color);

        std::string finalScoreText = TextFormat("SCORE FINAL: %06d", score);
        float scoreFontSize = 50;
        Vector2 finalScoreTextSize = MeasureTextEx(customFont, finalScoreText.c_str(), scoreFontSize, 1);
        float scoreY = (SCREEN_HEIGHT / 2.0f - scoreFontSize / 2.0f) + 10;
        DrawTextEx(customFont, finalScoreText.c_str(), { (SCREEN_WIDTH - finalScoreTextSize.x) / 2.0f, scoreY }, scoreFontSize, 1, UI_HIGHLIGHT_COLOR);

        const char* pressEnterText = "PRESSIONE ENTER PARA VOLTAR AO MENU";
        float enterFontSize = 25;
        Vector2 pressEnterTextSize = MeasureTextEx(customFont, pressEnterText, enterFontSize, 1);
        float enterY = (SCREEN_HEIGHT / 2.0f - enterFontSize / 2.0f) + 100;
        DrawTextEx(customFont, pressEnterText, { (SCREEN_WIDTH - pressEnterTextSize.x) / 2.0f, enterY }, enterFontSize, 1, UI_TEXT_COLOR);
    }
};

// =============================================================================
//                             FUNÇÃO PRINCIPAL (main)
// =============================================================================
// O ponto de entrada do programa, em que o game é instanciado e executado.
int main() {
    Game game;
    game.Run();
    return 0;
}