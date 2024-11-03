#include "Global.hpp"
#include "Game.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdlib> 
#include <ctime>   

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    sf::View view({0.0f, 0.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT});
    ImGui::SFML::Init(window);

    Game game;
    game.Init();
    std::srand((unsigned int)std::time(NULL));

    sf::Clock DT_Clock;
    float deltaTime = 0.0f;
    DT_Clock.restart();
    while(window.isOpen())
    {
        const Vector2 mousePosition = sf::Mouse::getPosition(window);
        const Vector2 windowCenter = window.getView().getCenter();
        const Vector2 worldMousePosition = window.mapPixelToCoords(mousePosition);
        deltaTime = DT_Clock.restart().asSeconds();
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
    // sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    // LoadItemTextures();
    // Item::InitBackground();
    // Item items[2] = 
    // {
    //     Item{Item::PICKAXE},
    //     Item{Item::GRASS}
    // };
    // while(window.isOpen())
    // {
    //     sf::Event evnt{};
    //     while (window.pollEvent(evnt))
    //     {
    //         if(evnt.type == sf::Event::Closed)
    //         {
    //             window.close();
    //         }
    //     }
    //     window.clear(sf::Color::White);
    //     items[0].Draw({SCREEN_WIDTH / 2.0f , SCREEN_HEIGHT / 2.0f}, false, false, window);
    //     items[1].Draw({SCREEN_WIDTH / 2.0f + 100.0f, SCREEN_HEIGHT / 2.0f}, false, false, window);
    //     window.display();
    // }
}