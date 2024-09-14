#include <iostream>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Global.hpp"
#include "Player.hpp"
#include "Map.hpp"


int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    sf::View view({0.0f, 0.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT});
    ImGui::SFML::Init(window);

    Map map;

    sf::Clock DT_Clock;
    float deltaTime = 0.0f;
    DT_Clock.restart();
    while(window.isOpen())
    {
        deltaTime = DT_Clock.restart().asSeconds();
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
        map.Update(deltaTime);
        ImGui::SFML::Update(window, clock);
        view.setCenter(map.GetPlayer().GetPosition());
        window.clear(sf::Color::Blue);
        window.setView(view);
        ImGui::SFML::Render(window);
        map.Draw(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}