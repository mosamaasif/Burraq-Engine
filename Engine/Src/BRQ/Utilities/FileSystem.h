#pragma once

#include <BRQ.h>

namespace BRQ { namespace Utilities {

    class FileSystem {

    public:
        enum class InputMode {

            ReadOnly = 0,
            ReadBinary,
            WriteOnly,
            WriteBinary,
            Append,
            ApendBinary,
            ReadAndWrite,
            ReadAndWriteBinary,
        };

    private:
        static FileSystem*      s_Instance;
        std::string             m_RootDirectory;

    protected:
        FileSystem() = default;
        FileSystem(const FileSystem& filesystem) = delete;

    public:
        ~FileSystem() = default;

        static void Init();
        static void Shutdown();

        static FileSystem* GetInstance() { return s_Instance; }

        std::vector<BYTE> ReadFile(const std::string_view& filename, InputMode mode = InputMode::ReadOnly) const;
        void WriteFile(const std::string_view& filename, InputMode mode, const std::vector<BYTE>& data) const;

        void SetRootDirectory(const std::string& root) { m_RootDirectory = root; }
    private:
        const char* InputModeToString(InputMode mode) const;
    };
} }