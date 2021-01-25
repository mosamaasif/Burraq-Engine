#include <BRQ.h>

#include "Platform/Platform.h"
#include "Logger/Log.h"
#include "Window.h"
#include "Input.h"

#include "Graphics/Renderer.h"
#include "Graphics/Mesh.h"

int main() {

    using namespace BRQ;
    using namespace Utilities;

    Log::Init();

    Window* window = Window::Create();

    FileSystem::Init();

    Renderer::SubmitResources({ {"Src/Shaders/shader.vert.spv", VKShader::ShaderType::Vertex},
                                {"Src/Shaders/shader.frag.spv", VKShader::ShaderType::Fragment} });

    Renderer::Init(window);
    auto renderer = Renderer::GetInstance();

    auto input = Input::GetInstance();

    while (window->IsOpen()) {

        renderer->Present();
        window->OnUpdate();
    }

    Renderer::Shutdown();

    delete window;

    FileSystem::Shutdown();
    Log::Shutdown();

    return 0;
}