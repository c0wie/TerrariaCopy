#include "Item.hpp"

Item::Item(short type)
{
    SetItemProperties(type);
}

void Item::Draw(Vector2 position, bool isInHand, sf::RenderWindow &window) const
{
    sf::Color bColor = isInHand? sf::Color::Yellow : sf::Color::Black; 
    sf::RectangleShape background({32, 32});
    background.setOrigin(background.getSize() / 2.0f);
    background.setPosition(position);
    background.setFillColor(bColor);

    sf::Sprite item;
    item.setPosition(position);
    item.setOrigin(txt->getSize().x / 2.0f, txt->getSize().y / 2.0f);
    item.setTexture(*txt);

    window.draw(background);
    window.draw(item);
}

void Item::SetItemProperties(short type)
{
    this->type = type;
    if(IsNone())
    {
        maxStackSize = 0;
        damage = 0.0f;
    }
    else if(IsBlock())
    {
        maxStackSize = 127;
        damage = 0;
    }
    else if(type == SWORD)
    {
        maxStackSize = 0;
        damage = 100.0f;
    }
    else if(type == PICKAXE)
    {
        maxStackSize = 0;
        damage = 50.0f;
    }
    else if(type == AXE)
    {
        maxStackSize = 0;
        damage = 30.0f;
    }
    LoadTexture();
}

bool Item::IsWeapon() const
{
    return type == SWORD;
}

bool Item::IsBlock() const
{
    return (!IsTool() && !IsWeapon());
}

bool Item::IsTool() const
{
    return type == PICKAXE || type == AXE ;
}

bool Item::IsNone() const 
{
    return type == NONE;
}

void Item::LoadTexture()
{
    // no silver texture for now
    if(type == SILVER)
    {
        return;
    }
    if( !txt->loadFromFile("resources/Items/Item_" + std::to_string(type) + ".png") )
    {
        std::cout << "Unable to load texture from file" << "Item_" + std::to_string(type) + ".png\n"; 
        std::exit(1);
    }

}