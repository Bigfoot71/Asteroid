#ifndef ASTEROID_COMPONENT_TEXT_BOX_HPP
#define ASTEROID_COMPONENT_TEXT_BOX_HPP

#include <rayflex.hpp>

namespace Asteroid {

    class TextBox
    {
      private:
        static constexpr float padding = 16;

      private:
        struct Line
        {
            std::string text;
            raylib::Vector2 size;
        };

      private:
        std::vector<Line> lines;
        raylib::Rectangle bounds;   // Bounds de la text box
        raylib::Font *font{};       // Pointeur vers la font à utiliser
        Vector2 cursor;             // Curseur position text virutel (x=char - y=line)
        float fontSize;             // Taille de la police d'ecriture
        float spacing;              // Espacement des lettres et des retours à la ligne
        float speed;                // Lettres par secondes
        bool finish;

      public:
        TextBox() = default;

        // Speed: Average letter per word in English ~5, average word read per second in English ~5 (?)
        TextBox(const Rectangle& bounds, const std::string& text, raylib::Font* font = nullptr, float fontSize = 24, float spacing = 2.4f, float speed = 25)
        {
            Init(bounds, text, font, fontSize, spacing, speed);
        }

        void Init(const Rectangle& bounds, const std::string& text, raylib::Font* font = nullptr, float fontSize = 24, float spacing = 2.4f, float speed = 25)
        {
            SetText(text, bounds, font, fontSize, spacing);
            this->speed = speed;
        }

        void SetText(const std::string& text, const raylib::Rectangle& bounds = {}, raylib::Font* font = nullptr, float fontSize = 0, float spacing = 0)
        {
            if (bounds.GetSize() != Vector2{}) this->bounds = bounds;
            if (fontSize) this->fontSize = fontSize;
            if (spacing) this->spacing = spacing;
            if (font) this->font = font;

            cursor = {};
            finish = false;

            lines.clear(); // keep capacity
            lines.push_back({});

            char word[64];
            float wordX = this->bounds.x + padding;
            auto start = &text[0];
            auto end = start + text.size();

            // Word iteration
            while (start < end)
            {
                // Ignore spaces at the beginning
                while (*start == ' ') start++;

                // Find the end of the word
                const char* wordEnd = start;
                while (*wordEnd != ' ' && *wordEnd != '\0') ++wordEnd;

                // Word processing
                if (start < wordEnd)
                {
                    std::size_t len = wordEnd - start;
                    if (len >= 62) { start = wordEnd; continue; }; // overflow

                    std::copy(start, wordEnd, word);
                    word[len] = ' ', word[len+1] = '\0';

                    Vector2 size = MeasureTextEx(font ? *font : GetFontDefault(), word, fontSize, spacing);

                    if (wordX + size.x > this->bounds.x + this->bounds.width - padding)
                    {
                        wordX = this->bounds.x + padding;
                        lines.push_back({});
                    }

                    lines.back().text += word;
                    lines.back().size += size;
                    wordX += size.x;
                }

                // Moving pointer to end of pointer
                start = wordEnd;
            }
        }

        void Finish()
        {
            cursor.x = lines.back().text.size() - 1;
            cursor.y = lines.size() - 1;
            finish = true;
        }

        bool IsFinish() const
        {
            return finish;
        }

        bool Update(float dt)
        {
            int charCount = lines[static_cast<int>(cursor.y)].text.size();

            if (!finish && cursor.x < charCount)
            {
                cursor.x += speed * dt;

                if (cursor.x >= charCount)
                {
                    finish = cursor.y == lines.size() - 1;
                    if (!finish) cursor.x = 0, cursor.y++;
                }
            }

            return finish;
        }

        void Draw() const
        {
            DrawRectangleRounded(bounds, 0.25f, 9, { 0, 0, 0, 63 });
            DrawRectangleRoundedLines(bounds, 0.25f, 9, 3, WHITE);

            Vector2 pos = { bounds.x + padding, bounds.y + padding };

            for (int i = 0; i < cursor.y; i++)
            {
                DrawTextEx(font ? *font : GetFontDefault(), lines[i].text.c_str(), pos, fontSize, spacing, WHITE);
                pos.y += fontSize + spacing;
            }

            const char *lastLine = TextSubtext(lines[static_cast<int>(cursor.y)].text.c_str(), 0, cursor.x);
            DrawTextEx(font ? *font : GetFontDefault(), lastLine, pos, fontSize, spacing, WHITE);
        }

        void DrawCentered() const
        {
            DrawRectangleRounded(bounds, 0.25f, 9, { 0, 0, 0, 95 });
            DrawRectangleRoundedLines(bounds, 0.25f, 9, 3, WHITE);

            const float totalHeight = bounds.height - 2 * padding;
            float posY = bounds.y + padding + (totalHeight - lines.size() * (fontSize + spacing)) * 0.5f;

            for (int i = 0; i < cursor.y; i++)
            {
                auto &line = lines[i];
                DrawTextEx(font ? *font : GetFontDefault(), line.text.c_str(), { bounds.x + (bounds.width - line.size.x) * 0.5f, posY }, fontSize, spacing, WHITE);
                posY += fontSize + spacing;
            }

            auto &lastLine = lines[static_cast<int>(cursor.y)];
            const char *text = TextSubtext(lastLine.text.c_str(), 0, cursor.x);
            DrawTextEx(font ? *font : GetFontDefault(), text, { bounds.x + (bounds.width - lastLine.size.x) * 0.5f, posY }, fontSize, spacing, WHITE);
        }
    };

}

#endif //ASTEROID_COMPONENT_TEXT_BOX_HPP