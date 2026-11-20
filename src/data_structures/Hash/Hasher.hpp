#pragma once

#include <random>
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <cstdint>

using std::string;
using std::vector;

using std::uint8_t;
using std::uint64_t;
using std::size_t;

using std::random_device;
using std::mt19937_64;
using std::uniform_int_distribution;

// Transforma un valor de 64 bits al formato 'little-endian'
void littleEndian(uint64_t &value)
{
    uint64_t result = 0;

    // Itera por cada byte y lo reordena
    for (int i = 0; i < 8; ++i)
    {
        result |= ((value >> (i * 8)) & 0xFF) << ((7 - i) * 8);
    }

    value = result;
}

// Estado global de Hasher
class HasherGlobalState
{
private:
    // 'Words' del estado global
    uint64_t v0, v1, v2, v3;

    // Clave de 128 bits en 'little-endian' -- 2 de 64 bits
    uint64_t key[2];

    HasherGlobalState()
    {
        generateKey(key[0]);
        generateKey(key[1]);

        littleEndian(key[0]);
        littleEndian(key[1]);

        v0 = key[0] ^ 0x736f6d6570736575;
        v1 = key[1] ^ 0x646f72616e646f6d;
        v2 = key[0] ^ 0x6c7967656e657261;
        v3 = key[1] ^ 0x7465646279746573;
    }

    // Evita la copia de la instancia
    HasherGlobalState(const HasherGlobalState &) = delete;
    HasherGlobalState &operator=(const HasherGlobalState &) = delete;

    void generateKey(uint64_t &key)
    {
        random_device rd;
        mt19937_64 gen(rd());
        uniform_int_distribution<uint64_t> dis;

        key = dis(gen);
    }

public:
    static HasherGlobalState &shared()
    {
        static HasherGlobalState hs;
        return hs;
    }

    void words(uint64_t &v0, uint64_t &v1, uint64_t &v2, uint64_t &v3)
    {
        v0 = this->v0;
        v1 = this->v1;
        v2 = this->v2;
        v3 = this->v3;
    }
};

// Implementacion estandar de SipHash-2-4
class Hasher
{
private:
    // Estado interno
    uint64_t state = 0;

    // 'words' del estado interno
    uint64_t v0, v1, v2, v3;

    // Constantes de procesamiento
    static constexpr uint8_t c = 2;
    static constexpr uint8_t d = 4;

    // Constante auxiliar
    static constexpr uint8_t eight = 8;

    void compression(const string &input)
    {
        size_t w = std::ceil((input.length() + 1) / 8.0);

        vector<uint64_t> blocks(w);

        buildBlocks(blocks, w, input);

        // Para cada bloque `m_i`:
        //  - Se transforma a 'litte-endian'
        //  - Se opera con el 'word' interno `v3`
        //  - Se realiza `c` iteraciones de `sipRound`
        //  - Se opera con el 'word' interno `v0`
        for (size_t i = 0; i < w; i++)
        {
            littleEndian(blocks[i]);

            v3 ^= blocks[i];

            executeSipRound(c);

            v0 ^= blocks[i];
        }
    }

    void compression(const uint64_t &input)
    {
        constexpr uint8_t w = 2;

        uint64_t blocks[w] = { input, 0x08 };

        // Para cada bloque `m_i`:
        //  - Se transforma a 'litte-endian'
        //  - Se opera con el 'word' interno `v3`
        //  - Se realiza `c` iteraciones de `sipRound`
        //  - Se opera con el 'word' interno `v0`
        for (uint8_t i = 0; i < w; i++)
        {
            littleEndian(blocks[i]);

            v3 ^= blocks[i];

            executeSipRound(c);

            v0 ^= blocks[i];
        }
    }

    void finalization()
    {
        v2 ^= 0xFF;

        executeSipRound(d);

        state = v0 ^ v1 ^ v2 ^ v3;
    }

    // *** Funcion auxiliar de `buildBlocks` ***
    // Inicializa los bloques
    // * No agrega relleno
    void initBlocks(vector<uint64_t> &blocks, const size_t &w, const string &input)
    {
        const size_t len = input.size();

        for (size_t i = 0; i < w; i++)
        {
            blocks[i] = 0; // 0b0000_0000 0000_0000 0000_0000 0000_0000 0000_0000 0000_0000 0000_0000 0000_0000

            // Asigna el codigo 'ascii' de cada caracter a su bloque correspondiente
            //  start_i : índice del primer elemento del bloque actual `i`
            //  end_i   : índice del último elemento del bloque actual `i`
            //  j       : variable - adiciona cada byte, 8 en total, a partir de la constante
            const size_t start_i = i * eight;
            const size_t end_i = std::min(start_i + eight, len);

            for (size_t j = start_i; j < end_i; j++)
            {
                blocks[i] = (blocks[i] << eight) | uint8_t(input[j]);
            }
        }
    }

    // *** Funcion auxiliar de `buildBlocks` ***
    // Rellena el ultimo bloque con ceros hasta completar 8 bytes.
    void padding(vector<uint64_t> &blocks, const size_t &w, const size_t &inputLen)
    {
        // Cantidad de bytes a rellenar.
        // considerando una entrada vacía    : 1 <= p <= 8
        // considerando una entrada no vacía : 1 <= p <= 7
        const uint8_t p = eight - (inputLen % eight);

        for (uint8_t i = 0; i < p; i++)
        {
            blocks[w - 1] = (blocks[w - 1] << eight) | 0x00;
        }
    }

    // *** La transformacion de cada bloque a 'little-endian' se maneja directamente en `compression` ***
    void buildBlocks(vector<uint64_t> &blocks, const size_t &w, const string &input)
    {
        size_t len = input.length();

        initBlocks(blocks, w, input);

        padding(blocks, w, len);

        // Length
        // Adiciona la longitud de la entrada al ultmo bloque `w - 1`
        // En caso la longitud sea mayor al valor máximo representado por un byte sin signo(255),
        // se transoforma dicho valor a uno válido dentro del rango `[0; 2^8 - 1]`
        blocks[w - 1] |= len % 256;
    }

    uint64_t rotateLeft(uint64_t x, int b)
    {
        return (x << b) | (x >> (64 - b));
    }

    void sipRound()
    {
        v0 = (v0 + v1) & 0xFFFFFFFFFFFFFFFF;
        v1 = rotateLeft(v1, 13);
        v1 ^= v0;
        v0 = rotateLeft(v0, 32);

        v2 = (v2 + v1) & 0xFFFFFFFFFFFFFFFF;
        v1 = rotateLeft(v1, 17);
        v1 ^= v2;
        v2 = rotateLeft(v2, 32);

        v2 = (v2 + v3) & 0xFFFFFFFFFFFFFFFF;
        v3 = rotateLeft(v3, 16);
        v3 ^= v2;

        v0 = (v0 + v3) & 0xFFFFFFFFFFFFFFFF;
        v3 = rotateLeft(v3, 21);
        v3 ^= v0;
    }

    void executeSipRound(const uint8_t &amount)
    {
        for (uint8_t i = 0; i < amount; i++)
        {
            sipRound();
        }
    }

public:
    Hasher()
    {
        HasherGlobalState::shared().words(v0, v1, v2, v3);
    }

    void combine(const string &input)
    {
        if (input.length() < 1)
        {
            throw runtime_error("Entrada vacia.");
        }

        compression(input);
        finalization();
    }

    void combine(const uint64_t &input)
    {
        compression(input);
        finalization();
    }

    uint64_t finalize()
    {
        return state;
    }
};