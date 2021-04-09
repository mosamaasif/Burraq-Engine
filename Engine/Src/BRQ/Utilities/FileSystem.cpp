#include <BRQ.h>

#include "FileSystem.h"

namespace BRQ { namespace Utilities {

    FileSystem* FileSystem::s_Instance = nullptr;

    void FileSystem::Init() {

        s_Instance = new FileSystem();
    }

    void FileSystem::Shutdown() {

        if (s_Instance) {

            delete s_Instance;
        }
    }

    std::vector<BYTE> FileSystem::ReadFile(const std::string_view& filename, InputMode mode) const {

        std::vector<BYTE> result;

        FILE* handle = nullptr;

        std::string path = m_RootDirectory + filename.data();

        errno_t error = fopen_s(&handle, path.c_str(), InputModeToString(mode));

        if (error) {

            BRQ_CORE_WARN("Can't open file: {}", path.c_str());
            return result;
        }

        fseek(handle, 0, SEEK_END);

        U64 size = ftell(handle);

        fseek(handle, 0, SEEK_SET);

        result.resize(size);

        fread(&result[0], size, sizeof(BYTE), handle);

        return std::move(result);
    }

    void FileSystem::WriteFile(const std::string_view& filename, InputMode mode, const std::vector<BYTE>& data) const {

        FILE* handle = nullptr;

        std::string path = m_RootDirectory + filename.data();

        errno_t error = fopen_s(&handle, path.c_str(), InputModeToString(mode));

        if (error) {

            BRQ_CORE_WARN("Can't write to file: {}", path.c_str());
            return;
        }

        fwrite(&data[0], data.size(), sizeof(BYTE), handle);
    }

    const char* FileSystem::InputModeToString(InputMode mode) const {

        switch (mode) {
        case InputMode::ReadOnly:
            return "r";
        case InputMode::ReadBinary:
            return "rb";
        case InputMode::WriteOnly:
            return "w";
        case InputMode::WriteBinary:
            return "wb";
        case InputMode::Append:
            return "a";
        case InputMode::ApendBinary:
            return "ab";
        case InputMode::ReadAndWrite:
            return "r+";
        case InputMode::ReadAndWriteBinary:
            return "rb+";
        default:
            return "rb+";
        }
    }
} }