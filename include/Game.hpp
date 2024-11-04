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
    static void Init();
    void Update(Vector2 mousePos, Vector2 windowCenter, sf::Event &event, float deltaTime);
    void Draw(Vector2 mousePos, sf::RenderWindow &window);
    void Save();
    void Load();
private:
    void SpawnPlayer();
    void HandleMouseInput(Vector2 mousePos, Vector2 windowCenter, float deltaTime);
    static void InitBackground();
public:
    Player player;
private:
    Map map;
    static std::shared_ptr<sf::RectangleShape> background;
    GameState gameState{MENU};
    bool loading{false};
};