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

        map.Update(deltaTime);
        ImGui::SFML::Update(window, clock);
        ImGui::Begin("player info");
        ImGui::Text("Can player jump: %s" , map.player.canJump? "True" : "False");
        ImGui::Text("Player velocity %i, %i", (int)map.player.velocity.x, (int)map.player.velocity.y);
        ImGui::Text("Player position %i, %i", (int)map.player.position.x, (int)map.player.position.y);
        ImGui::End();
        view.setCenter(map.player.position);
        window.clear(sf::Color::Blue);
        window.setView(view);
        ImGui::SFML::Render(window);
        map.Draw(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}