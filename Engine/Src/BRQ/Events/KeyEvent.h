#pragma once

#include "Application/Keys.h"
#include "Event.h"

namespace BRQ {

	class KeyEvent : public Event {
	protected:
		Key m_Code;

		KeyEvent(const Key code)
			: m_Code(code) {}

	public:
		Key GetKeyCode() const { return m_Code; }

		EVENT_CLASS_CATEGORY(EventCategoryInput)
	};

	class KeyPressedEvent : public KeyEvent {

	public:
		KeyPressedEvent(const Key code)
			: KeyEvent(code) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << (char)m_Code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	};

	class KeyReleasedEvent : public KeyEvent {

	public:
		KeyReleasedEvent(const Key code)
			: KeyEvent(code) {}

		std::string ToString() const override {

			std::stringstream ss;
			ss << "KeyReleasedEvent: " << (char)m_Code;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}