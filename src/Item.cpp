#include "Item.hpp"

std::array<ItemProperties, Item::ITEM_COUNT> loadedItems = 
{
    //            ID  Size  Dmg
    ItemProperties{0,  0,   0  },  //NONE
    ItemProperties{1,  128, 0  },  //STONE
    ItemProperties{2,  128, 0  },  //GRASS
    ItemProperties{3,  128, 0  },  //LOG
    ItemProperties{4,  128, 0  },  //IRON
    ItemProperties{5,  128, 0  },  //COPPER
    ItemProperties{6,  128, 0  },  //GOLD
    ItemProperties{7,  128, 0  },  //SILVER
    ItemProperties{12, 128, 0  },  //TORCH
    ItemProperties{9,  128, 5000 },  //PICKAXE
    ItemProperties{10, 128, 100},  //SWORD
    ItemProperties{11, 128, 30 },   //AXE
};

sf::RectangleShape Item::background{{32, 32}};

Item::Item(short Type, int count)
{
    SetProperties(Type, count);
}

void Item::Draw(Vector2 position, bool isActive, bool isInHand, sf::RenderWindow &window) const
{
    if(!isInHand)
    {
        const sf::Color bColor = isActive? sf::Color::Yellow : sf::Color::Black; 
        background.setPosition(position);
        background.setFillColor(bColor);
        window.draw(background);
    }
    sprite->setPosition(position);
    window.draw(*sprite);
}

void Item::SetProperties(short Type, int count)
{
    type = Type;
    if(Type == NONE)
    {
        currentStackSize = 0;
    }   
    currentStackSize = count;
    maxStackSize = loadedItems[Type].maxStackSize;
    damage = loadedItems[Type].damage;
    sprite->setTexture(loadedItems[Type].txt);
    const Vector2 size = {loadedItems[Type].txt.getSize().x, loadedItems[Type].txt.getSize().y};
    sprite->setOrigin(size / 2.0f);
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

void Item::InitBackground()
{
    background.setOrigin(background.getSize() / 2.0f);
    background.setOutlineThickness(3.0f);
    background.setOutlineColor(sf::Color(163, 161, 140));
}

void Item::LoadTextures()
{
    for(int i = 0; i < Item::ITEM_COUNT; i++)
    {
        if(i == Item::SILVER )
        {
            continue;
        }
        const std::string filename = "resources/Items/Item_" + std::to_string(loadedItems[i].txtID) + ".png";
        loadedItems[i].txt.loadFromFile(filename);
    }
}