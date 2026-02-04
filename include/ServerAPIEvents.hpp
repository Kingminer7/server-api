#pragma once

// std
#include <string>
#include <utility>

#include <map>
#include <unordered_map>

// geode
#include <Geode/loader/Mod.hpp>
#include <Geode/loader/Event.hpp>

#ifdef GEODE_IS_WINDOWS
#ifdef KM7DEV_SERVER_API_EXPORTING
#define SERVER_API_DLL __declspec(dllexport)
#else
#define SERVER_API_DLL __declspec(dllimport)
#endif
#else
#define SERVER_API_DLL __attribute__((visibility("default")))
#endif

namespace ServerAPITrust {
/// @brief Enum representing how trusted a Mod is to clean up after itself.
///
/// ServerAPI switches servers on a dime without warning other mods which
/// may affect crucial systems relying on the server GD is connected to
/// staying the same. While one mod may switch servers while preventing
/// GD from saving player save data to disk, another may not, so this
/// enum represents how trusted a mod is to use ServerAPI safely.
enum class TrustLevel {
    Untrusted = 0, Trusted, HighlyTrusted
};

/// @brief Lookup Table of trusted mods, you may change this in your copy of this header.
::std::unordered_map<::std::string, TrustLevel> trustedModsLUT = {
    {"lblazen.gdps_hub", TrustLevel::HighlyTrusted},
    {"km7dev.gdps-switcher", TrustLevel::HighlyTrusted}
};

/// @brief Gets the TrustLevel associated with a mod using its ID
/// @param modID Mod's ID from its mod.json
/// @return Its trurst level as a TrustLevel enum
inline TrustLevel trustLevelFor(::std::string modID) {
    auto it = trustedModsLUT.find(modID);
    if (it == trustedModsLUT.end()) {
        return TrustLevel::Untrusted;
    }
    return it->second;
}

/// @brief Gets the TrustLevel associated with a mod
/// @param mod Pointer to the mod
/// @return Its trurst level as a TrustLevel enum
inline TrustLevel trustLevelFor(::geode::Mod* mod) {
    return trustLevelFor(mod->getID());
}

/// @param modID Mod's ID from its mod.json
/// @return True if the mod is HighlyTrusted
inline bool isHighlyTrusted(::std::string modID) {
    auto it = trustedModsLUT.find(modID);
    if (it == trustedModsLUT.end()) {
        return false;
    }
    return it->second == TrustLevel::HighlyTrusted;
}

/// @param mod Pointer to the mod
/// @return True if the mod is HighlyTrusted
inline bool isHighlyTrusted(::geode::Mod* mod) {
    return isHighlyTrusted(mod->getID());
}

/// @param modID Mod's ID from its mod.json
/// @return True if the mod is Trusted or HighlyTrusted
inline bool isTrusted(::std::string modID) {
    auto it = trustedModsLUT.find(modID);
    if (it == trustedModsLUT.end()) {
        return false;
    }
    return it->second >= TrustLevel::Trusted;
}

/// @param mod Pointer to the mod
/// @return True if the mod is Trusted or HighlyTrusted
inline bool isTrusted(::geode::Mod* mod) {
    return isTrusted(mod->getID());
}
} // namespace ServerAPITrust

// @brief Represents the server being updated to and by who
struct ServerUpdatingEventData {    
private:
    const ::geode::Mod* updater;
    const ::std::string& url;
public:
    ServerUpdatingEventData(::geode::Mod* updater, const std::string& url) : updater(updater), url(url) {};
    const ::std::string& getUrl() const { return url; }
    const ::geode::Mod* getUpdater() const { return updater; }
    const ::geode::Mod* whoUpdatedTheFuckingServer() const { return updater; }
};

struct ServerUpdatingEvent : ::geode::Event<ServerUpdatingEvent, void(ServerUpdatingEventData*)> {
  using Event::Event;
};

/// @brief Represents server in use by ServerAPI
struct GetCurrentServerEventData {
  GetCurrentServerEventData() {}

  int m_id = 0;
  ::std::string m_url = "";
  int m_prio = 0;
};

struct GetCurrentServerEvent : ::geode::Event<GetCurrentServerEvent, bool(GetCurrentServerEventData*)> {
  using Event::Event;
};

/// @brief Represents server info acquired from its ID
struct GetServerByIdEventData {
  GetServerByIdEventData(int id) : m_id(::std::move(id)) {}

  int m_id = 0;
  ::std::string m_url = "";
  int m_prio = 0;
};

struct GetServerByIdEvent : ::geode::Event<GetServerByIdEvent, bool(GetServerByIdEventData*)> {
  using Event::Event;
};

/// @brief Will register a server with a URL and a priority.
struct RegisterServerEventData {
  RegisterServerEventData(::std::string url, int priority)
      : m_url(::std::move(url)), m_priority(::std::move(priority)) {}

  int m_id;
  ::std::string m_url = "";
  int m_priority = 0;
};

struct RegisterServerEvent : ::geode::Event<RegisterServerEvent, bool(RegisterServerEventData*)> {
  using Event::Event;
};

/// @brief Will update a server's information; requires server's ID
struct UpdateServerEventData {
  UpdateServerEventData(int id, ::std::string url, int priority)
      : m_id(::std::move(id)), m_url(::std::move(url)),
        m_priority(::std::move(priority)) {}
  UpdateServerEventData(int id, ::std::string url)
      : m_id(::std::move(id)), m_url(::std::move(url)) {}
  UpdateServerEventData(int id, int priority)
      : m_id(::std::move(id)), m_priority(::std::move(priority)) {}

  int m_id = 0;
  ::std::string m_url = "";
  int m_priority = 0;
};

struct UpdateServerEvent : ::geode::Event<UpdateServerEvent, bool(UpdateServerEventData*)> {
  using Event::Event;
};

/// @brief Will remove a server from the registry; requires server's ID
struct RemoveServerEventData {
  RemoveServerEventData(int id) : m_id(::std::move(id)) {}

  int m_id = 0;
};

struct RemoveServerEvent : ::geode::Event<RemoveServerEvent, bool(RemoveServerEventData*)> {
  using Event::Event;
};

/// @brief Represents base (https) URL for built-in servers (boomlings.com on vanilla GD)
struct GetBaseUrlEventData {
  GetBaseUrlEventData() {}

  ::std::string m_url = "";
};

struct GetBaseUrlEvent : ::geode::Event<GetBaseUrlEvent, bool(GetBaseUrlEventData*)> {
  using Event::Event;
};

/// @brief Represents secondary (http) URL for built-in servers (boomlings.com on vanilla GD)
struct GetSecondaryUrlEventData {
  GetSecondaryUrlEventData() {}

  ::std::string m_url = "";
};

struct GetSecondaryUrlEvent : ::geode::Event<GetSecondaryUrlEvent, bool(GetSecondaryUrlEventData*)> {
  using Event::Event;
};

/// @brief Represents entire server info registry
///
/// Server registry is a map of ints (IDs) to a pair of string and int (URL and priority)
struct GetRegisteredServersEventData {
  GetRegisteredServersEventData() {}

  ::std::map<int, ::std::pair<::std::string, int>> m_servers = {};
};

struct GetRegisteredServersEvent : ::geode::Event<GetRegisteredServersEvent, bool(GetRegisteredServersEventData*)> {
  using Event::Event;
};

// thx prevter for the help

namespace ServerAPIEvents {

/// @brief Represents server information
struct Server {
    /// @brief Handle for server info
    int id;
    /// @brief URL of the server (duh)
    ::std::string url;
    /// @brief ServerAPI will use the server with the highest priority for GD's online features
    int priority;
};

/// @brief Gets copy of the server currently in use by ServerAPI
/// @return Server struct representing server currently in use by ServerAPI
inline Server getCurrentServer() {
  auto event = GetCurrentServerEventData();
  GetCurrentServerEvent().send(&event);
  return {event.m_id, event.m_url, event.m_prio};
}

/// @brief Gets copy of server information from its ID
/// @return Server struct representing the server with the specified ID
inline Server getServerById(int id) {
  auto event = GetServerByIdEventData(id);
  GetServerByIdEvent().send(&event);
  return {event.m_id, event.m_url, event.m_prio};
}

/// @brief Registers a server for ServerAPI to use
/// @param url The server's URL (e.g. https://www.boomlings.com/)
/// @param priority Its priority with respect to other servers competing for GD to connect to
/// @return Server struct representing the newly registered server
inline Server registerServer(::std::string url, int priority = 0) {
  auto event = RegisterServerEventData(url, priority);
  RegisterServerEvent().send(&event);
  return {event.m_id, event.m_url, event.m_priority};
}

/// @brief Updates an existing server
/// @param server The information to update the server with, the ID being a handle to the already existing server
inline void updateServer(Server server) {
  auto event = UpdateServerEventData(server.id, server.url, server.priority);
  UpdateServerEvent().send(&event);
}

/// @brief Updates an existing server
/// @param id Handle to an already existing server
/// @param url The new url for the server to use
/// @param priority The new priority for ServerAPI
inline void updateServer(int id, ::std::string url, int priority) {
  auto event = UpdateServerEventData(id, url, priority);
  UpdateServerEvent().send(&event);
}

/// @brief Updates an existing server without updating its priority
/// @param id Handle to an already existing server
/// @param url The new url for the server to use
inline void updateServer(int id, ::std::string url) {
  auto event = UpdateServerEventData(id, url);
  UpdateServerEvent().send(&event);
}

/// @brief Updates an existing server without updating its URL
/// @param id Handle to an already existing server
/// @param priority The new priority for ServerAPI
inline void updateServer(int id, int priority) {
  auto event = UpdateServerEventData(id, priority);
  UpdateServerEvent().send(&event);
}

/// @brief Removes a server from the registry
/// @param id Handle to the server to remove
inline void removeServer(int id) {
  auto event = RemoveServerEventData(id);
  RemoveServerEvent().send(&event);
}

/// @brief Gets the base (https) URL for built-in servers
/// @return Base (https) URL for built-in servers (boomlings.com on vanilla GD)
inline ::std::string getBaseUrl() {
  auto event = GetBaseUrlEventData();
  GetBaseUrlEvent().send(&event);
  return event.m_url;
}

/// @brief Gets the secondary (http) URL for built-in servers
/// @return Secondary (http) URL for built-in servers (boomlings.com on vanilla GD)
inline ::std::string getSecondaryUrl() {
  auto event = GetSecondaryUrlEventData();
  GetSecondaryUrlEvent().send(&event);
  return event.m_url;
}

/// @brief Gets a copy the entire registry of servers
/// @return Map of int (ID) and pair of string (URL) and int (Priority)
inline ::std::map<int, ::std::pair<::std::string, int>> getRegisteredServers() {
  auto event = GetRegisteredServersEventData();
  GetRegisteredServersEvent().send(&event);
  return event.m_servers;
}
} // namespace ServerAPIEvents
