namespace LivePT {

    // Структура для возврата точных выверенных координат текстового токена
    struct PreciseCoord {
        long line = 0;
        long column = 0;
        size_t absoluteOffset = std::wstring::npos;
    };

    // Сканирует текст файла НАЗАД от сырых координат компилятора, находя точное начало L"eval"
    inline PreciseCoord FindEvalTokenBackward(const std::wstring& fileText, long rawLine, long rawColumn) {
        PreciseCoord result;
        if (fileText.empty()) return result;

        std::wstring macroName = GetConfiguredMacroName(); // Получаем L"eval"

        // КРИТИЧЕСКИЙ АРХИТЕКТУРНЫЙ СДВИГ: 
        // Препроцессор компилятора (MSVC/Clang) ВСЕГДА раздувает символ '\t' до 8 пробелов.
        // Чтобы стартовая точка поиска не улетела влево, на этапе 2 используем жесткий шаг = 8.
        const long COMPILER_TAB_SIZE = 8;

        // 1. Быстро переходим к началу физической строки rawLine
        size_t lineStartOffset = 0;
        long currentLineIdx = 1;
        while (currentLineIdx < rawLine && lineStartOffset < fileText.length()) {
            size_t nextNL = fileText.find(L'\n', lineStartOffset);
            if (nextNL != std::wstring::npos) {
                lineStartOffset = nextNL + 1;
                currentLineIdx++;
            }
            else {
                break;
            }
        }

        // 2. Вычисляем символ в строке, соответствующий rawColumn
        size_t nextNL = fileText.find(L'\n', lineStartOffset);
        if (nextNL == std::wstring::npos) nextNL = fileText.length();
        std::wstring lineText = fileText.substr(lineStartOffset, nextNL - lineStartOffset);

        // __builtin_COLUMN() возвращает 1-based индекс (где '\t' = 1 символ).
        // Переводим в 0-based индекс и даем небольшой запас вправо (+ длина макроса),
        // чтобы rfind гарантированно захватил токен, даже если rawColumn указывает ровно на 'e'.
        size_t paramCharIdx = (rawColumn > 0) ? static_cast<size_t>(rawColumn - 1) : 0;
        paramCharIdx = (std::min)(paramCharIdx + macroName.length(), lineText.length());

        size_t searchOrigin = lineStartOffset + paramCharIdx;
        if (searchOrigin > fileText.length()) searchOrigin = fileText.length();

        auto isTokenValid = [&](size_t pos) {
            bool validLeft = (pos == 0 || (!iswalnum(fileText[pos - 1]) && fileText[pos - 1] != L'_'));
            bool validRight = (pos + macroName.length() >= fileText.length() ||
                (!iswalnum(fileText[pos + macroName.length()]) && fileText[pos + macroName.length()] != L'_'));
            return validLeft && validRight;
            };

        // 3. Лексическое сканирование текста НАЗАД в поисках макроса
        size_t targetMacroStart = std::wstring::npos;
        size_t scanPos = searchOrigin;

        while (scanPos > 0) {
            size_t rfindPos = fileText.rfind(macroName, scanPos);
            if (rfindPos == std::wstring::npos) break;

            if (isTokenValid(rfindPos)) {
                targetMacroStart = rfindPos;
                break;
            }

            if (rfindPos == 0) break;
            scanPos = rfindPos - 1;
        }

        // ЗАЩИТА ОТ ПРОМАХА НА ТЕКУЩЕЙ СТРОКЕ:
        // Если rfind улетел на предыдущие строки (меньше lineStartOffset) или вернул npos,
        // проверяем, нет ли валидного токена eval правее на самой строке rawLine.
        if (targetMacroStart == std::wstring::npos || targetMacroStart < lineStartOffset) {
            size_t forwardPos = lineStartOffset;
            while (forwardPos < nextNL) {
                size_t fPos = fileText.find(macroName, forwardPos);
                if (fPos == std::wstring::npos || fPos >= nextNL) break;
                if (isTokenValid(fPos)) {
                    targetMacroStart = fPos;
                    break;
                }
                forwardPos = fPos + 1;
            }
        }

        // 4. Математический пересчет абсолютного смещения в честные Line и Column
        long preciseLine = 1;
        size_t preciseLineStartOffset = 0;
        size_t scanOffset = 0;

        while (scanOffset < targetMacroStart) {
            size_t nextNLOffset = fileText.find(L'\n', scanOffset);
            if (nextNLOffset != std::wstring::npos && nextNLOffset < targetMacroStart) {
                preciseLine++;
                preciseLineStartOffset = nextNLOffset + 1;
                scanOffset = nextNLOffset + 1;
            }
            else {
                break;
            }
        }

        // ВНИМАНИЕ: Переводим остаток строки в визуальные колонки для финальной привязки к Visual Studio!
        // Внутри GetVisualColumn() честно используется GetVSTabSize(), возвращающий пользовательские 4 пробела.
        std::wstring preciseLineText = fileText.substr(preciseLineStartOffset, targetMacroStart - preciseLineStartOffset);
        long preciseColumn = GetVisualColumn(preciseLineText, preciseLineText.length());

        result.line = preciseLine;
        result.column = preciseColumn;
        result.absoluteOffset = targetMacroStart;
        return result;
    }

    inline std::wstring ReadFileFromDisk(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (!file.is_open()) return L"";

        // Читаем весь файл в string
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        if (content.empty()) return L"";

        // Переводим UTF-8 / ANSI контент в std::wstring для работы с вашими функциями
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, content.c_str(), static_cast<int>(content.size()), NULL, 0);
        if (size_needed <= 0) {
            // Если не UTF-8, пробуем текущую ANSI кодировку
            size_needed = MultiByteToWideChar(CP_ACP, 0, content.c_str(), static_cast<int>(content.size()), NULL, 0);
            if (size_needed <= 0) return L"";
            std::wstring wstr(size_needed, 0);
            MultiByteToWideChar(CP_ACP, 0, content.c_str(), static_cast<int>(content.size()), wstr.data(), size_needed);
            return wstr;
        }

        std::wstring wstr(size_needed, 0);
        MultiByteToWideChar(CP_UTF8, 0, content.c_str(), static_cast<int>(content.size()), wstr.data(), size_needed);
        return wstr;
    }

    static bool baseIsAligned = false;

    inline void AlignDatabaseFastFromDisk() {

        if (baseIsAligned) return;
        baseIsAligned = true;

        auto& fileCompileTree = GetFileCompileTree();
        auto& params = LivePT::getParamDesc();

        Log("================ [LivePT: Поиск eval в файлах] ================");
        Log("[LivePT Init] ",false, true, true); 
        Log("Всего зарегистрировано eval в базе : " + std::to_string(params.size()));

        for (auto& [filePath, fileMap] : fileCompileTree) {
            std::string fileName = filePath.substr(filePath.find_last_of("/\\") + 1);

            std::wstring fileText = ReadFileFromDisk(filePath);
            if (fileText.empty()) {
                Log("[LivePT Disk Init] ", false, true, true);
                Log("Ошибка чтения или пустой файл: " + filePath);
                continue;
            }

            Log("[LivePT File] ", false, true, true);
            Log("Файл: {", false);
            Log(fileName, false, true);
            Log("} (найдено макросов: " + std::to_string(fileMap.size()) + ")");

            for (auto& [staticKey, paramId] : fileMap) {
                if (paramId < 0 || paramId >= static_cast<int>(params.size())) continue;

                auto& p = params[paramId];

                PreciseCoord precise = FindEvalTokenBackward(fileText, staticKey.line, staticKey.column);

                if (precise.absoluteOffset != std::wstring::npos) {
                    p.line = static_cast<int>(precise.line);
                    p.column = static_cast<int>(precise.column);
                    //p.loaded = true;

                    Log("\t", false);
                    Log("-->[FOUND] ", false,true,true);
                    Log("     globalID=" + std::to_string(paramId) +
                        " (localID=" + std::to_string(p.counterID) + ")" +
                        " | Line: " + std::to_string(p.line) +
                        ", Col: " + std::to_string(p.column) +
                        " (raw compiler: " + std::to_string(staticKey.line) + ":" + std::to_string(staticKey.column) + ")");
                }
                else {
                    Log("\t", false);
                    Log("-->[NOT FOUND] ", false, true, true);
                    Log("globalID=" + std::to_string(paramId) +
                        " | Не удалось найти токен eval около " +
                        std::to_string(staticKey.line) + ":" + std::to_string(staticKey.column));
                }
            }
        }
        Log("===============================================================");
    }

    inline void ShiftDatabaseCoordinates(const std::string& targetFile, long targetLine, long visualStartCol, int lineDelta, int columnDelta) {

        auto& params = LivePT::getParamDesc();
        std::string normalizedTarget = LivePT::NormalizePath(targetFile.c_str());

        for (size_t i = 0; i < params.size(); ++i) {
            auto& p = params[i];
            if (p.fileName != normalizedTarget) continue;

            int oldLine = p.line;
            int oldCol = p.column;

            if (lineDelta != 0 && p.line > targetLine) {
                p.line += lineDelta;
            }

            if (columnDelta != 0 && p.line == targetLine && p.column > visualStartCol) {
                p.column += columnDelta;
            }

            if (oldLine != p.line || oldCol != p.column) {
                Log("[LivePT Shift] globalID=" + std::to_string(i) +
                    " (" + p.fileName + ") сдвинут: " +
                    std::to_string(oldLine) + ":" + std::to_string(oldCol) + " -> " +
                    std::to_string(p.line) + ":" + std::to_string(p.column));
            }
        }
    }

} // namespace LivePT
