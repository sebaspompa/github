#pragma once
#include <string>
#include <optional>
#include <functional>
#include <stdexcept>

using std::optional;
using std::function;
using std::runtime_error;
using std::string;

// Esta estructura almacena el `id` del usuario activo en el sistema.
// Define un unico punto de acceso global a la sesion del usuario en el sistema.
// Patrón de diseño 'Singleton' aplicado por Google Assistant.
struct SessionService
{
private:
    // Constructor privado para evitar la creación externa con 'new' o de forma directa.
    SessionService() {}

    ~SessionService() = default;

    // El `id` del usuario activo representa la sesión.
    optional<string> userId;

    // Permite notificar los cambios de estado de la sesión(activo o inactivo).
    function<void(bool)> onChangedSession;

public:
    // Eliminar el constructor de copia y el operador de asignación para evitar que se duplique la instancia.
    SessionService(const SessionService &) = delete;
    SessionService &operator=(const SessionService &) = delete;

    // El uso de un observador como intermediario, asegura que implemente el metodo requerido.
    // template <SessionObserver Observer>
    // void init(Observer &observer)
    // {
    //     onCallback = [&observer](bool isActive)
    //     {
    //         observer.onChangedSession(isActive);
    //     };
    // }

    // ***Este metodo solo puede ser invocado por la estructura `App`***
    // Ejecuta la funcion especificada pasandole el estado de la sesion cuando esta es creada o revocada.
    void init(function<void(bool)> onChangedSession)
    {
        this->onChangedSession = onChangedSession;
    }

    // Método estático para acceder a la única instancia.
    static SessionService &instance()
    {
        // Se inicializa solo la primera vez que se llama a este método.
        // Es "thread-safe" por defecto en C++11.
        static SessionService _instance;
        return _instance;
    }

    // Crea una sesión en base al `id` del usuario.
    // Lanza error en caso ya exista una sesión.
    void create(string userId)
    {
        if (!this->userId.has_value())
        {
            this->userId = userId;
            onChangedSession(true);
        }
        else
        {
            throw runtime_error("Se ha intentado crear una sesión cuando existía otra activa.");
        }
    }

    // Revoca la sesión activa, en caso exista.
    void revoke()
    {
        if (userId.has_value())
        {
            userId = std::nullopt;
            onChangedSession(false);
        }
    }

    // Retorna la sesión activa.
    // Lanza error en caso no exista.
    string session() const
    {
        try
        {
            return userId.value();
        }
        catch(const std::exception& e)
        {
            throw runtime_error("Se ha intentado obtener el `id` de un usuario en una sesión inactiva.");
        }
    }

    // Retorna `true` si existe una sesión activa, `false` si no existe.
    bool isActive() const
    {
        return userId.has_value();
    }
};