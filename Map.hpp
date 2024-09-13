#include "Global.hpp"
#include "Player.hpp"
#include <SFML/Graphics.hpp>

struct Tile
{
    Tile() = default;
    Tile(sf::Vector2f position) : position(position) {}
    void Draw(sf::RenderWindow &window) const
    {
        sf::RectangleShape tileBody{size};
        tileBody.setPosition(position);
        tileBody.setOrigin(size / 2.0f);
        tileBody.setFillColor(sf::Color::Green);
        window.draw(tileBody);
    }
    sf::Vector2f position{0.0f, 0.0f};
    sf::Vector2f size{50.0f, 50.0f};
};

void SolveCollsion(Player &player, const Tile &tile);

class Map
{
public:
    Map(int32_t size);
public:
    void Update(float deltaTime);
    void Draw(sf::RenderWindow &window) const;
    Player GetPlayer() const { return m_Player; }
private:
    void HandleKeyboardInput();
    void HandleCollision();
private:
    std::vector<Tile> m_TiledMap;
    Player m_Player;
};
