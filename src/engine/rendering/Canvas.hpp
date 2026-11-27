#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <stdexcept>
#include <algorithm>
#include <functional>

// Core
#include "src/engine/core/Point.hpp"
#include "src/engine/core/Rect.hpp"
#include "src/engine/core/Size.hpp"

#include "src/engine/rendering/ansi.hpp"

// Utils
#include "src/engine/utils.hpp"

using std::string;
using std::cout;
using std::runtime_error;
using std::vector;
using std::size_t;
using std::function;
using std::min;
using std::max;
using std::size_t;
using uint = unsigned int;

// Notes:
// -    Orden estricto de pasos para una correcta actualizacion del buffer.
//          1. Modificar el estado del buffer actual
//          2. Renderizar solo los cambios.
//          3. Actualizar el estado del buffer anterior al nuevo estado del actual.

struct Cell
{
    string ch;
    ANSI::Style style;

    Cell(const string& ch, const ANSI::Style& style) : ch(ch), style(style) {}

    bool operator==(const Cell& other) const
    {
        return ch == other.ch && style == other.style;
    }

    bool operator!=(const Cell& other) const
    {
        return !(*this == other);
    }
};

struct Canvas
{
private:
    const Size size;
    const uint len = size.height * size.width;

    vector<Cell> previous_buffer;
    vector<Cell> buffer;

    // Inicializa el buffer
    void init()
    {
        for (size_t i = 0; i < len; i++)
        {
            previous_buffer.push_back(
                Cell("", ANSI::Style()));

            buffer.push_back(
                Cell(" ", ANSI::Style()));
        }
    }

    // Renderiza unicamente los cambios realizados en el buffer.
    void render()
    {
        for (size_t i = 0; i < len; i++)
        {
            try
            {
                Point p = transform_index_1_to_2_dim(i);
                p = normalize_point_in_ansi(p);

                if (!is_change_cell(i))
                {
                    continue;
                }

                const auto& cell = buffer[i];
                const auto& style = cell.style;

                cout << "\033[" << p.y << ";" << p.x << "H";

                // SGR
                cout << "\033["
                    << static_cast<int>(style.foreground)
                    << ";"
                    << static_cast<int>(style.background);

                if (style.bold)
                {
                    cout << ";1";
                }

                if (style.dim)
                {
                    cout << ";2";
                }

                if (style.italic)
                {
                    cout << ";3";
                }

                if (style.underline)
                {
                    cout << ";4";
                }

                if (style.blink)
                {
                    cout << ";5";
                }

                if (style.rapidBlink)
                {
                    cout << ";6";
                }

                if (style.reverse)
                {
                    cout << ";7";
                }

                if (style.hidden)
                {
                    cout << ";8";
                }

                if (style.strikethrough)
                {
                    cout << ";9";
                }

                if (style.doubleUnderline)
                {
                    cout << ";21";
                }

                cout << "m";

                // Carácter
                cout << cell.ch;

                // Reset
                cout << "\033[0m";
            }
            catch (const std::exception& e)
            {
                return;
            }
        }
    }

    // Transforma un indice del buffer unidimensional en un valor bidimensional.
    Point transform_index_1_to_2_dim(uint index)
    {
        if (size.width == 0)
        {
            throw runtime_error("Error: La cantidad de columnas del buffer es cero.");
        }

        const uint x = index % size.width;
        const uint y = index / size.width;

        return Point(x, y);
    }

    // Transforma un par ordenado en un valor unidimensional perteneciente al buffer.
    uint transform_index_2_to_1_dim(uint x, uint y)
    {
        if (x >= size.width || y >= size.height)
        {
            throw runtime_error("Error(transform_index_2_to_1_dim): El par ordenado `(x, y)` no es una direccion valida en la matriz del buffer. Se asume que el origen es `(0, 0)`.");
        }

        return y * size.width + x;
    }

    // Normaliza un par ordenado, agregando una unidad a cada componente, a escape ANSI.
    // Dado que el origen en ANSI es el par ordenado: (1, 1)
    Point normalize_point_in_ansi(Point point)
    {
        return Point(++point.x, ++point.y);
    }

    // Retorna `true` si la celda en la posicion `index` ha cambiado de valor, `false` caso contrario.
    bool is_change_cell(const uint& index)
    {
        return previous_buffer[index] != buffer[index];
    }

    // Actualiza el estado del buffer anterior al estado actual.
    void update_buffer()
    {
        previous_buffer = buffer;
    }

    // Aplica un closure a cada celda perteneciente a una sub-matriz del buffer.
    void forEach(const Rect& rect, function<void(Cell&)> closure)
    {
        const uint endY = min(rect.origin.y + rect.size.height, size.height);
        const uint endX = min(rect.origin.x + rect.size.width, size.width);

        for (uint y = rect.origin.y; y < endY; y++)
        {
            for (uint x = rect.origin.x; x < endX; x++)
            {
                const uint i = transform_index_2_to_1_dim(x, y);
                closure(buffer[i]);
            }
        }
    }

public:
    Canvas(Size size) : size(size)
    {
        init();
    }

    void drawUTF8(const string& text, const Point& position, const ANSI::Style& style)
    {
        uint start_i;
        try
        {
            start_i = transform_index_2_to_1_dim(position.x, position.y);
        }
        catch (const runtime_error& e)
        {
            return;
            //throw e;
        }

        vector<string> chs = splitUtf8(text);

        for (auto ch : chs)
        {
            // Verifica si la celda no ha cambiado
            if (buffer[start_i].ch == ch && buffer[start_i].style == style)
            {
                start_i += 1;
                continue;
            }

            // Verifica si el contenido de la celda ha cambiado
            if (buffer[start_i].ch != ch)
            {
                buffer[start_i].ch = ch;
            }

            // Verifica si el estilo general de la celda ha cambiado
            if (buffer[start_i].style != style)
            {
                buffer[start_i].style = style;
            }

            start_i += 1;

            // Verifica si el texto alcanzo el limite de su fila. Evita que se desborde.
            // Indica que se desbordo a la siguiente fila.
            if (start_i % size.width == 0)
            {
                return;
            }
        }
    }

    // Limpia el contenido de un espacio bidimensional en el buffer actual.
    void clear(Rect rect)
    {
        for (size_t i = 0; i < rect.origin.y; i++)
        {
            Point origin = { rect.origin.x, rect.origin.y + int(i) };
            clear(origin, rect.size.width);
        }
    }

    // Limpia el contenido de un espacio unidimensional en el buffer actual.
    void clear(Point origin, uint width)
    {
        uint start_i = transform_index_2_to_1_dim(origin.x, origin.y);

        // Evita el desborde en el proceso de eliminado de cada celda.
        uint end_i = transform_index_2_to_1_dim(min(origin.x + width, size.width), origin.y);

        for (size_t i = start_i; i < end_i; i++)
        {
            buffer[i] = Cell{ " ", ANSI::Style() };
        }
    }

    // Limpia todo el contenido del buffer actual.
    void clear()
    {
        for (Cell& cell : buffer)
        {
            cell = Cell{ " ", ANSI::Style() };
        }
    }

    // Renderiza el contenido del canvas en la terminal.
    void show()
    {
        render();
        update_buffer();
        cout.flush();
    }

    void paint(const Rect& rect, ANSI::Background bgColor)
    {
        forEach(rect, [&bgColor](Cell& cell)
            { cell.style.background = bgColor; });
    }

    Size getSize() const
    {
        return size;
    }
};