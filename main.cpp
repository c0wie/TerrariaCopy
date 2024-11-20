#include "Global.hpp"
#include "Game.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdlib> 
#include <ctime>   
#include <stack>
#include "Vector2.h"
#include "PerlinNoise.hpp"
#include "Math.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    sf::View view({0.0f, 0.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT});
    ImGui::SFML::Init(window);

    std::srand((unsigned int)std::time(NULL));
    Player::loadTextures();
    Tile::LoadTextures();
    Item::LoadTextures();
    Item::InitBackground();
    Game::InitBackground();
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
        view.setCenter(game.GetPlayerPosition());
        window.setView(view);
        ImGui::SFML::Update(window, clock);
        game.Update(worldMousePosition, windowCenter, evnt, deltaTime);
        window.clear();
        game.Draw(worldMousePosition, window);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    /*
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    std::srand((unsigned int)std::time(NULL));
    ImGui::SFML::Init(window);

    constexpr int CAVE_WIDTH = 300;
    constexpr int CAVE_HEIGHT = 300;

    int octaves = 5;
    int bias = 1;
    int octaves2 = 5;
    int bias2 = 2;
    auto randomNumbers = GenerateRandomVector<CAVE_WIDTH * CAVE_HEIGHT>(0.0f, 1.0f);
    auto randomNumbers2 = GenerateRandomVector<CAVE_WIDTH * CAVE_HEIGHT>(0.0f, 1.0f);
    std::vector<float> seed = PerlinNoise2D<CAVE_WIDTH, CAVE_HEIGHT>(randomNumbers, octaves, bias);
    std::vector<float> seed2 = PerlinNoise2D<CAVE_WIDTH, CAVE_HEIGHT>(randomNumbers2, octaves2, bias2);
    auto sortedSeed = seed;
    std::sort(sortedSeed.begin(), sortedSeed.end());
    auto sortedSeed2 = seed2;
    std::sort(sortedSeed2.begin(), sortedSeed2.end());

    std::vector<sf::RectangleShape> cells;
    cells.resize(CAVE_WIDTH * CAVE_HEIGHT);

    auto GetColor1 = [](float noiseValue, auto &sortedSeed)
    {
        if(noiseValue >= sortedSeed[0.45f * sortedSeed.size()] && noiseValue <= sortedSeed[0.55f * sortedSeed.size()])
        {
            return sf::Color::Green;
        }
        sf::Uint8 baseColor = 255 * noiseValue;
        return sf::Color{baseColor, baseColor, baseColor};
    };
    auto GetColor2 = [](float noiseValue, auto &sortedSeed)
    {
        if(noiseValue > sortedSeed[0.95f * sortedSeed.size()])
        {
            return sf::Color::Green;
        }
        sf::Uint8 baseColor = 255 * noiseValue;
        return sf::Color{baseColor, baseColor, baseColor};
    };
    
    for(int x = 0; x < CAVE_WIDTH; x++)
    {
        for(int y = 0; y < CAVE_HEIGHT; y++)
        {
            sf::RectangleShape &cell = cells[y * CAVE_WIDTH + x];
            cell.setSize({SCREEN_WIDTH / CAVE_WIDTH, SCREEN_HEIGHT / CAVE_HEIGHT});
            cell.setPosition(x * cell.getSize().x, y * cell.getSize().y);
            cell.setOrigin(cell.getSize() / 2.0f);
            cell.setFillColor(GetColor1(seed[y * CAVE_WIDTH + x], sortedSeed));
            if(cell.getFillColor() != sf::Color::Green)
            {
                cell.setFillColor(GetColor2(seed[y * CAVE_WIDTH + x], sortedSeed2));
            }
        }
    }

    bool smthChanged = false;
    sf::Clock DT_Clock;
    float deltaTime = 0.0f;
    DT_Clock.restart();
    while(window.isOpen())
    {
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
                window.close();
            }
        }
        
        ImGui::SFML::Update(window, clock);
        ImGui::Begin("Noise properties");
            ImGui::Text("octaves1: %i", octaves);
            ImGui::SameLine();
            if( ImGui::Button("octaves1") )
            {
                if(octaves <= 5)
                {
                    octaves++;
                }
                else
                {
                    octaves = 0;
                }
                smthChanged = true;
            }
            ImGui::Text("bias1: %i", bias);
            ImGui::SameLine();
            if( ImGui::Button("bias1") )
            {
                if(bias <= 5)
                {
                    bias++;
                }
                else
                {
                    bias = 0;
                }
                smthChanged = true;
            }
            ImGui::Text("octaves2: %i", octaves2);
            ImGui::SameLine();
            if( ImGui::Button("octaves2") )
            {
                if(octaves2 <= 5)
                {
                    octaves2++;
                }
                else
                {
                    octaves2 = 0;
                }
                smthChanged = true;
            }
            ImGui::Text("bias2: %i", bias2);
            ImGui::SameLine();
            if( ImGui::Button("bias2") )
            {
                if(bias2 <= 5)
                {
                    bias2++;
                }
                else
                {
                    bias2 = 0;
                }
                smthChanged = true;
            }
        ImGui::End();
        if(smthChanged)
        {
            seed = PerlinNoise2D<CAVE_WIDTH, CAVE_HEIGHT>(randomNumbers, octaves, bias);
            sortedSeed = seed;
            std::sort(sortedSeed.begin(), sortedSeed.end());

            seed2 = PerlinNoise2D<CAVE_WIDTH, CAVE_HEIGHT>(randomNumbers2, octaves2, bias2);
            sortedSeed2 = seed2;
            std::sort(sortedSeed2.begin(), sortedSeed2.end());
            for(int x = 0; x < CAVE_WIDTH; x++)
            {
                for(int y = 0; y < CAVE_HEIGHT; y++)
                {
                    cells[y * CAVE_WIDTH + x].setFillColor(GetColor1(seed[y * CAVE_WIDTH + x], sortedSeed));
                    if(cells[y * CAVE_WIDTH + x].getFillColor() != sf::Color::Green)
                    {
                        cells[y * CAVE_WIDTH + x].setFillColor(GetColor2(seed2[y * CAVE_WIDTH + x], sortedSeed2));
                    }
                }
            }
            smthChanged = false;
        }   
        window.clear();
        for(int x = 0; x < CAVE_WIDTH; x++)
        {
            for(int y = 0; y < CAVE_HEIGHT; y++)
            {
                window.draw(cells[y * CAVE_WIDTH + x]);
            }
        }
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
    */
}