/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012-2013 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <SFML/OpenGL.hpp>

#include <TGUI/Panel.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel() :
    m_Size                         (100, 100),
    m_BackgroundColor              (sf::Color::Transparent),
    m_Texture                      (nullptr)
    {
        m_Callback.widgetType = Type_Panel;
        m_AllowFocus = true;
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::Panel(const Panel& panelToCopy) :
    Container        (panelToCopy),
    m_Size           (panelToCopy.m_Size),
    m_BackgroundColor(panelToCopy.m_BackgroundColor),
    m_Texture        (panelToCopy.m_Texture)
    {
        if (m_Texture)
        {
            m_Sprite.setTexture(*m_Texture);
            m_Sprite.setScale(m_Size.x / m_Texture->getSize().x, m_Size.y / m_Texture->getSize().y);
            m_Sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel::~Panel()
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel& Panel::operator= (const Panel& right)
    {
        // Make sure it is not the same widget
        if (this != &right)
        {
            Panel temp(right);
            this->Container::operator=(right);

            std::swap(m_Size,                          temp.m_Size);
            std::swap(m_BackgroundColor,               temp.m_BackgroundColor);
            std::swap(m_Texture,                       temp.m_Texture);
            std::swap(m_Sprite,                        temp.m_Sprite);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Panel* Panel::clone()
    {
        return new Panel(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setSize(float width, float height)
    {
        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // Set the size of the panel
        m_Size.x = width;
        m_Size.y = height;

        // If there is a background texture then resize it
        if (m_Texture)
            m_Sprite.setScale(m_Size.x / m_Texture->getSize().x, m_Size.y / m_Texture->getSize().y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f Panel::getSize() const
    {
        return sf::Vector2f(m_Size.x, m_Size.y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setBackgroundTexture(sf::Texture *const texture)
    {
        // Store the texture
        m_Texture = texture;

        // Set the texture for the sprite
        if (m_Texture)
        {
            m_Sprite.setTexture(*m_Texture, true);
            m_Sprite.setScale(m_Size.x / m_Texture->getSize().x, m_Size.y / m_Texture->getSize().y);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Texture* Panel::getBackgroundTexture()
    {
        return m_Texture;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_BackgroundColor = backgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Panel::getBackgroundColor() const
    {
        return m_BackgroundColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::setTransparency(unsigned char transparency)
    {
        Container::setTransparency(transparency);

        m_BackgroundColor.a = m_Opacity;

        m_Sprite.setColor(sf::Color(255, 255, 255, m_Opacity));
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::mouseOnWidget(float x, float y)
    {
        // Don't continue when the panel has not been loaded yet
        if (m_Loaded == false)
            return false;

        // Check if the mouse is inside the panel
        if (getTransform().transformRect(sf::FloatRect(0, 0, m_Size.x, m_Size.y)).contains(x, y))
            return true;
        else
        {
            if (m_MouseHover)
                mouseLeftWidget();

            // Tell the widgets inside the panel that the mouse is no longer on top of them
            m_EventManager.mouseNotOnWidget();
            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMousePressed(float x, float y)
    {
        if (mouseOnWidget(x, y))
        {
            m_MouseDown = true;

            if (!m_CallbackFunctions[LeftMousePressed].empty())
            {
                m_Callback.trigger = LeftMousePressed;
                m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }
        }

        Container::leftMousePressed(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::leftMouseReleased(float x , float y)
    {
        if (mouseOnWidget(x, y))
        {
            if (!m_CallbackFunctions[LeftMouseReleased].empty())
            {
                m_Callback.trigger = LeftMouseReleased;
                m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
                m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
                addCallback();
            }

            if (m_MouseDown)
            {
                if (!m_CallbackFunctions[LeftMouseClicked].empty())
                {
                    m_Callback.trigger = LeftMouseClicked;
                    m_Callback.mouse.x = static_cast<int>(x - getPosition().x);
                    m_Callback.mouse.y = static_cast<int>(y - getPosition().y);
                    addCallback();
                }
            }
        }

        m_MouseDown = false;

        Container::leftMouseReleased(x, y);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::widgetFocused()
    {
        m_EventManager.tabKeyPressed();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::widgetUnfocused()
    {
        m_EventManager.unfocusAllWidgets();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::setProperty(const std::string& property, const std::string& value)
    {
        if (!Widget::setProperty(property, value))
        {
            if (property == "BackgroundColor")
            {
                setBackgroundColor(extractColor(value));
            }
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Panel::getProperty(const std::string& property, std::string& value)
    {
        if (!Widget::getProperty(property, value))
        {
            if (property == "BackgroundColor")
                value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Panel::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the texture wasn't created
        if (m_Loaded == false)
            return;

        // Calculate the scale factor of the view
        float scaleViewX = target.getSize().x / target.getView().getSize().x;
        float scaleViewY = target.getSize().y / target.getView().getSize().y;

        // Get the global position
        sf::Vector2f topLeftPosition = states.transform.transformPoint(getPosition() - target.getView().getCenter() + (target.getView().getSize() / 2.f));
        sf::Vector2f bottomRightPosition = states.transform.transformPoint(getPosition() + m_Size - target.getView().getCenter() + (target.getView().getSize() / 2.f));

        // Get the old clipping area
        GLint scissor[4];
        glGetIntegerv(GL_SCISSOR_BOX, scissor);

        // Calculate the clipping area
        GLint scissorLeft = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.x * scaleViewX), scissor[0]);
        GLint scissorTop = TGUI_MAXIMUM(static_cast<GLint>(topLeftPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1] - scissor[3]);
        GLint scissorRight = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.x * scaleViewX), scissor[0] + scissor[2]);
        GLint scissorBottom = TGUI_MINIMUM(static_cast<GLint>(bottomRightPosition.y * scaleViewY), static_cast<GLint>(target.getSize().y) - scissor[1]);

        // If the widget outside the window then don't draw anything
        if (scissorRight < scissorLeft)
            scissorRight = scissorLeft;
        else if (scissorBottom < scissorTop)
            scissorTop = scissorBottom;

        // Set the clipping area
        glScissor(scissorLeft, target.getSize().y - scissorBottom, scissorRight - scissorLeft, scissorBottom - scissorTop);

        // Set the transform
        states.transform *= getTransform();

        // Draw the background
        if (m_BackgroundColor != sf::Color::Transparent)
        {
            sf::RectangleShape background(m_Size);
            background.setFillColor(m_BackgroundColor);
            target.draw(background, states);
        }

        // Draw the background texture if there is one
        if (m_Texture)
            target.draw(m_Sprite, states);

        // Draw the widgets
        drawWidgetContainer(&target, states);

        // Reset the old clipping area
        glScissor(scissor[0], scissor[1], scissor[2], scissor[3]);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
