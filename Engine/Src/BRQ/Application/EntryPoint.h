#pragma once

extern BRQ::Application* BRQ::CreateApplication(const WindowProperties& props);

int main(int argc, char** argv) {

    auto application = BRQ::CreateApplication();

    application->Run();

    delete application;

    return 0;
}

