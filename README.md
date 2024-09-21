# Server API
API to communicate with servers, allowing public and private server support.

## Usage (Developer)
### Getting the server currently in use
To get the currently used server, use `ServerAPI::get()->getCurrentURL()`. Whatever server has the highest priority is returned.<br/>
Example of getting something from the servers, uses Geode's web api
```c++
#include <Geode/binding/GJAccountManager.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/loader/Event.hpp>

#include <km7dev.server_api/include/ServerAPI.hpp>

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
        auto url = ServerAPI::get()->getCurrentURL() + "getGJUserInfo20.php";
        auto req = web::WebRequest();
        req.bodyString(fmt::format("secret=Wmfd2893gb7&targetAccountID={}", GJAccountManager::get()->m_accountID));
        req.userAgent(""),
        m_fields->m_listener.setFilter(req.post(url));
        return true;
    }
};
```
You can also get the current server's priority, if you need it. `ServerAPI::get()->getCurrentPrio()`
### Registering a server
To register a server, it's simple. Just call `ServerAPI::get()->registerURL(url, priority)`. It returns an int, which is the id of the registered server. Store this if you plan to edit or remove your URL.</br>
Here's an example.
```c++
#include <km7dev.server_api/include/ServerAPI.hpp>

class ExampleClass {
    private:
        // Storing the ID for later usage.
        int serverId;
    public: 
        void init()
        {
            // This server will be used if all other servers have a priority less than 10.
            this->serverId = ServerAPI::get()->registerURL("https://my-epic-gd-servers.google.com", 10);
        }
};
```
### Editing a server
To edit a server, there are multiple functions. The ID in all of these is the one given by `ServerAPI::get()->registerURL`.
- `ServerAPI::get()->updateURL(id, url)` - This just updates the URL.
- `ServerAPI::get()->updatePrio(id, priority)` - This just updates the priority. 
- `ServerAPI::get()->updateURLAndPrio(id, url, priority)` - This updates the URL and priority.
```c++
#include <km7dev.server_api/include/ServerAPI.hpp>

class ExampleClass {
    private:
        int serverId;
    public: 
        void myFunc()
        {
            // Updating just the URL
            ServerAPI::get()->updateURL(this->serverId, "https://my-other-epic-servers.bing.com");
            // Updating just the priority
            ServerAPI::get()->updatePriority(this->serverId, 6);
            // Updating the URL and priority
            ServerAPI::get()->updateURL(this->serverId, "https://my-other-epic-servers.bing.com", 6);
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
    auto url = ServerAPI::get()->getUrlById(1);
    // If there isn't a url with this id, it'll return 0
    auto url = ServerAPI::get()->getUrlById(1);
}
```
### Removing a server
If you're done with the server and want to remove it, use `ServerAPI::get()->removeServer(id)`.
```c++
#include <km7dev.server_api/include/ServerAPI.hpp>

class ExampleClass {
    private:
        int serverId;
    public: 
        void myFunc()
        {
            ServerAPI::get()->removeURL(this->serverId);
        }
};
```
### Getting all servers
If you want to have info about the various servers, use `ServerAPI::get()->getAllServers();`
```c++
#include <km7dev.server_api/include/ServerAPI.hpp>

class ExampleClass {
    private:
        int serverId;
    public: 
        void myFunc()
        {
            auto servers = ServerAPI::get()->getAllServers();
            
            // do something with the servers
        }
};
```