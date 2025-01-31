# Server API
API to communicate with servers, allowing public and private server support.

## Credits
- [GD Noxi](https://www.youtube.com/channel/UCdZjMv0DCgkFQz_lklsYYJw): Fix for NCS Music
- [M41den](https://github.com/m41denx): Fix for extra /

## Usage (Developer)
### [Put this in your mod if you want an optional dependency.](https://github.com/Kingminer7/server-api/edit/main/include/ServerAPIEvents.hpp), or use suggested dependency if what I've been told is correct
Servers are returned as a struct with 3 values: url, prio, and id.<br/>
`server.url` is the actual server url, `server.priority` is the server priority, and `server.id` is the id used internally by Server API.<br/>
### Getting the server currently in use
To get the currently used server, use `ServerAPIEvents::getCurrentServer()`. Whatever server has the highest priority is returned.<br/>
Example of getting something from the servers, uses Geode's web api
```c++
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include <km7dev.server_api/include/ServerAPIEvents.hpp>
// or, if you dont want to require dep and have the header in your source
#include "ServerAPIEvents.hpp"

using namespace geode::prelude;

class $modify(MenuLayer) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        m_fields->m_listener.bind([] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                log::info("{}", res->string().unwrapOr("Request failed."));
            }
        });

        // structure: server + "endpoint"
        auto url = ServerAPIEvents::getCurrentServer().url + "getGJUserInfo20.php";
        auto req = web::WebRequest();
        req.bodyString(fmt::format("secret=Wmfd2893gb7&targetAccountID={}", GJAccountManager::get()->m_accountID));
        req.userAgent(""),
        m_fields->m_listener.setFilter(req.post(url));
        return true;
    }
};
```
### Registering a server
To register a server, it's simple. Just call `ServerAPIEvents::registerServer(url, priority)`. It returns an int, which is the id of the registered server. Store this if you plan to edit or remove your URL.</br>
Here's an example.
```c++
#include <km7dev.server_api/include/ServerAPIEvents.hpp>
// or, if you dont want to require dep and have the header in your source
#include "ServerAPIEvents.hpp"

class ExampleClass {
    private:
        // Storing the server for later usage. Changing values here ***does not*** update values in Server API unles you use updateServer (documented below)
        ServerAPIEvents::Server server;
    public: 
        void init()
        {
            // This server will be used if all other servers have a priority less than 10.
            this->server = ServerAPIEvents::registerServer("https://my-epic-gd-servers.google.com", 10);
        }
};
```
### Editing a server
To edit a server, use `ServerAPIEvents::updateServer` with parameters based on what you're updating. The ID in these is the one given in server structs.
- `ServerAPIEvents::updateServer(id, url)` - This just updates the URL.
- `ServerAPIEvents::updateServer(id, priority)` - This just updates the priority. 
- `ServerAPIEvents::updateServer(id, url, priority)` - This updates the URL and priority.
- `ServerAPIEvents::updateServer(server)` - This updates the URL and priority, although using a Server struct.
```c++
#include <km7dev.server_api/include/ServerAPIEvents.hpp>
// or, if you dont want to require dep and have the header in your source
#include "ServerAPIEvents.hpp"

class ExampleClass {
    private:
        ServerAPIEvents::Server server;
    public: 
        void myFunc()
        {
            // Updating just the URL
            ServerAPIEvents::updateServer(this->server.id, "https://my-other-epic-servers.bing.com");
            // Updating just the priority
            ServerAPIEvents::updateServer(this->server.id, 6);
            // Updating the URL and priority
            ServerAPIEvents::updateServer(this->server.id, "https://my-other-epic-gd-servers.bing.com", 10);
            // Updating the URL and priority with a server struct
            ServerAPIEvents::updateServer({
                id: this->server.id,
                url: "https://my-other-epic-gd-servers.bing.com",
                priority: 6,
            });
        }
};
```
### Getting info from an ID
You can get the URL with `ServerAPI::get()->getURLById(id);`. If there isn't a URL with that id, it'll return an empty string.<br>
You can get the URL with `ServerAPI::get()->getPrioById(id);`. If there isn't a URL with that id, it'll return zero.
```c++
void myFunc()
{
    // Getting the URL.
    // If there isn't a url with this id, it'll return ""
    auto url = ServerAPIEvents::getServerById(1).url;
    // If there isn't a server with this id, it'll return 0
    auto prio = ServerAPIEvents::getServerById(1).priority;
}
```
### Removing a server
If you're done with the server and want to remove it, use `ServerAPI::get()->removeServer(id)`.
```c++
#include <km7dev.server_api/include/ServerAPI.hpp>
// or, if you dont want to require dep and have the header in your source
#include "ServerAPIEvents.hpp"

class ExampleClass {
    private:
        ServerAPIEvents::Server server;
    public: 
        void myFunc()
        {
            ServerAPIEvents::removeServer(this->server.id);
        }
};
```
### Getting all servers
If you want to have info about the various servers, use `ServerAPI::get()->getAllServers();`
```c++
#include <km7dev.server_api/include/ServerAPI.hpp>
// or, if you dont want to require dep and have the header in your source
#include "ServerAPIEvents.hpp"

class ExampleClass {
    public: 
        void myFunc()
        {
            auto servers = ServerAPIEvents::getRegisteredServers();
            
            // do something with the servers
        }
};
```
