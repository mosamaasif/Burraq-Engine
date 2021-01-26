#include <Engine.h>
#include <BRQ/Application/EntryPoint.h>

class Minecraft : public BRQ::Application {

public:
    Minecraft(const BRQ::WindowProperties& props)
        : Application(props)
    {

    }

    ~Minecraft()
    {

    }
};

BRQ::Application* BRQ::CreateApplication(const BRQ::WindowProperties& props) {

    return new Minecraft(props);
}