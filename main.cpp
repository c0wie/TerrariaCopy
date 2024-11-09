#include "Global.hpp"
#include "Game.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdlib> 
#include <ctime>   
#include <stack>
#include "Vector2.h"

// I bet I'd forgot about this declaration and scratch my head for an hour why I get linekr problem LoL
/*constexpr float TILE_SIZE = 64.0f;
constexpr float SCREEN_WIDTH = 1200.0f;
constexpr float SCREEN_HEIGHT = 900.0f;
constexpr float BUFFOR = 200.0f;
constexpr int MAP_WIDTH = 8;
constexpr int MAP_HEIGHT = 8;

enum TileType
{
    Wall,
    Empty,
    Light
};
struct TestTiles
{
public:
    TestTiles() {}
    TestTiles(Vector2 position, short type) : 
        position(position)
    {
        SetType(type);
    }   
public:
    void Draw(sf::RenderWindow &window) const
    {
        sf::RectangleShape tile(size);
        tile.setOrigin(size / 2.0f);
        tile.setFillColor(color);
        tile.setOutlineThickness(0.8f);
        tile.setOutlineColor(sf::Color::Black);
        tile.setPosition(position);
        sf::RectangleShape overlay(size);
        overlay.setOrigin(size / 2.0f);
        overlay.setPosition(position);
        overlay.setFillColor({0, 0, 0, alfa});
        window.draw(tile);
        window.draw(overlay);
    }
    void SetType(short Type)
    {
        if(Type == Wall)
        {
            lightConsupmtion = 1.0f;
            lightStrength = 0.0f;
            isLightSource = false;
            color = sf::Color::Blue;
            type = Wall;
        }
        else if(Type == Light)
        {
            lightConsupmtion = 0.0f;
            lightStrength = 255.0f;
            isLightSource = true;
            color = sf::Color::Yellow;
            type = Light;
        }
        else if(Type == Empty)
        {
            lightConsupmtion = 0.0f;
            lightStrength = 0.0f;
            isLightSource = false;
            color = sf::Color::White;
            type = Empty;
        }
    }
    Vector2 GetCoords() const 
    {
        return Vector2{(position.x - (2 * BUFFOR) ) / TILE_SIZE, (position.y - BUFFOR) / TILE_SIZE};
    }
public:
    sf::Color color{sf::Color::White};
    Vector2 position{-1.0f, -1.0f};
    Vector2 size{TILE_SIZE, TILE_SIZE};
    float lightConsupmtion{51.0f};
    float lightStrength{0.0f};
    short type{Empty};
    unsigned int alfa{255};
    bool isLightSource{false};
};

bool IsValidCoords(Vector2 coords) 
{
    if(coords.x < 0 && coords.x >= MAP_WIDTH)
    {
        return false;
    }
    if(coords.y < 0 && coords.y >= MAP_HEIGHT)
    {
        return false;
    }
    return true;
}

void convertSketch(std::array<TestTiles, MAP_WIDTH * MAP_HEIGHT> &map, const std::array<std::string, MAP_HEIGHT> &mapSketch)
{
    for(int y = 0; y < MAP_HEIGHT; y++)
    {
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            switch (mapSketch[y][x])
            {
            case '#':
                map[y * MAP_WIDTH + x].SetType(Wall);
                break;
            case 'L':
                map[y * MAP_WIDTH + x].SetType(Light);
                break;
            case ' ':
                map[y * MAP_WIDTH + x].SetType(Empty);
                break;
            default:
                std::cout << "smth has gone wrong\n" << mapSketch[y][x] << '\n';
                std::exit(1);
            }
        }
    }
}

void Update(std::array<TestTiles, MAP_WIDTH * MAP_HEIGHT> &testMap, TestTiles &lightSource)
{
    std::stack<Vector2> lightStack;
    std::vector<bool> visitedTilesCoords(MAP_WIDTH * MAP_HEIGHT, false);
    lightStack.push(lightSource.GetCoords());
    float lightBrightness = lightSource.lightStrength;
    while (!lightStack.empty() && lightBrightness > 0.0f)
    {
        TestTiles &tile = testMap[lightStack.top().y * MAP_WIDTH + lightStack.top().x];
        lightStack.pop();
        const Vector2 coords = tile.GetCoords();
        if(visitedTilesCoords[coords.y * MAP_WIDTH + coords.x] == true)
        {
            continue;
        }
        
        //right tile
        if( IsValidCoords({coords.x + 1, coords.y}) )
        {
            lightStack.push({coords.x + 1, coords.y});
        }
        //left tile
        if( IsValidCoords({coords.x - 1, coords.y}) )
        {
            lightStack.push({coords.x - 1, coords.y});
        }
        //top tile
        if( IsValidCoords({coords.x, coords.y - 1}) )
        {
            lightStack.push({coords.x, coords.y - 1});
        }
        //bottom tile
        if( IsValidCoords({coords.x, coords.y + 1}) )
        {
            lightStack.push({coords.x, coords.y + 1});
        }
        //bottom right tile
        if( IsValidCoords({coords.x + 1, coords.y + 1}) )
        {
            lightStack.push({coords.x + 1, coords.y + 1});
        }
        // bottom left tile
        if( IsValidCoords({coords.x - 1, coords.y + 1}) )
        {
            lightStack.push({coords.x - 1, coords.y + 1});
        }
        // top right tile
        if( IsValidCoords({coords.x + 1, coords.y - 1}) )
        {
            lightStack.push({coords.x + 1, coords.y - 1});
        }
        // top left tile
        if( IsValidCoords({coords.x - 1, coords.y - 1}) )
        {
            lightStack.push({coords.x - 1, coords.y - 1});
        }
        tile.alfa -= lightBrightness;
        lightBrightness -= tile.lightConsupmtion / 8.0f;
        visitedTilesCoords[coords.y * MAP_WIDTH + coords.x] = true;
    }
}
*/
int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    sf::View view({0.0f, 0.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT});
    ImGui::SFML::Init(window);

    std::srand((unsigned int)std::time(NULL));
    Game::Init();
    Game game;
    

    sf::Clock DT_Clock;
    float deltaTime = 0.0f;
    DT_Clock.restart();
    while(window.isOpen())
    {
        const Vector2 mousePosition = sf::Mouse::getPosition(window);
        const Vector2 windowCenter = window.getView().getCenter();
        const Vector2 worldMousePosition = window.mapPixelToCoords(mousePosition);
        deltaTime = DT_Clock.restart().asSeconds();
        // adds limit to how long one frame could be
        if(deltaTime > 1.0f / 20.0f)
        {
            deltaTime = 1.0f / 20.0f;
        }
        sf::Time clock = sf::seconds(deltaTime);

        sf::Event evnt{};
        while (window.pollEvent(evnt))
        {
            ImGui::SFML::ProcessEvent(evnt);
            if(evnt.type == sf::Event::Closed)
            {
                game.Save();
                window.close();
            }
        }
        view.setCenter(game.player.position);
        window.setView(view);
        ImGui::SFML::Update(window, clock);
        game.Update(worldMousePosition, windowCenter, evnt, deltaTime);
        window.clear();
        game.Draw(worldMousePosition, window);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    /*sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    
    const std::array<std::string, MAP_HEIGHT> mapSketch = 
    {
        "########",
        "#       ",
        "# ##   #",
        "# #L#  #",
        "# ###  #",
        "#      #",
        "#      #",
        "########"
    };
    std::array<TestTiles, MAP_WIDTH * MAP_HEIGHT> testMap;

#pragma region init testMap
    for(int y = 0; y < MAP_HEIGHT; y++)
    {
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            testMap[y * MAP_WIDTH + x].position = {(x * TILE_SIZE) + BUFFOR * 2 , (y * TILE_SIZE) + BUFFOR};
        }
    }
    convertSketch(testMap, mapSketch);
#pragma endregion

    Update(testMap, testMap[ 3 * MAP_WIDTH + 3]);
    while(window.isOpen())
    {
        sf::Event evnt{};
        while (window.pollEvent(evnt))
        {
            if(evnt.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        window.clear(sf::Color::White);
        for(auto &tile : testMap)
        {
            tile.Draw(window);
        }
        window.display();
    }*/
}