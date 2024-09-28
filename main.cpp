#include "Map.hpp"
#include <imgui.h>
#include <imgui-SFML.h>
#include "Math.hpp"
#include <cstdlib> 
#include <ctime>   

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    sf::View view({0.0f, 0.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT});
    ImGui::SFML::Init(window);

    Map map;
    LoadMap(map.tiles);
    std::srand((unsigned int)std::time(NULL));

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
                SaveMap(map.tiles);
                window.close();
            }
        }
        map.Update(deltaTime);
        ImGui::SFML::Update(window, clock);
        ImGui::Begin("player info");
        ImGui::Text("Can player jump: %s" , map.player.canJump? "True" : "False");
        ImGui::Text("Player velocity %i, %i", (int)map.player.velocity.x, (int)map.player.velocity.y);
        ImGui::Text("Player position %i, %i", (int)map.player.position.x, (int)map.player.position.y);
        float fps = 1.0f / deltaTime;
        ImGui::Text("FPS %u", (unsigned)fps);
        ImGui::End();
        window.clear();
        view.setCenter(map.player.position);
        window.setView(view);
        map.Draw(window);
        ImGui::SFML::Render(window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}