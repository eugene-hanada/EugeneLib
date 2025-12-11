# Copilot instructions for EugeneLib (short)

- Primary language: Japanese.
- Project: C++ (C++23), Visual Studio / CMake. Follow repository AGENT.md for full rules.
- Naming: PascalCase for types and methods, enum class with PascalCase enumerators, member vars use trailing underscore (e.g. windowSize_).
- APIs: Put public declarations in Include/, implementations in Source/. Prefer const T& / std::span / std::string_view for non-trivial params.
- Error policy: initialization/fatal errors Å® exceptions; per-frame/runtime recoverable errors Å® return status (avoid throwing in game loop).
- Ownership: raw pointers non-owning, use unique_ptr / ComPtr / vk::Unique* for ownership.
- Style: follow repo style (tabs, brace-on-same-line). Add Doxygen-style comments for public API.
- Security: do not surface secrets or private credentials.
- Formatting: prefer repository .clang-format if present.