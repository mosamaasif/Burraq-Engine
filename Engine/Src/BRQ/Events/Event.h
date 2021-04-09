#pragma once

#include "Core/Base.h"

namespace BRQ {

    enum class EventType {

        None = 0,
        WindowClose, WindowResize,
        KeyPressed, KeyReleased,
        MouseButtonPressed, MouseButtonReleased, MouseMoved
    };

    enum EventCategory {

        None = 0,
        EventCategoryApplication	= BIT(0),
        EventCategoryInput			= BIT(1),
        EventCategoryMouse			= BIT(2),
        EventCategoryMouseButton	= BIT(3)
    };

#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType() { return EventType::type; }\
                                virtual EventType GetEventType() const override { return GetStaticType(); }\
                                virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

    class Event {
    public:
        bool Handled = false;

    public:
        virtual ~Event() = default;

        virtual EventType GetEventType() const = 0;
        virtual const char* GetName() const = 0;
        virtual I32 GetCategoryFlags() const = 0;
        virtual std::string ToString() const { return GetName(); }

        bool IsInCategory(EventCategory category) {

            return GetCategoryFlags() & (I32)category;
        }
    };

    class EventDispatcher {

    private:
        Event& m_Event;

    public:
        EventDispatcher(Event& event)
            : m_Event(event) { }

        template<typename T, typename F>
        bool Dispatch(const F& function) {

            if (m_Event.GetEventType() == T::GetStaticType()) {

                m_Event.Handled |= function(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }
    };
}