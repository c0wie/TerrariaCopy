#include <iostream>
#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "Global.hpp"
#include "Player.hpp"
#include "Map.hpp"
#include "Collision.hpp"
#include "Math.hpp"

std::vector<Rect> m_Rects;
void DrawLine( sf::RenderWindow &window, Vector2 startPoint, Vector2 endPoint, sf::Color color)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0].position = startPoint;
    line[0].color = color;
    line[1].position = endPoint;
    line[1].color = color;
    window.draw(line);
}

void DrawPoint(sf::RenderWindow &window, Vector2 position, sf::Color color)
{
    sf::CircleShape circle(5.0f);
    circle.setPosition(position);
    circle.setOrigin(circle.getRadius(), circle.getRadius());
    circle.setFillColor(color);
    circle.setOutlineThickness(0.8f);
    circle.setOutlineColor(sf::Color::Black);
    window.draw(circle);
}

void DrawRectangle(sf::RenderWindow &window, Rect rect, sf::Color color)
{
    sf::RectangleShape r(rect.size);
    r.setPosition(rect.pos);
    r.setOrigin(r.getSize() / 2.0f);
    r.setFillColor(color);
    window.draw(r);
}

/*void OnUserUpdate(float deltaTime, sf::RenderWindow &window)
{
    Vector2 mousePos = { (float)sf::Mouse::getPosition(window).x, (float)sf::Mouse::getPosition(window).y };
    // Rect box = { {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f}, {100.0f, 100.0f}};
    Rect &player = m_Rects[0];
    Vector2 rayDirection = mousePos - player.pos;
    Vector2 cp, cn;
    float ct;

    std::vector<std::pair<int, float >> z;


    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        player.velocity += Normalize(rayDirection) * 100.0f * deltaTime;
    }

    // order of collisions
    for(int i = 1; i < m_Rects.size(); i++)
    {
        if(RectDynamicRectCollision(player, m_Rects[i], cp, cn, ct, deltaTime))
        {
            z.emplace_back(i, ct);
        }
    }
    // closest collisions first
    std::sort(z.begin(), z.end(), [](const std::pair<int, float> &a, const std::pair<int, float> &b)
        {
            return a.second < b.second;
        });
    // collision resolution
    for(auto j : z)
    {
        if(RectDynamicRectCollision(player, m_Rects[j.first], cp, cn, ct, deltaTime))
        {
            player.velocity += cn * Vector2(abs(player.velocity)) * (1.0f - ct);
        }
    }
    player.pos += player.velocity * deltaTime;

    window.clear();
    // DrawLine(window, rayPoint, mousePos, sf::Color::Red);
    // if(RayRectCollison(rayPoint, rayDirection, box, cp, cn, time) && time < 1.0f)
    // {
    //     DrawRectangle(window, box, sf::Color::Red);
    //     DrawPoint(window, cp, sf::Color::Magenta);
    //     DrawLine(window, cp, {cp + cn * 100.0f}, sf::Color::Blue);
    // }
    // else
    // {
    //     DrawRectangle(window, box, sf::Color::White);
    // }

    for(const auto &r : m_Rects)
    {
        DrawRectangle(window, r, sf::Color::White);
    }
}*/

void OnUserCreate()
{
    m_Rects.emplace_back(Vector2{10.0f, 10.0f}, Vector2{100.0f, 100.0f});
    m_Rects.emplace_back(Vector2{400.0f, 400.0f}, Vector2{300.0f, 100.0f});
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "TerrariaCopy", sf::Style::Titlebar | sf::Style::Close);
    sf::View view({0.0f, 0.0f}, {SCREEN_WIDTH, SCREEN_HEIGHT});
    ImGui::SFML::Init(window);

    Map map;

    sf::Clock DT_Clock;
    float deltaTime = 0.0f;
    DT_Clock.restart();

    OnUserCreate();
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
        window.clear();
        view.setCenter(map.player.position);
        window.setView(view);
        ImGui::SFML::Render(window);
        map.Draw(window);


        // OnUserUpdate(deltaTime, window);
        window.display();
    }
    ImGui::SFML::Shutdown();
}