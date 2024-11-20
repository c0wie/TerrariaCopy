#pragma once 

#include "Map.hpp"
#include "Player.hpp"
#include "Collision.hpp"

class Game
{
public:
    enum GameState
    {
        MENU,
        MAP,
        PLAYER_SELECTOR,
        SETTINGS
    };
public:
    Game();
    Game(const Game &other) = delete;
    Game(Game &&other) = delete;
    Game &operator=(const Game &other) = delete;
    Game &operator=(Game &&other) = delete;
    ~Game() = default;
public:
    void Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event, float deltaTime);
    void Draw(Vector2 mousePos, sf::RenderWindow &window);
    void Save();
    void Load();
    static void InitBackground();

    // temporary function
    Vector2 GetPlayerPosition() const { return m_Player.position; }
private:
    void SpawnPlayer();
    void HandleMouseInput(Vector2 mousePos, Vector2 windowCenter, float deltaTime);
    void HandleCollisions(float deltaTime);
private:
    Player m_Player;
    Map m_Map;
    GameState m_GameState{MENU};
    static sf::RectangleShape m_Background;
};