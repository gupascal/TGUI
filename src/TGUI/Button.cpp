/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus's Graphical User Interface
// Copyright (C) 2012 Bruno Van de Velde (vdv_b@tgui.eu)
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


#include <TGUI/Objects.hpp>
#include <TGUI/ClickableObject.hpp>
#include <TGUI/Button.hpp>

#include <cmath>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button() :
    m_TextureNormal_L    (NULL),
    m_TextureMouseHover_L(NULL),
    m_TextureMouseDown_L (NULL),
    m_TextureFocused_L   (NULL),
    m_TextureNormal_M    (NULL),
    m_TextureMouseHover_M(NULL),
    m_TextureMouseDown_M (NULL),
    m_TextureFocused_M   (NULL),
    m_TextureNormal_R    (NULL),
    m_TextureMouseHover_R(NULL),
    m_TextureMouseDown_R (NULL),
    m_TextureFocused_R   (NULL),
    m_LoadedPathname     (""),
    m_SplitImage         (false),
    m_SeparateHoverImage (false),
    m_TextSize           (0)
    {
        m_Callback.objectType = Type_Button;
        m_Text.setColor(sf::Color::Black);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::Button(const Button& copy) :
    ClickableObject     (copy),
    m_LoadedPathname    (copy.m_LoadedPathname),
    m_SplitImage        (copy.m_SplitImage),
    m_SeparateHoverImage(copy.m_SeparateHoverImage),
    m_Text              (copy.m_Text),
    m_TextSize          (copy.m_TextSize)
    {
        // Copy the textures
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_L, m_TextureNormal_L))         m_SpriteNormal_L.setTexture(*m_TextureNormal_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_M, m_TextureNormal_M))         m_SpriteNormal_M.setTexture(*m_TextureNormal_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureNormal_R, m_TextureNormal_R))         m_SpriteNormal_R.setTexture(*m_TextureNormal_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseHover_L, m_TextureMouseHover_L)) m_SpriteMouseHover_L.setTexture(*m_TextureMouseHover_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseHover_M, m_TextureMouseHover_M)) m_SpriteMouseHover_M.setTexture(*m_TextureMouseHover_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseHover_R, m_TextureMouseHover_R)) m_SpriteMouseHover_R.setTexture(*m_TextureMouseHover_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseDown_L, m_TextureMouseDown_L))   m_SpriteMouseDown_L.setTexture(*m_TextureMouseDown_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseDown_M, m_TextureMouseDown_M))   m_SpriteMouseDown_M.setTexture(*m_TextureMouseDown_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureMouseDown_R, m_TextureMouseDown_R))   m_SpriteMouseDown_R.setTexture(*m_TextureMouseDown_R);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_L, m_TextureFocused_L))       m_SpriteFocused_L.setTexture(*m_TextureFocused_L);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_M, m_TextureFocused_M))       m_SpriteFocused_M.setTexture(*m_TextureFocused_M);
        if (TGUI_TextureManager.copyTexture(copy.m_TextureFocused_R, m_TextureFocused_R))       m_SpriteFocused_R.setTexture(*m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button::~Button()
    {
        // Remove all the textures from memory (if we are the only one who use the textures)

        if (m_TextureNormal_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_R);

        if (m_TextureMouseHover_L != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_L);
        if (m_TextureMouseHover_M != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_M);
        if (m_TextureMouseHover_R != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_R);

        if (m_TextureMouseDown_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_L);
        if (m_TextureMouseDown_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_M);
        if (m_TextureMouseDown_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_R);

        if (m_TextureFocused_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_R);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button& Button::operator= (const Button& right)
    {
        // Make sure it is not the same object
        if (this != &right)
        {
            Button temp(right);
            this->ClickableObject::operator=(right);

            std::swap(m_TextureNormal_L,     temp.m_TextureNormal_L);
            std::swap(m_TextureNormal_M,     temp.m_TextureNormal_M);
            std::swap(m_TextureNormal_R,     temp.m_TextureNormal_R);
            std::swap(m_TextureMouseHover_L, temp.m_TextureMouseHover_L);
            std::swap(m_TextureMouseHover_M, temp.m_TextureMouseHover_M);
            std::swap(m_TextureMouseHover_R, temp.m_TextureMouseHover_R);
            std::swap(m_TextureMouseDown_L,  temp.m_TextureMouseDown_L);
            std::swap(m_TextureMouseDown_M,  temp.m_TextureMouseDown_M);
            std::swap(m_TextureMouseDown_R,  temp.m_TextureMouseDown_R);
            std::swap(m_TextureFocused_L,    temp.m_TextureFocused_L);
            std::swap(m_TextureFocused_M,    temp.m_TextureFocused_M);
            std::swap(m_TextureFocused_R,    temp.m_TextureFocused_R);
            std::swap(m_SpriteNormal_L,      temp.m_SpriteNormal_L);
            std::swap(m_SpriteNormal_M,      temp.m_SpriteNormal_M);
            std::swap(m_SpriteNormal_R,      temp.m_SpriteNormal_R);
            std::swap(m_SpriteMouseHover_L,  temp.m_SpriteMouseHover_L);
            std::swap(m_SpriteMouseHover_M,  temp.m_SpriteMouseHover_M);
            std::swap(m_SpriteMouseHover_R,  temp.m_SpriteMouseHover_R);
            std::swap(m_SpriteMouseDown_L,   temp.m_SpriteMouseDown_L);
            std::swap(m_SpriteMouseDown_M,   temp.m_SpriteMouseDown_M);
            std::swap(m_SpriteMouseDown_R,   temp.m_SpriteMouseDown_R);
            std::swap(m_SpriteFocused_L,     temp.m_SpriteFocused_L);
            std::swap(m_SpriteFocused_M,     temp.m_SpriteFocused_M);
            std::swap(m_SpriteFocused_R,     temp.m_SpriteFocused_R);
            std::swap(m_LoadedPathname,      temp.m_LoadedPathname);
            std::swap(m_SplitImage,          temp.m_SplitImage);
            std::swap(m_SeparateHoverImage,  temp.m_SeparateHoverImage);
            std::swap(m_Text,                temp.m_Text);
            std::swap(m_TextSize,            temp.m_TextSize);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Button* Button::clone()
    {
        return new Button(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Button::load(const std::string& pathname)
    {
        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // Make sure that the pathname isn't empty
        if (pathname.empty())
            return false;

        // Store the pathname
        m_LoadedPathname = pathname;

        // When the pathname does not end with a "/" then we will add it
        if (m_LoadedPathname[m_LoadedPathname.length()-1] != '/')
            m_LoadedPathname.push_back('/');

        // Open the info file
        InfoFileParser infoFile;
        if (infoFile.openFile(m_LoadedPathname + "info.txt") == false)
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + m_LoadedPathname + "info.txt");
            return false;
        }

        std::string property;
        std::string value;

        // Set some default settings
        m_SplitImage = false;
        m_SeparateHoverImage = true;
        std::string imageExtension = "png";

        // Read untill the end of the file
        while (infoFile.readProperty(property, value))
        {
            // Check what the property is
            if (property.compare("splitimage") == 0)
            {
                if ((value.compare("true") == 0) || (value.compare("1") == 0))
                    m_SplitImage = true;
                else
                {
                    if ((value.compare("false") != 0) && (value.compare("0") != 0))
                        TGUI_OUTPUT("TGUI warning: Wrong value passed to SplitImage: \"" + value + "\".");
                }
            }
            else if (property.compare("separatehoverimage") == 0)
            {
                if ((value.compare("false") == 0) || (value.compare("0") == 0))
                    m_SeparateHoverImage = false;
                else
                {
                    if ((value.compare("true") != 0) && (value.compare("1") != 0))
                        TGUI_OUTPUT("TGUI warning: Wrong value passed to SeparateHoverImage: \"" + value + "\".");
                }
            }
            else if (property.compare("phases") == 0)
            {
                // Get and store the different phases
                extractPhases(value);
            }
            else if (property.compare("textcolor") == 0)
            {
                m_Text.setColor(extractColor(value));
            }
            else if (property.compare("extension") == 0)
            {
                imageExtension = value;
            }
        }

        // Close the info file
        infoFile.closeFile();

        // If the button was loaded before then remove the old textures first
        if (m_TextureNormal_L != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_L);
        if (m_TextureNormal_M != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_M);
        if (m_TextureNormal_R != NULL)     TGUI_TextureManager.removeTexture(m_TextureNormal_R);
        if (m_TextureMouseHover_L != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_L);
        if (m_TextureMouseHover_M != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_M);
        if (m_TextureMouseHover_R != NULL) TGUI_TextureManager.removeTexture(m_TextureMouseHover_R);
        if (m_TextureMouseDown_L != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_L);
        if (m_TextureMouseDown_M != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_M);
        if (m_TextureMouseDown_R != NULL)  TGUI_TextureManager.removeTexture(m_TextureMouseDown_R);
        if (m_TextureFocused_L != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_L);
        if (m_TextureFocused_M != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_M);
        if (m_TextureFocused_R != NULL)    TGUI_TextureManager.removeTexture(m_TextureFocused_R);

        // Check if the image is split
        if (m_SplitImage)
        {
            // Load the required texture
            if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Normal." + imageExtension, m_TextureNormal_L))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Normal." + imageExtension, m_TextureNormal_M))
             && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Normal." + imageExtension, m_TextureNormal_R)))
            {
                m_SpriteNormal_L.setTexture(*m_TextureNormal_L, true);
                m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
                m_SpriteNormal_R.setTexture(*m_TextureNormal_R, true);

                // Set the size of the button
                m_Size.x = static_cast<float>(m_TextureNormal_L->getSize().x + m_TextureNormal_M->getSize().x + m_TextureNormal_R->getSize().x);
                m_Size.y = static_cast<float>(m_TextureNormal_M->getSize().y);
            }
            else
                return false;

            // Load the optional textures
            if (m_ObjectPhase & ObjectPhase_Focused)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Focus." + imageExtension, m_TextureFocused_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Focus." + imageExtension, m_TextureFocused_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Focus." + imageExtension, m_TextureFocused_R)))
                {
                    m_SpriteFocused_L.setTexture(*m_TextureFocused_L, true);
                    m_SpriteFocused_M.setTexture(*m_TextureFocused_M, true);
                    m_SpriteFocused_R.setTexture(*m_TextureFocused_R, true);

                    m_AllowFocus = true;
                }
                else
                    return false;
            }

            if (m_ObjectPhase & ObjectPhase_Hover)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Hover." + imageExtension, m_TextureMouseHover_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Hover." + imageExtension, m_TextureMouseHover_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Hover." + imageExtension, m_TextureMouseHover_R)))
                {
                    m_SpriteMouseHover_L.setTexture(*m_TextureMouseHover_L, true);
                    m_SpriteMouseHover_M.setTexture(*m_TextureMouseHover_M, true);
                    m_SpriteMouseHover_R.setTexture(*m_TextureMouseHover_R, true);
                }
                else
                    return false;
            }

            if (m_ObjectPhase & ObjectPhase_MouseDown)
            {
                if ((TGUI_TextureManager.getTexture(m_LoadedPathname + "L_Down." + imageExtension, m_TextureMouseDown_L))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "M_Down." + imageExtension, m_TextureMouseDown_M))
                    && (TGUI_TextureManager.getTexture(m_LoadedPathname + "R_Down." + imageExtension, m_TextureMouseDown_R)))
                {
                    m_SpriteMouseDown_L.setTexture(*m_TextureMouseDown_L, true);
                    m_SpriteMouseDown_M.setTexture(*m_TextureMouseDown_M, true);
                    m_SpriteMouseDown_R.setTexture(*m_TextureMouseDown_R, true);
                }
                else
                    return false;
            }
        }
        else // The image isn't split
        {
            // Load the required texture
            if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Normal." + imageExtension, m_TextureNormal_M))
            {
                m_SpriteNormal_M.setTexture(*m_TextureNormal_M, true);
                m_Size = Vector2f(m_TextureNormal_M->getSize());
            }
            else
                return false;

            // Load the optional textures
            if (m_ObjectPhase & ObjectPhase_Focused)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Focus." + imageExtension, m_TextureFocused_M))
                {
                    m_SpriteFocused_M.setTexture(*m_TextureFocused_M, true);
                    m_AllowFocus = true;
                }
                else
                    return false;
            }

            if (m_ObjectPhase & ObjectPhase_Hover)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Hover." + imageExtension, m_TextureMouseHover_M))
                    m_SpriteMouseHover_M.setTexture(*m_TextureMouseHover_M, true);
                else
                    return false;
            }

            if (m_ObjectPhase & ObjectPhase_MouseDown)
            {
                if (TGUI_TextureManager.getTexture(m_LoadedPathname + "Down." + imageExtension, m_TextureMouseDown_M))
                    m_SpriteMouseDown_M.setTexture(*m_TextureMouseDown_M, true);
                else
                    return false;
            }
        }

        // When there is no error we will return true
        m_Loaded = true;
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setSize(float width, float height)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new size of the button
        m_Size.x = width;
        m_Size.y = height;

        // Recalculate the text size when auto sizing
        if (m_TextSize == 0)
            setText(m_Text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::string Button::getLoadedPathname() const
    {
        return m_LoadedPathname;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setText(const sf::String& text)
    {
        // Don't do anything when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the new text
        m_Text.setString(text);

        // Check if the text is auto sized
        if (m_TextSize == 0)
        {
            // Calculate a possible text size
            float size = m_Size.y * 0.85f;
            m_Text.setCharacterSize(static_cast<unsigned int>(size));
            m_Text.setCharacterSize(static_cast<unsigned int>(m_Text.getCharacterSize() - m_Text.getLocalBounds().top));

            // Make sure that the text isn't too width
            if (m_Text.getGlobalBounds().width > (m_Size.x * 0.8f))
            {
                // The text is too width, so make it smaller
                m_Text.setCharacterSize(static_cast<unsigned int>(size * m_Size.x * 0.8f / m_Text.getGlobalBounds().width));
                m_Text.setCharacterSize(static_cast<unsigned int>(m_Text.getCharacterSize() - m_Text.getLocalBounds().top));
            }
        }
        else // When the text has a fixed size
        {
            // Set the text size
            m_Text.setCharacterSize(m_TextSize);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::String Button::getText() const
    {
        return m_Text.getString();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextFont(const sf::Font& font)
    {
        m_Text.setFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* Button::getTextFont() const
    {
        return m_Text.getFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextColor(const sf::Color& color)
    {
        m_Text.setColor(color);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& Button::getTextColor() const
    {
        return m_Text.getColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::setTextSize(unsigned int size)
    {
        // Change the text size
        m_TextSize = size;

        // Call setText to reposition the text
        setText(m_Text.getString());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int Button::getTextSize() const
    {
        return m_Text.getCharacterSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::keyPressed(sf::Keyboard::Key key)
    {
        // Check if the space key or the return key was pressed
        if (key == sf::Keyboard::Space)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[SpaceKeyPressed].empty() == false)
            {
                m_Callback.trigger = SpaceKeyPressed;
                addCallback();
            }
        }
        else if (key == sf::Keyboard::Return)
        {
            // Add the callback (if the user requested it)
            if (m_CallbackFunctions[ReturnKeyPressed].empty() == false)
            {
                m_Callback.trigger = ReturnKeyPressed;
                addCallback();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::objectFocused()
    {
        // We can't be focused when we don't have a focus image
        if ((m_ObjectPhase & ObjectPhase_Focused) == 0)
            m_Parent->unfocusObject(this);
        else
            Object::objectFocused();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::initialize(tgui::Group *const parent)
    {
        m_Parent = parent;
        m_Text.setFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Don't draw when the button wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Calculate the scaling
        Vector2f scaling(m_Size.x / m_TextureNormal_M->getSize().x, m_Size.y / m_TextureNormal_M->getSize().y);

        // Apply the transformations
        states.transform *= getTransform();

        // Remember the current transformation
        sf::Transform oldTransform = states.transform;

        // Drawing the button image will be different when the image is split
        if (m_SplitImage)
        {
            // Set the scaling
            states.transform.scale(scaling.y, scaling.y);

            // Draw the left image
            {
                // Check if there is a separate hover image
                if (m_SeparateHoverImage)
                {
                    // Draw the correct image
                    if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                    {
                        target.draw(m_SpriteMouseDown_L, states);
                    }
                    else if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                    {
                        target.draw(m_SpriteMouseHover_L, states);
                    }
                    else
                        target.draw(m_SpriteNormal_L, states);
                }
                else
                {
                    // When the button is down then draw another image
                    if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                        target.draw(m_SpriteMouseDown_L, states);
                    else
                        target.draw(m_SpriteNormal_L, states);

                    // When the mouse is on top of the button then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_SpriteMouseHover_L, states);
                }

                // When the button is focused then draw an extra image
                if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                    target.draw(m_SpriteFocused_L, states);
            }

            // Check if the middle image may be drawn
            if ((scaling.y * (m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x)) < m_Size.x)
            {
                // Calculate the scale for our middle image
                float scaleX = scaling.x - (((m_TextureNormal_L->getSize().x + m_TextureNormal_R->getSize().x) * scaling.y) / m_TextureNormal_M->getSize().x);

                // Put the middle image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

                // Set the scale for the middle image
                states.transform.scale(scaleX / scaling.y, 1);

                // Draw the middle image
                {
                    // Check if there is a separate hover image
                    if (m_SeparateHoverImage)
                    {
                        // Draw the correct image
                        if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                        {
                            target.draw(m_SpriteMouseDown_M, states);
                        }
                        else if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        {
                            target.draw(m_SpriteMouseHover_M, states);
                        }
                        else
                            target.draw(m_SpriteNormal_M, states);
                    }
                    else
                    {
                        // When the button is down then draw another image
                        if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                            target.draw(m_SpriteMouseDown_M, states);
                        else
                            target.draw(m_SpriteNormal_M, states);

                        // When the mouse is on top of the button then draw an extra image
                        if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                            target.draw(m_SpriteMouseHover_M, states);
                    }

                    // When the button is focused then draw an extra image
                    if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                        target.draw(m_SpriteFocused_M, states);
                }

                // Put the right image on the correct position
                states.transform.translate(static_cast<float>(m_TextureNormal_M->getSize().x), 0);

                // Set the scale for the right image
                states.transform.scale(scaling.y / scaleX, 1);
            }
            else // The middle image is not drawn
                states.transform.translate(static_cast<float>(m_TextureNormal_L->getSize().x), 0);

            // Draw the right image
            {
                // Check if there is a separate hover image
                if (m_SeparateHoverImage)
                {
                    // Draw the correct image
                    if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                    {
                        target.draw(m_SpriteMouseDown_R, states);
                    }
                    else if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                    {
                        target.draw(m_SpriteMouseHover_R, states);
                    }
                    else
                        target.draw(m_SpriteNormal_R, states);
                }
                else
                {
                    // When the button is down then draw another image
                    if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                        target.draw(m_SpriteMouseDown_R, states);
                    else
                        target.draw(m_SpriteNormal_R, states);

                    // When the mouse is on top of the button then draw an extra image
                    if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                        target.draw(m_SpriteMouseHover_R, states);
                }

                // When the button is focused then draw an extra image
                if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                    target.draw(m_SpriteFocused_R, states);
            }
        }
        else // The image is not split
        {
            // Draw the button at the correct size
            states.transform.scale(m_Size.x / m_TextureNormal_M->getSize().x, m_Size.y / m_TextureNormal_M->getSize().y);

            // Check if there is a separate hover image
            if (m_SeparateHoverImage)
            {
                // Draw the correct image
                if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                {
                    target.draw(m_SpriteMouseDown_M, states);
                }
                else if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                {
                    target.draw(m_SpriteMouseHover_M, states);
                }
                else
                    target.draw(m_SpriteNormal_M, states);
            }
            else
            {
                // Draw the button
                if ((m_MouseDown) && (m_MouseHover) && (m_ObjectPhase & ObjectPhase_MouseDown))
                    target.draw(m_SpriteMouseDown_M, states);
                else
                    target.draw(m_SpriteNormal_M, states);

                // When the mouse is on top of the button then draw an extra image
                if ((m_MouseHover) && (m_ObjectPhase & ObjectPhase_Hover))
                    target.draw(m_SpriteMouseHover_M, states);
            }

            // When the button is focused then draw an extra image
            if ((m_Focused) && (m_ObjectPhase & ObjectPhase_Focused))
                target.draw(m_SpriteFocused_M, states);
        }

        // If the button has a text then also draw the text
        if (m_Text.getString().getSize() > 0)
        {
            // Reset the transformations
            states.transform = oldTransform;

            // Get the current size of the text, so that we can recalculate the position
            sf::FloatRect rect = m_Text.getGlobalBounds();

            // Calculate the new position for the text
            rect.left = (m_Size.x - rect.width) * 0.5f - rect.left;
            rect.top = (m_Size.y - rect.height) * 0.5f - rect.top;

            // Set the new position
            states.transform.translate(std::floor(rect.left + 0.5f), std::floor(rect.top + 0.5f));

            // Draw the text
            target.draw(m_Text, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
