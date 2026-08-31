/*==============================================================================

    Event Manage System For OOP [Event_Manager.cpp]

    Author : Choi HyungJoon

==============================================================================*/
#include "Project_Header.h"
#include "Event_Manager.h"

EventManager& EventManager::GetInstance()
{
    static EventManager instance;
    return instance;
}

void EventManager::Subscribe(EventType type, EventCallback callback)
{

    listeners[type].push_back(callback);
}

void EventManager::Fire(EventType type, EventData* data)
{
    // Check Event Type If Has Ever Been Subscribed In The Map Data
    auto it = listeners.find(type);

    // If Subscribed Is Alive?
    if (it != listeners.end())
    {
        // Around Callback Function For Data Send
        for (auto& listener : it->second)
        {
            listener(data);
        }
    }
}