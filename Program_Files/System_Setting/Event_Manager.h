/*==============================================================================

	Event Manage System For OOP [Event_Manager.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef EVENT_MANAGER_H
#define EVENT_MANAGER_H
#include <functional>
#include <unordered_map>
#include "Event_Types.h"

// Event Manager Class
class EventManager
{
public:
    // Define Ponter
    using EventCallback = std::function<void(EventData*)>;

    static EventManager& GetInstance();

    // Event Subscribe
    void Subscribe(EventType type, EventCallback callback);

    // Event Fire
    void Fire(EventType type, EventData* data = nullptr);

private:
    EventManager() = default;
    ~EventManager() = default;

    // For Single
    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    // Callback Map For Subscribe Each Event Type
    std::unordered_map<EventType, std::vector<EventCallback>> listeners;
};

#endif // EVENT_MANAGER_H