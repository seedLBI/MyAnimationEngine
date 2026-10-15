#ifndef LIVEPT_LOG_UTILS_H
#define LIVEPT_LOG_UTILS_H

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>
#include <Windows.h>

namespace LivePT {
    enum TypeUnicodeTable {
        DEFAULT,
        MATHEMATICAL_BOLD,
        MATHEMATICAL_BOLD_ITALIC,
        MATHEMATICAL_BOLD_SCRIPT,
        MATHEMATICAL_BOLD_FRAKTUR,
        MATHEMATICAL_SANS_SERIF,
        MATHEMATICAL_SANS_SERIF_BOLD,
        MATHEMATICAL_SANS_SERIF_ITALIC,
        MATHEMATICAL_SANS_SERIF_BOLD_ITALIC,
        MATHEMATICAL_MONOSPACE,
        FULLWIDTH
    };
    enum PunctuationWeight {
        WEIGHT_NORMAL,
        WEIGHT_LIGHT,
        WEIGHT_DOUBLE,
        WEIGHT_MEDIUM_BOLD,
        WEIGHT_HEAVY_BOLD
    };

    inline std::wstring CodePointToWCharVector(char32_t codepoint) {
        std::wstring result;

        if (codepoint <= 0xFFFF) {
            result.push_back(static_cast<wchar_t>(codepoint));
        }
        else if (codepoint <= 0x10FFFF) {
            codepoint -= 0x10000;
            result.push_back(static_cast<wchar_t>(0xD800 + (codepoint >> 10)));
            result.push_back(static_cast<wchar_t>(0xDC00 + (codepoint & 0x3FF)));
        }
        else {
            result.push_back(L'\xFFFD');
        }

        return result;
    }

    inline PunctuationWeight GetWeight(TypeUnicodeTable type) {
        switch (type) {
        case MATHEMATICAL_SANS_SERIF:
        case MATHEMATICAL_SANS_SERIF_ITALIC:
            return WEIGHT_LIGHT;

        case MATHEMATICAL_MONOSPACE:
            return WEIGHT_DOUBLE;

        case MATHEMATICAL_BOLD:
        case MATHEMATICAL_BOLD_ITALIC:
        case MATHEMATICAL_BOLD_SCRIPT:
        case MATHEMATICAL_BOLD_FRAKTUR:
            return WEIGHT_MEDIUM_BOLD;

        case MATHEMATICAL_SANS_SERIF_BOLD:
        case MATHEMATICAL_SANS_SERIF_BOLD_ITALIC:
            return WEIGHT_HEAVY_BOLD;

        default:
            return WEIGHT_NORMAL;
        }
    }
    inline char32_t TranslatePunctuation(wchar_t input, TypeUnicodeTable type) {
        PunctuationWeight weight = GetWeight(type);

        if (weight == WEIGHT_LIGHT) {
            switch (input) {
            case L'[': return 0x2772; // ❲
            case L']': return 0x2773; // ❳
            case L'<': return 0x27E8; // ⟨
            case L'>': return 0x27E9; // ⟩
            default:   return input;
            }
        }

        if (weight == WEIGHT_DOUBLE) {
            switch (input) {
            case L'(': return 0x2985; // ⦅
            case L')': return 0x2986; // ⦆
            case L'[': return 0x27E6; // ⟦
            case L']': return 0x27E7; // ⟧
            case L'{': return 0x2983; // ⦃
            case L'}': return 0x2984; // ⦄
            case L'<': return 0x27EA; // ⟪
            case L'>': return 0x27EB; // ⟫
            default:   return input;
            }
        }

        if (weight == WEIGHT_MEDIUM_BOLD) {
            switch (input) {
            case L'(': return 0x2768; // ❨
            case L')': return 0x2769; // ❩
            case L'[': return 0x3010; //〖
            case L']': return 0x3011; // 〗
            case L'{': return 0x2774; // ❴
            case L'}': return 0x2775; // ❵
            case L'<': return 0x276C; // ❬
            case L'>': return 0x276D; // ❭
            case L',': return 0x275F; // ❟
            case L';': return 0x204F; // ⁏
            case L'!': return 0x01C3; // ǃ
            case L'-': return 0x2013; // –
            default:   return input;
            }
        }

        if (weight == WEIGHT_HEAVY_BOLD) {
            switch (input) {
            case L'(': return 0x276A; // ❪
            case L')': return 0x276B; // ❫
            case L'[': return 0x3010; //【
            case L']': return 0x3011; // 】
            case L'{': return 0x2774; // ❴
            case L'}': return 0x2775; // ❵
            case L'<': return 0x2770; // ❰
            case L'>': return 0x2771; // ❱
            case L',': return 0x275F; // ❟
            case L';': return 0xFF1B; // ；
            case L'!': return 0x2757; // ❗
            case L'?': return 0x2753; // ❓
            case L'-': return 0x2501; // ━
            case L'+': return 0x2795; // ➕
            default:   return input;
            }
        }

        return input;
    }
    inline std::wstring GetTranslate(wchar_t input, TypeUnicodeTable type) {

        if (input < 0x20) { //(\n, \r, \t)
            return std::wstring(1, input);
        }

        std::wstring result;

        if (type == DEFAULT) {
            result = std::wstring(1, input);
        }
        else if (type == FULLWIDTH) {
            if (input == L' ') {
                result = CodePointToWCharVector(0x3000);
            }
            else if (input >= 0x21 && input <= 0x7E) {
                result = CodePointToWCharVector(static_cast<char32_t>(input) + 0xFEE0);
            }
            else {
                result = std::wstring(1, input);
            }
        }
        else {
            char32_t upper_base = 0;
            char32_t lower_base = 0;
            char32_t digit_base = 0;

            switch (type) {
            case MATHEMATICAL_BOLD:
                upper_base = 0x1D400; lower_base = 0x1D41A; digit_base = 0x1D7CE; break;
            case MATHEMATICAL_BOLD_ITALIC:
                upper_base = 0x1D468; lower_base = 0x1D482; digit_base = 0x1D7CE; break;
            case MATHEMATICAL_BOLD_SCRIPT:
                upper_base = 0x1D4D0; lower_base = 0x1D4EA; digit_base = 0x1D7CE; break;
            case MATHEMATICAL_BOLD_FRAKTUR:
                upper_base = 0x1D56C; lower_base = 0x1D586; digit_base = 0x1D7CE; break;
            case MATHEMATICAL_SANS_SERIF:
                upper_base = 0x1D5A0; lower_base = 0x1D5BA; digit_base = 0x1D7E2; break;
            case MATHEMATICAL_SANS_SERIF_BOLD:
                upper_base = 0x1D5D4; lower_base = 0x1D5EE; digit_base = 0x1D7EC; break;
            case MATHEMATICAL_SANS_SERIF_ITALIC:
                upper_base = 0x1D608; lower_base = 0x1D622; digit_base = 0x1D7E2; break;
            case MATHEMATICAL_SANS_SERIF_BOLD_ITALIC:
                upper_base = 0x1D63C; lower_base = 0x1D656; digit_base = 0x1D7EC; break;
            case MATHEMATICAL_MONOSPACE:
                upper_base = 0x1D670; lower_base = 0x1D68A; digit_base = 0x1D7F6; break;
            default:
                break;
            }

            if (input >= L'A' && input <= L'Z' && upper_base != 0) {
                result = CodePointToWCharVector(upper_base + (input - L'A'));
            }
            else if (input >= L'a' && input <= L'z' && lower_base != 0) {
                result = CodePointToWCharVector(lower_base + (input - L'a'));
            }
            else if (input >= L'0' && input <= L'9' && digit_base != 0) {
                result = CodePointToWCharVector(digit_base + (input - L'0'));
            }
            else if (input >= 0x21 && input <= 0x7E) {
                result = CodePointToWCharVector(TranslatePunctuation(input, type));
            }
            else {
                result = std::wstring(1, input);
            }
        }

        return result;
    }

    inline std::wstring GetTranslatedWord(const std::string& text, TypeUnicodeTable type = MATHEMATICAL_BOLD) {
        std::wstring w(text.size(), 0), out;
        int n = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), (int)text.size(), w.data(), (int)w.size());
        if (!n) n = MultiByteToWideChar(CP_ACP, 0, text.data(), (int)text.size(), w.data(), (int)w.size());
        w.resize(n);

        for (wchar_t c : w)
            out += GetTranslate(c, type);

        w = std::move(out);
        return w;
    }

    inline void LogTime(TypeUnicodeTable type = MATHEMATICAL_SANS_SERIF_BOLD_ITALIC) {
        SYSTEMTIME st;
        GetLocalTime(&st);

        wchar_t buf[32];
        int len = swprintf_s(buf, L"[%02u:%02u:%02u:%03u]",
            (unsigned)st.wHour, (unsigned)st.wMinute,
            (unsigned)st.wSecond, (unsigned)st.wMilliseconds);

        std::wstring out;
        for (int i = 0; i < len; ++i)
            out += GetTranslate(buf[i], type);

        OutputDebugStringW(out.c_str());
    }
    inline void Log(const std::string& text, TypeUnicodeTable type = MATHEMATICAL_BOLD) {
        OutputDebugStringW(GetTranslatedWord(text,type).c_str());
    }
    inline void LogWithTime(const std::string& text, TypeUnicodeTable type = MATHEMATICAL_SANS_SERIF_BOLD_ITALIC) {
        Log(text, type);
        LogTime(type);
    }
    inline void LogWithTime(const std::string& logo, const std::string& text, TypeUnicodeTable type = MATHEMATICAL_SANS_SERIF_BOLD_ITALIC) {
        Log(logo, type);
        LogTime(type);
        Log(text, MATHEMATICAL_BOLD);
    }

    inline void LogClickablePath(const std::string& absoluteFilePath, int line) {
        char workDirBuf[MAX_PATH];
        GetCurrentDirectoryA(MAX_PATH, workDirBuf);

        auto NormalizePath = [](const char* fullPath) -> std::string {
            std::string path(fullPath);
            std::replace(path.begin(), path.end(), '\\', '/');
            return path;
        };

        std::string workDir = NormalizePath(workDirBuf);
        std::string absPath = NormalizePath(absoluteFilePath.c_str());

        std::string wdLower = workDir;
        std::string absLower = absPath;
        std::transform(wdLower.begin(), wdLower.end(), wdLower.begin(), ::tolower);
        std::transform(absLower.begin(), absLower.end(), absLower.begin(), ::tolower);

        std::string result;

        if (absLower.find(wdLower) == 0) {
            std::string relPath = absPath.substr(workDir.length());
            if (!relPath.empty() && relPath[0] == '/') {
                relPath.erase(0, 1);
            }

            result = relPath + "(" + std::to_string(line) + "): ";
        }
        else {
            result = absPath + "(" + std::to_string(line) + "): ";
        }

        Log(result, TypeUnicodeTable::DEFAULT);
        Log("Click Me!!!!", TypeUnicodeTable::MATHEMATICAL_BOLD);

    }

}

#endif