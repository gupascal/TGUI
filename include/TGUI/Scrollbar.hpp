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


#ifndef TGUI_SCROLLBAR_HPP
#define TGUI_SCROLLBAR_HPP

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace tgui
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class TGUI_API Scrollbar : public Slider
    {
      public:

        typedef SharedObjectPtr<Scrollbar> Ptr;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Default constructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Scrollbar();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Copy constructor
        ///
        /// \param copy  Instance to copy
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Scrollbar(const Scrollbar& copy);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Destructor
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual ~Scrollbar();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Overload of assignment operator
        ///
        /// \param right  Instance to assign
        ///
        /// \return Reference to itself
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        Scrollbar& operator= (const Scrollbar& right);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Makes a copy of the object by calling the copy constructor.
        // This function calls new and if you use this function then you are responsible for calling delete.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual Scrollbar* clone();


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Loads the scrollbar images.
        ///
        /// \param pathname  The path to the folder that contains the images.
        ///                  The folder must also contain an info.txt file, which will give more information about the scrollbar.
        ///
        /// \return
        ///        - true on success
        ///        - false when the pathname is empty
        ///        - false when the info.txt file was not found
        ///        - false when the images couldn't be loaded
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool load(const std::string& pathname);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Does nothing.
        ///
        /// \param minimum  Unused parameter
        ///
        /// This function is overridden from Slider so that the minimum can't be changed.
        /// The minimum will always stay 0.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setMinimum(unsigned int minimum);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the current value.
        ///
        /// \param value  The new value
        ///
        /// The value has to be smaller than maximum - low value.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setValue(unsigned int value);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes the 'low value'.
        ///
        /// \param lowValue  The new low value.
        ///                  In e.g. a list box, this value is the amount of items that fit inside the list box.
        ///
        /// Until the maximum is bigger than this value, no scrollbar will be drawn.
        /// You can however choose to always draw the scrollbar by calling setAutoHide(false).
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setLowValue(unsigned int lowValue);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns the low value.
        ///
        /// \see setLowValue
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual unsigned int getLowValue() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Changes whether the scrollbar should hide automatically or not.
        ///
        /// When true (default), the scrollbar will not be drawn when the maximum is smaller than the low value.
        ///
        /// \param autoHide  Should the scrollbar be invisible when you can't scroll?
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void setAutoHide(bool autoHide);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// \brief Returns whether the scrollbar is hiding automatically or not.
        ///
        /// When true (default), the scrollbar will not be drawn when the maximum is smaller than the low value.
        /// So when you can't scroll, the scrollbar won't be drawn.
        ///
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool getAutoHide() const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Used to communicate with EventManager.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual bool mouseOnObject(float x, float y);
        virtual void leftMousePressed(float x, float y);
        virtual void leftMouseReleased(float x, float y);
        virtual void mouseMoved(float x, float y);


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Because this class is derived from sf::Drawable, you can just call the draw function from your sf::RenderTarget.
        // This function will be called and it will draw the scrollbar on the render target.
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      public:

        enum ScrollbarCallbacks
        {
            AllScrollbarCallbacks = SliderCallbacksCount - 1,
            ScrollbarCallbacksCount = SliderCallbacksCount
        };


        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      protected:

        // Maximum should be above this value before the scrollbar is needed
        unsigned int m_LowValue;

        // When no scrollbar is needed, should the scrollbar be drawn or stay hidden?
        bool m_AutoHide;

        // Did the mouse went down on one of the arrows?
        bool m_MouseDownOnArrow;

        sf::Texture* m_TextureArrowNormal;
        sf::Texture* m_TextureArrowHover;

        sf::Sprite   m_SpriteArrowNormal;
        sf::Sprite   m_SpriteArrowHover;


        // ListBox, ComboBox and TextBox can access the scrollbar directly
        friend class ListBox;
        friend class ComboBox;
        friend class TextBox;
        friend class ChatBox;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TGUI_SCROLLBAR_HPP
