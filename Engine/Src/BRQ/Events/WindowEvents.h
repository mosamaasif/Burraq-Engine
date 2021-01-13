#pragma once

#include "Events/Event.h"

namespace BRQ {

    class WindowResizeEvent : public Event {

    private:
        U32 m_Width, m_Height;

    public:
        WindowResizeEvent(U32 width, U32 height)
            : m_Width(width), m_Height(height) { }

        U32 GetWidth() const { return m_Width; }
        U32 GetHeight() const { return m_Height; }

        std::string ToString() const override {

            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(WindowResize)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };

    class WindowCloseEvent : public Event {

    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(WindowClose)
        EVENT_CLASS_CATEGORY(EventCategoryApplication)
    };
}