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


#include <TGUI/Label.hpp>
#include <TGUI/Panel.hpp>
#include <TGUI/Scrollbar.hpp>
#include <TGUI/SharedWidgetPtr.inl>
#include <TGUI/ChatBox.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox() :
    m_TextSize   (15),
    m_BorderColor(sf::Color::Black),
    m_Scroll     (nullptr)
    {
        m_Callback.widgetType = Type_Unknown;
        m_DraggableWidget = true;

        m_Panel = new Panel();
        m_Panel->setSize(360, 200);
        m_Panel->setBackgroundColor(sf::Color::White);

        // Load the chat box with default values
        m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::ChatBox(const ChatBox& copy) :
    Widget            (copy),
    WidgetBorders     (copy),
    m_LoadedConfigFile(copy.m_LoadedConfigFile),
    m_TextSize        (copy.m_TextSize),
    m_BorderColor     (copy.m_BorderColor)
    {
        m_Panel = new Panel(*copy.m_Panel);

        // If there is a scrollbar then copy it
        if (copy.m_Scroll != nullptr)
            m_Scroll = new Scrollbar(*copy.m_Scroll);
        else
            m_Scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox::~ChatBox()
    {
        delete m_Panel;

        if (m_Scroll != nullptr)
            delete m_Scroll;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox& ChatBox::operator= (const ChatBox& right)
    {
        if (this != &right)
        {
            // If there already was a scrollbar then delete it now
            if (m_Scroll != nullptr)
            {
                delete m_Scroll;
                m_Scroll = nullptr;
            }

            ChatBox temp(right);
            this->Widget::operator=(right);
            this->WidgetBorders::operator=(right);

            std::swap(m_LoadedConfigFile, temp.m_LoadedConfigFile);
            std::swap(m_TextSize,         temp.m_TextSize);
            std::swap(m_BorderColor,      temp.m_BorderColor);
            std::swap(m_Panel,            temp.m_Panel);
            std::swap(m_Scroll,           temp.m_Scroll);
        }

        return *this;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ChatBox* ChatBox::clone()
    {
        return new ChatBox(*this);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::load(const std::string& configFileFilename)
    {
        m_LoadedConfigFile = configFileFilename;

        // When everything is loaded successfully, this will become true.
        m_Loaded = false;

        // If there already was a scrollbar then delete it now
        if (m_Scroll != nullptr)
        {
            delete m_Scroll;
            m_Scroll = nullptr;
        }

        // Open the config file
        ConfigFile configFile;
        if (!configFile.open(configFileFilename))
        {
            TGUI_OUTPUT("TGUI error: Failed to open " + configFileFilename + ".");
            return false;
        }

        // Read the properties and their values (as strings)
        std::vector<std::string> properties;
        std::vector<std::string> values;
        if (!configFile.read("ChatBox", properties, values))
        {
            TGUI_OUTPUT("TGUI error: Failed to parse " + configFileFilename + ".");
            return false;
        }

        // Close the config file
        configFile.close();

        // Find the folder that contains the config file
        std::string configFileFolder = "";
        std::string::size_type slashPos = configFileFilename.find_last_of("/\\");
        if (slashPos != std::string::npos)
            configFileFolder = configFileFilename.substr(0, slashPos+1);

        // Handle the read properties
        for (unsigned int i = 0; i < properties.size(); ++i)
        {
            std::string property = properties[i];
            std::string value = values[i];

            if (property == "backgroundcolor")
            {
                setBackgroundColor(configFile.readColor(value));
            }
            else if (property == "bordercolor")
            {
                setBorderColor(configFile.readColor(value));
            }
            else if (property == "borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
            }
            else if (property == "scrollbar")
            {
                if ((value.length() < 3) || (value[0] != '"') || (value[value.length()-1] != '"'))
                {
                    TGUI_OUTPUT("TGUI error: Failed to parse value for Scrollbar in section ChatBox in " + configFileFilename + ".");
                    return false;
                }

                // load the scrollbar and check if it failed
                m_Scroll = new Scrollbar();
                if (m_Scroll->load(configFileFolder + value.substr(1, value.length()-2)) == false)
                {
                    // The scrollbar couldn't be loaded so it must be deleted
                    delete m_Scroll;
                    m_Scroll = nullptr;

                    return false;
                }
                else // The scrollbar was loaded successfully
                {
                    // Initialize the scrollbar
                    m_Scroll->setVerticalScroll(true);
                    m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y - m_TopBorder - m_BottomBorder));
                    m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
                    m_Scroll->setMaximum(static_cast<unsigned int>(m_Panel->getWidgets().size() * m_TextSize * 1.4f));
                }
            }
            else
                TGUI_OUTPUT("TGUI warning: Unrecognized property '" + property + "' in section ChatBox in " + configFileFilename + ".");
        }

        return m_Loaded = true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const std::string& ChatBox::getLoadedConfigFile()
    {
        return m_LoadedConfigFile;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setSize(float width, float height)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // A negative size is not allowed for this widget
        if (width  < 0) width  = -width;
        if (height < 0) height = -height;

        // There is a minimum width
        if (m_Scroll == nullptr)
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder, width);
        else
            width = TGUI_MAXIMUM(50 + m_LeftBorder + m_RightBorder + m_Scroll->getSize().x, width);

        // Remember the old height
        float oldHeight = m_Panel->getSize().y;

        // Set the new size
        m_Panel->setSize(width, height);

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != nullptr)
        {
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y - m_TopBorder - m_BottomBorder));
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
        }

        // Find out how much the height has changed
        float heightDiff = m_Panel->getSize().y - oldHeight;

        // Reposition all labels in the chatbox
        std::vector<Widget::Ptr>& labels = m_Panel->getWidgets();
        for (std::vector<Widget::Ptr>::iterator it = labels.begin(); it != labels.end(); ++it)
            (*it)->setPosition((*it)->getPosition().x, (*it)->getPosition().y + heightDiff);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    sf::Vector2f ChatBox::getSize() const
    {
        return m_Panel->getSize();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::addLine(const sf::String& text, const sf::Color& color)
    {
        // Call this function for every line in the text
        std::string::size_type newlinePos = text.find("\n");
        if (newlinePos != std::string::npos)
        {
            std::string::size_type lastNewlinePos = 0;
            while (newlinePos != std::string::npos)
            {
                addLine(text.toWideString().substr(lastNewlinePos, newlinePos - lastNewlinePos));

                lastNewlinePos = newlinePos + 1;
                newlinePos = text.find("\n", lastNewlinePos);
            }

            addLine(text.toWideString().substr(lastNewlinePos, newlinePos - lastNewlinePos));
            return;
        }

        Label::Ptr label(*m_Panel);
        label->setText(text);
        label->setTextColor(color);
        label->setTextSize(m_TextSize);
        label->setPosition(m_LeftBorder + 2.0f, m_Panel->getSize().y - (m_TextSize * 1.2f));

        float width;
        if (m_Scroll == nullptr)
            width = m_Panel->getSize().x - m_LeftBorder - m_RightBorder;
        else
            width = m_Panel->getSize().x - m_LeftBorder - m_RightBorder - m_Scroll->getSize().x;

        if (width < 0)
            width = 0;

        // Split the label over multiple lines if necessary
        while (label->getSize().x > width)
        {
            Label::Ptr newLabel = m_Panel->copy(label);
            newLabel->setText("");

            while (label->getSize().x > width)
            {
                sf::String string = label->getText();
                newLabel->setText(string[string.getSize()-1] + newLabel->getText());

                string.erase(string.getSize()-1, 1);
                label->setText(string);
            }

            label = newLabel;
        }

        if (m_Scroll != nullptr)
        {
            m_Scroll->setMaximum(static_cast<unsigned int>(m_Panel->getWidgets().size() * m_TextSize * 1.4f));

            if (m_Scroll->getMaximum() > m_Scroll->getLowValue())
                m_Scroll->setValue(m_Scroll->getMaximum() - m_Scroll->getLowValue());
        }

        // Reposition the labels
        updateDisplayedText();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextFont(const sf::Font& font)
    {
        m_Panel->setGlobalFont(font);

        std::vector<Widget::Ptr>& labels = m_Panel->getWidgets();
        for (std::vector<Widget::Ptr>::iterator it = labels.begin(); it != labels.end(); ++it)
            static_cast<Label::Ptr>(*it)->setTextFont(font);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Font* ChatBox::getTextFont() const
    {
        return &m_Panel->getGlobalFont();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTextSize(unsigned int size)
    {
        // Store the new text size
        m_TextSize = size;

        // There is a minimum text size
        if (m_TextSize < 8)
            m_TextSize = 8;

        // Update the position of the labels
        updateDisplayedText();

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != nullptr)
        {
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    unsigned int ChatBox::getTextSize() const
    {
        return m_TextSize;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBorders(unsigned int leftBorder, unsigned int topBorder, unsigned int rightBorder, unsigned int bottomBorder)
    {
        // Set the new border size
        m_LeftBorder   = leftBorder;
        m_TopBorder    = topBorder;
        m_RightBorder  = rightBorder;
        m_BottomBorder = bottomBorder;

        // There is a minimum width
        float width = m_Panel->getSize().x;
        if (width < (50.f + m_LeftBorder + m_RightBorder))
            width = 50.f + m_LeftBorder + m_RightBorder;

        // Make sure that the panel has a valid size
        m_Panel->setSize(width, m_Panel->getSize().y);

        // Reposition the labels
        std::vector<Widget::Ptr>& labels = m_Panel->getWidgets();
        for (std::vector<Widget::Ptr>::iterator it = labels.begin(); it != labels.end(); ++it)
            (*it)->setPosition(m_LeftBorder + 2.0f, (*it)->getPosition().y);

        // If there is a scrollbar then reinitialize it
        if (m_Scroll != nullptr)
        {
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBackgroundColor(const sf::Color& backgroundColor)
    {
        m_Panel->setBackgroundColor(backgroundColor);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setBorderColor(const sf::Color& borderColor)
    {
        m_BorderColor = borderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getBackgroundColor() const
    {
        return m_Panel->getBackgroundColor();
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    const sf::Color& ChatBox::getBorderColor() const
    {
        return m_BorderColor;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::setScrollbar(const std::string& scrollbarConfigFileFilename)
    {
        // Do nothing when the string is empty
        if (scrollbarConfigFileFilename.empty() == true)
            return false;

        // If the scrollbar was already created then delete it first
        if (m_Scroll != nullptr)
            delete m_Scroll;

        // load the scrollbar and check if it failed
        m_Scroll = new Scrollbar();
        if(m_Scroll->load(scrollbarConfigFileFilename) == false)
        {
            // The scrollbar couldn't be loaded so it must be deleted
            delete m_Scroll;
            m_Scroll = nullptr;

            return false;
        }
        else // The scrollbar was loaded successfully
        {
            // Initialize the scrollbar
            m_Scroll->setVerticalScroll(true);
            m_Scroll->setSize(m_Scroll->getSize().x, m_Panel->getSize().y - m_TopBorder - m_BottomBorder);
            m_Scroll->setLowValue(static_cast<unsigned int>(m_Panel->getSize().y) - m_TopBorder - m_BottomBorder);
            m_Scroll->setMaximum(static_cast<unsigned int>(m_Panel->getWidgets().size() * m_TextSize * 1.4f));

            return true;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::removeScrollbar()
    {
        // Delete the scrollbar
        delete m_Scroll;
        m_Scroll = nullptr;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::setTransparency(unsigned char transparency)
    {
        Widget::setTransparency(transparency);

        m_BorderColor.a = m_Opacity;
        m_Panel->setTransparency(transparency);

        if (m_Scroll != nullptr)
            m_Scroll->setTransparency(transparency);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::mouseOnWidget(float x, float y)
    {
        // Get the current position
        sf::Vector2f position = getPosition();

        // Pass the event to the scrollbar (if there is one)
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(position.x + m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x, position.y + m_TopBorder);

            // Pass the event
            m_Scroll->mouseOnWidget(x, y);

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }

        // Check if the mouse is on top of the list box
        if (getTransform().transformRect(sf::FloatRect(static_cast<float>(m_LeftBorder),
                                                       static_cast<float>(m_TopBorder),
                                                       m_Panel->getSize().x - m_LeftBorder - m_RightBorder,
                                                       m_Panel->getSize().y - m_TopBorder - m_BottomBorder)).contains(x, y))
            return true;
        else // The mouse is not on top of the list box
        {
            if (m_MouseHover)
                mouseLeftWidget();

            m_MouseHover = false;
            return false;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMousePressed(float x, float y)
    {
        // Don't do anything when the text box wasn't loaded correctly
        if (m_Loaded == false)
            return;

        // Set the mouse down flag to true
        m_MouseDown = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Remember the old scrollbar value
            unsigned int oldValue = m_Scroll->getValue();

            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x, getPosition().y + m_TopBorder);

            // Pass the event
            if (m_Scroll->mouseOnWidget(x, y))
                m_Scroll->leftMousePressed(x, y);

            // Reset the position
            m_Scroll->setPosition(0, 0);

            // If the value of the scrollbar has changed then update the text
            if (oldValue != m_Scroll->getValue())
                updateDisplayedText();
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::leftMouseReleased(float x, float y)
    {
        // If there is a scrollbar then pass it the event
        if (m_Scroll != nullptr)
        {
            // Only pass the event when the scrollbar still thinks the mouse is down
            if (m_Scroll->m_MouseDown == true)
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->getValue();

                // Temporarily set the position of the scroll
                m_Scroll->setPosition(getPosition().x + m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x, getPosition().y + m_TopBorder);

                // Pass the event
                m_Scroll->leftMouseReleased(x, y);

                // Reset the position
                m_Scroll->setPosition(0, 0);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->getValue())
                {
                    // Check if the scrollbar value was incremented (you have pressed on the down arrow)
                    if (m_Scroll->getValue() == oldValue + 1)
                    {
                        // Decrement the value
                        m_Scroll->setValue(m_Scroll->getValue()-1);

                        // Scroll down with the whole item height instead of with a single pixel
                        m_Scroll->setValue(m_Scroll->getValue() + static_cast<unsigned int>(m_TextSize * 1.4f + 0.5) - (m_Scroll->getValue() % static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f)));
                    }
                    else if (m_Scroll->getValue() == oldValue - 1) // Check if the scrollbar value was decremented (you have pressed on the up arrow)
                    {
                        // increment the value
                        m_Scroll->setValue(m_Scroll->getValue()+1);

                        // Scroll up with the whole item height instead of with a single pixel
                        if (m_Scroll->getValue() % static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f) > 0)
                            m_Scroll->setValue(m_Scroll->getValue() - (m_Scroll->getValue() % static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f)));
                        else
                            m_Scroll->setValue(m_Scroll->getValue() - static_cast<unsigned int>(m_TextSize * 1.4f + 0.5f));
                    }

                    updateDisplayedText();
                }
            }
        }

        m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseMoved(float x, float y)
    {
        if (m_MouseHover == false)
            mouseEnteredWidget();

        // Set the mouse move flag
        m_MouseHover = true;

        // If there is a scrollbar then pass the event
        if (m_Scroll != nullptr)
        {
            // Temporarily set the position of the scroll
            m_Scroll->setPosition(getPosition().x + m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x, getPosition().y + m_TopBorder);

            // Check if you are dragging the thumb of the scrollbar
            if ((m_Scroll->m_MouseDown) && (m_Scroll->m_MouseDownOnThumb))
            {
                // Remember the old scrollbar value
                unsigned int oldValue = m_Scroll->getValue();

                // Pass the event, even when the mouse is not on top of the scrollbar
                m_Scroll->mouseMoved(x, y);

                // If the value of the scrollbar has changed then update the text
                if (oldValue != m_Scroll->getValue())
                    updateDisplayedText();
            }
            else // You are just moving the mouse
            {
                // When the mouse is on top of the scrollbar then pass the mouse move event
                if (m_Scroll->mouseOnWidget(x, y))
                    m_Scroll->mouseMoved(x, y);
            }

            // Reset the position
            m_Scroll->setPosition(0, 0);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNotOnWidget()
    {
        if (m_MouseHover)
            mouseLeftWidget();

        m_MouseHover = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseHover = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseNoLongerDown()
    {
        m_MouseDown = false;

        if (m_Scroll != nullptr)
            m_Scroll->m_MouseDown = false;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::mouseWheelMoved(int delta)
    {
        // Only do something when there is a scrollbar
        if (m_Scroll != nullptr)
        {
            if (m_Scroll->getLowValue() < m_Scroll->getMaximum())
            {
                // Check if you are scrolling down
                if (delta < 0)
                {
                    // Scroll down
                    m_Scroll->setValue( m_Scroll->getValue() + (static_cast<unsigned int>(-delta) * (static_cast<unsigned int>(m_TextSize * 1.4f + 0.5) / 2)) );
                }
                else // You are scrolling up
                {
                    unsigned int change = static_cast<unsigned int>(delta) * (static_cast<unsigned int>(m_TextSize * 1.4f + 0.5) / 2);

                    // Scroll up
                    if (change < m_Scroll->getValue())
                        m_Scroll->setValue( m_Scroll->getValue() - change );
                    else
                        m_Scroll->setValue(0);
                }

                updateDisplayedText();
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::setProperty(const std::string& property, const std::string& value)
    {
        if (!Widget::setProperty(property, value))
        {
            if (property == "ConfigFile")
            {
                load(value);
            }
            else if (property == "TextSize")
            {
                setTextSize(atoi(value.c_str()));
            }
            else if (property == "Borders")
            {
                Borders borders;
                if (extractBorders(value, borders))
                    setBorders(borders.left, borders.top, borders.right, borders.bottom);
                else
                    TGUI_OUTPUT("TGUI error: Failed to parse 'Borders' property.");
            }
            else if (property == "BackgroundColor")
            {
                setBackgroundColor(extractColor(value));
            }
            else if (property == "BorderColor")
            {
                setBorderColor(extractColor(value));
            }
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool ChatBox::getProperty(const std::string& property, std::string& value)
    {
        if (!Widget::getProperty(property, value))
        {
            if (property == "ConfigFile")
                value = getLoadedConfigFile();
            else if (property == "TextSize")
                value = to_string(getTextSize());
            else if (property == "Borders")
                value = "(" + to_string(getBorders().left) + "," + to_string(getBorders().top) + "," + to_string(getBorders().right) + "," + to_string(getBorders().bottom) + ")";
            else if (property == "BackgroundColor")
                value = "(" + to_string(int(getBackgroundColor().r)) + "," + to_string(int(getBackgroundColor().g)) + "," + to_string(int(getBackgroundColor().b)) + "," + to_string(int(getBackgroundColor().a)) + ")";
            else if (property == "BorderColor")
                value = "(" + to_string(int(getBorderColor().r)) + "," + to_string(int(getBorderColor().g)) + "," + to_string(int(getBorderColor().b)) + "," + to_string(int(getBorderColor().a)) + ")";
            else // The property didn't match
                return false;
        }

        // You pass here when one of the properties matched
        return true;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::initialize(Container *const parent)
    {
        m_Parent = parent;
        setTextFont(m_Parent->getGlobalFont());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::updateDisplayedText()
    {
        float bottomPosition = m_Panel->getSize().y;
        std::vector<Widget::Ptr>& labels = m_Panel->getWidgets();
        for (unsigned int i = labels.size(); i > 0; --i)
        {
            unsigned int index = i - 1;

            static_cast<Label::Ptr>(labels[index])->setTextSize(m_TextSize);

            if (m_Scroll != nullptr)
                labels[index]->setPosition(m_LeftBorder + 2.0f, bottomPosition - (m_TextSize * 1.4f) + m_Scroll->getMaximum() - m_Scroll->getLowValue() - m_Scroll->getValue());
            else
                labels[index]->setPosition(m_LeftBorder + 2.0f, bottomPosition - (m_TextSize * 1.4f));

            bottomPosition -= m_TextSize * 1.4f;
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ChatBox::draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // Adjust the transformation
        states.transform *= getTransform();

        // Draw the panel
        target.draw(*m_Panel, states);

        // Draw left border
        sf::RectangleShape border(sf::Vector2f(static_cast<float>(m_LeftBorder), m_Panel->getSize().y));
        border.setFillColor(m_BorderColor);
        target.draw(border, states);

        // Draw top border
        border.setSize(sf::Vector2f(m_Panel->getSize().x, static_cast<float>(m_TopBorder)));
        target.draw(border, states);

        // Draw right border
        border.setPosition(m_Panel->getSize().x - m_RightBorder, 0);
        border.setSize(sf::Vector2f(static_cast<float>(m_RightBorder), m_Panel->getSize().y));
        target.draw(border, states);

        // Draw bottom border
        border.setPosition(0, m_Panel->getSize().y - m_BottomBorder);
        border.setSize(sf::Vector2f(m_Panel->getSize().x, static_cast<float>(m_BottomBorder)));
        target.draw(border, states);

        // Check if there is a scrollbar
        if (m_Scroll != nullptr)
        {
            // Reset the transformation
            states.transform.translate(m_Panel->getSize().x - m_RightBorder - m_Scroll->getSize().x, static_cast<float>(m_TopBorder));

            // Draw the scrollbar
            target.draw(*m_Scroll, states);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
