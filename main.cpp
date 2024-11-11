#include "Global.hpp"
#include "Game.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include <cstdlib> 
#include <ctime>   
#include <stack>
#include "Vector2.h"

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
}