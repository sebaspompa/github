#pragma once

#include <string>
#include <algorithm>

#include "src/engine/utils.hpp"
#include "src/engine/rendering/Canvas.hpp"

using std::string;

namespace DTool
{
    // Obtiene la posición X necesaria para centrar un texto
    // dentro de un espacio de ancho determinado.
    inline unsigned int centerX(const string& text, unsigned int width)
    {
        const unsigned int textWidth = splitUtf8(text).size();

        if (width <= textWidth)
        {
            return 0;
        }

        return (width - textWidth) / 2;
    }

    // Calcula la posición X para centrar un texto
    // respecto al ancho total del Canvas.
    inline unsigned int centerCanvasX(const Canvas& canvas, const string& text)
    {
        return centerX(text, canvas.getSize().width);
    }

    // Calcula la posición X para centrar un texto
    // dentro de una región específica del Canvas.
    inline unsigned int centerRegionX(const string& text, unsigned int regionX, unsigned int regionWidth)
    {
        return regionX + centerX(text, regionWidth);
    }

    // Calcula la posición de un texto centrado en una región.
    inline Point centeredPosition(const string& text, unsigned int y, unsigned int regionX, unsigned int regionWidth)
    {
        return { centerRegionX(text, regionX, regionWidth), y };
    }

    // Calcula la posición de un texto centrado en el Canvas.
    inline Point canvasCenteredPosition(const Canvas& canvas, const string& text, unsigned int y)
    {
        return { centerCanvasX(canvas, text), y };
    }

    // Comprueba si una coordenada vertical está dentro
    // de los límites del Canvas.
    inline bool isValidY(unsigned int y, unsigned int height)
    {
        return y < height;
    }

    // Comprueba si una posición se encuentra dentro
    // de los límites del Canvas.
    inline bool isInside(const Point& position, unsigned int width, unsigned int height)
    {
        return position.x < width && position.y < height;
    }

    // Calcula el ancho máximo disponible dentro de una región.
    inline unsigned int availableWidth(unsigned int totalWidth, unsigned int margin)
    {
        return totalWidth > margin * 2 ? totalWidth - margin * 2 : 0;
    }

    // Calcula una posición centrada verticalmente.
    inline unsigned int centerY(unsigned int totalHeight, unsigned int contentHeight)
    {
        return totalHeight > contentHeight ? (totalHeight - contentHeight) / 2 : 0;
    }

    // Calcula una posición centrada verticalmente,
    // garantizando un margen superior mínimo.
    inline unsigned int centerY(unsigned int totalHeight, unsigned int contentHeight, unsigned int minTopMargin)
    {
        return std::max(minTopMargin, centerY(totalHeight, contentHeight));
    }

    // Calcula un ancho proporcional al Canvas,
    // limitado por un ancho mínimo y máximo.
    inline unsigned int proportionalWidth(unsigned int totalWidth, unsigned int percentage, unsigned int minWidth, unsigned int maxWidth)
    {
        unsigned int width = totalWidth * percentage / 100;

        width = std::min(width, maxWidth);

        if (totalWidth >= minWidth)
        {
            width = std::max(width, minWidth);
        }

        return std::min(width, totalWidth);
    }

    // Calcula una posición X para centrar una región
    // horizontalmente dentro del Canvas.
    inline unsigned int centerRegion(unsigned int totalWidth, unsigned int regionWidth)
    {
        return totalWidth > regionWidth ? (totalWidth - regionWidth) / 2 : 0;
    }

    // Calcula una posición dentro de una región
    // utilizando un desplazamiento vertical.
    inline unsigned int offsetY(unsigned int originY, unsigned int offset)
    {
        return originY + offset;
    }
}
