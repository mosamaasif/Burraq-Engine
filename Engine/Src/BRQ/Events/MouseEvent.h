#pragma once

#include "Event.h"
#include "Application/Keys.h"

namespace BRQ {

    class MouseMovedEvent : public Event {

    private:
        F32 m_MouseX, m_MouseY;

    public:
        MouseMovedEvent(const F32 x, const F32 y)
            : m_MouseX(x), m_MouseY(y) {}

        F32 GetX() const { return m_MouseX; }
        F32 GetY() const { return m_MouseY; }

        std::string ToString() const override {

            std::stringstream ss;
            ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseMoved)
        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
    };

    class MouseButtonEvent : public Event {

    protected:
        MouseButtonEvent(const MouseButton button)
            : m_Button(button) {}

        MouseButton m_Button;

    public:
        MouseButton GetMouseButton() const { return m_Button; }

        EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
    };

    class MouseButtonPressedEvent : public MouseButtonEvent {

    public:
        MouseButtonPressedEvent(const MouseButton button)
            : MouseButtonEvent(button) {}

        std::string ToString() const override {

            std::stringstream ss;
            ss << "MouseButtonPressedEvent: " << (U32)m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonPressed)
    };

    class MouseButtonReleasedEvent : public MouseButtonEvent {

    public:
        MouseButtonReleasedEvent(const MouseButton button)
            : MouseButtonEvent(button) {}

        std::string ToString() const override {

            std::stringstream ss;
            ss << "MouseButtonReleasedEvent: " << (U32)m_Button;
            return ss.str();
        }

        EVENT_CLASS_TYPE(MouseButtonReleased)
    };
}