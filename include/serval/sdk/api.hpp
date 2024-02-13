#ifndef SERVAL_SDK__API_HPP
#define SERVAL_SDK__API_HPP

#include "types.hpp"
#include "type_utils.hpp"
#include <magic_enum/magic_enum.hpp>
#include <spdlog/spdlog.h>

class ImGuiContext;

/**
 * @brief Public API to the engine
 * 
 */

namespace serval {
    // API's
    class ExtensionInit;
    class TaskSetup;
    class Init;
    class SystemSetup;
    class GameSetup;
    class Runtime;

    // Streams
    class StreamWriter;
    class StreamReader;

    // Commands
    class CommandReader;

    // Enums
    enum class StreamWriterAccess {
        Single,
        Multiple,
    };

    /**
     * @brief Extension initialisation
     * 
     */
    struct ExtensionInit {
        Init* engine_init_api;
        const std::shared_ptr<spdlog::logger> logger;
        ImGuiContext* imgui_context;
    };
}

/**
 * @brief Public API for setting up tasks and their resource access
 * 
 */
class serval::TaskSetup {
public:
    class TaskBuilder {
    public:
        TaskBuilder (TaskSetup* api, const char* name) : m_api(api), m_task_name(name) {}

        /**
         * @brief Declare that the task read-only accesses to a resource
         * 
         * @param resource The name of the resource
         * @return TaskBuilder& 
         */
        TaskBuilder& ro (serval::Id resource) {
            m_api->add_ro_resource(m_task_name, resource);
            return *this;
        }

        /**
         * @brief Declare that the task read-write accesses to a resource
         * 
         * @param resource The name of the resource
         * @return TaskBuilder& 
         */
        TaskBuilder& rw (serval::Id resource) {
            m_api->add_rw_resource(m_task_name, resource);
            return *this;
        }

        /**
         * @brief Declare that the task acts as a sync point
         * WARNING: This is an advanced feature that may have unexpected impact on the scheduling order
         * 
         * @return TaskBuilder& 
         */
        TaskBuilder& sync () {
            m_api->add_sync_point(m_task_name);
            return *this;
        }

        /**
         * @brief Helper function to add resources such that `task_name` waits for `task_dependencies` to complete before executing
         * NOTE: Any tasks added before `task_name` will run (and complete) before `task_name` is run, while any task added after `task_name`
         * will run after `task_name` has run (and completed)    
         * 
         * @tparam Args 
         * @param resource A unique name used to synchronise
         * @param dependency_tasks The dependency tasks to wait for
         * @return TaskBuilder& 
         */
        template <typename... Args>
        TaskBuilder& wait_for (serval::Id resource, Args... dependency_tasks) {
            ((m_api->add_ro_resource(dependency_tasks, resource)), ...);
            m_api->add_rw_resource(m_task_name, resource);
            return *this;
        }
    private:
        TaskSetup* m_api;
        const char* m_task_name;
    };

    /**
     * @brief Add a new task to a scheduler
     * 
     * @param scheduler_name The name of the scheduler to add the task to
     * @param task_name The name of the task to add
     * @param task The delegate to execute
     * @return TaskBuilder The object allowing the declaration of resource access
     */
    TaskBuilder addTask (const char* scheduler_name, const char* task_name, serval::Task task) {
        add_task(scheduler_name, task_name, task);
        return {this, task_name};
    }

    /**
     * @brief Add a new task to a scheduler
     * Allows binding memer functions directly: `setup->addTask<&Foo::bar>("scheduler", "task", this);`
     * 
     * @tparam Func The function to bind
     * @tparam Instance 
     * @param scheduler_name The name of the scheduler to add the task to
     * @param task_name The name of the task to add
     * @param instance The instance of the object to which Func belongs
     * @return TaskBuilder The object allowing the declaration of resource access
     */
    template <auto Func, typename Instance>
    TaskBuilder addTask (const char* scheduler_name, const char* task_name, Instance* instance) {
        serval::Task task;
        task.connect<Func>(instance);
        add_task(scheduler_name, task_name, task);
        return {this, task_name};
    }

    /**
     * @brief Remove a task from a scheduler
     * 
     * @param scheduler_name The name of the scheduler to remove the task from
     * @param task_name The name of the task to remove
     */
    virtual void removeTask (const char* scheduler_name, const char* task_name) = 0;
private:
    virtual void add_task (const char* scheduler_name, const char* task_name, serval::Task task) = 0;
    virtual void add_ro_resource (const char* task_name, serval::Id resource) = 0;
    virtual void add_rw_resource (const char* task_name, serval::Id resource) = 0;
    virtual void add_sync_point (const char* task_name) = 0;
    friend class TaskBuilder;
};


/**
 * @brief Public API for initialising the engine
 * 
 */
class serval::Init : virtual public serval::TaskSetup {
public:
    virtual ~Init () {}

    /**
     * @brief Create a new fixed-frequency task scheduler
     * 
     * `scheduler_name` must be unique across all extensions and the engines internal schedulers.
     * If a scheduler with the same name already exists, the program will log an error and terminate.
     * 
     * @param scheduler_name The name of the scheduler
     * @param interval_seconds The execution interval in seconds
     * @return serval::ID A hashed id of the scheduler name
     */
    virtual serval::Id addScheduler (const char* scheduler_name, float interval_seconds) = 0;

    /**
     * @brief Register a new game state class with the engine
     * 
     * `class_name` must be unique across all registered game state classes from all extensions. 
     * If a class with the same name already exists, the program will log an error and terminate.
     *  
     * @tparam Events The states serval::StateEvents implementation class
     * @param class_name The name of the game state class
     * @return serval::ID @return serval::ID A hashed id of the state class name
     */
    template <typename Events>
    serval::Id addGameStateClass (const char* class_name) {
        static_assert(std::is_base_of<serval::StateEvents, Events>::value, "Events must be derived from StateEvents");
        return add_game_state_class(class_name, serval::makeFactoryFn<Events>());
    }

    /**
     * @brief Unregister a game state class from the engine
     * 
     * @param class_name The name of the game state class
     */
    virtual void removeGameStateClass (const char* class_name) = 0;

    /**
     * @brief Register a new system with the engine
     * 
     * `system_name` must be unique across all registered systems from all extensions.
     * If a system with the same name already exists, the program will log an error and terminate.
     * 
     * @param system_name The name of the system
     * @param events The events receiver for the system
     * @return serval::ID A hashed id of the system name
     */
    template <typename System>
    serval::Id addSystem (const char* system_name) {
        static_assert(std::is_base_of<serval::SystemEvents, Class>::value, "System must be derived from SystemEvents");
        return add_system(system_name, serval::makeFactoryFn<System>());
    }

    /**
     * @brief Unregister a system from the engine
     * 
     * @param system_name The name of the system
     */
    virtual void removeSystem (const char* system_name) = 0;

    /**
     * @brief Register a new command stream with the engine
     * 
     * @param stream_name The name of the stream
     * @return const serval::CommandReader&  The reader which can access commands written to the stream
     */
    virtual const serval::CommandReader& addCommandStream (const char* stream_name) = 0;

    /**
     * @brief Create a writable notification stream
     * NOTE: Streams with access set to serval::StreamWriterAccess::Single must not write from multiple tasks
     * 
     * @param stream_name The name of the stream to create
     * @param access Single for single-writer streams and Multiple for multi-writer streams
     * @return serval::StreamWriter& The stream writer which can write to this stream
     */
    template <typename T=void>
    serval::StreamWriter& addNotificationStream (const char* stream_name, serval::StreamWriterAccess access) {
        return add_notification_stream(stream_name, magic_enum::enum_underlying(access));
    }

private:
    virtual serval::Id add_game_state_class (const char* class_name, serval::FactoryFn<serval::StateEvents> factory) = 0;
    virtual serval::Id add_system (const char* system_name, serval::FactoryFn<serval::SystemEvents> factory) = 0;
    virtual serval::StreamWriter& add_notification_stream (const char* stream_name, magic_enum::underlying_type_t<serval::StreamWriterAccess> access) = 0;
};


/**
 * @brief Public API to set up a system
 * 
 */
class serval::SystemSetup : public serval::TaskSetup {
public:
    virtual ~SystemSetup () {}
};


/**
 * @brief Public API to set up a game or scene
 * 
 */
class serval::GameSetup {
public:
    virtual ~GameSetup () {}

    virtual entt::registry& registry () = 0;
    // virtual serval::Handle loadResource (serval::ID resource_type) = 0;

};


/**
 * @brief Public runtime API
 * 
 */
class serval::Runtime {
public:
    virtual ~Runtime () {}


    /* ************************************* */
    /* **** Task API                    **** */
    /* ************************************* */

    /**
     * @brief Execute an asynchronous task
     * 
     * @param task The delegate to execute
     */
    virtual void asyncTask (serval::AsyncTask task) = 0;

    /**
     * @brief Execute an asynchronous task
     * Allows binding memer functions directly: `runtime->asyncTask<&Foo::bar>(this);`
     * 
     * @tparam Func The function to bind
     * @tparam Instance 
     * @param instance The instance of the object to which Func belongs
     */
    template <auto Func, typename Instance>
    void asyncTask (Instance* instance) {
        serval::AsyncTask task;
        task.connect<Func>(instance);
        asyncTask(task);
    }


    /* ************************************* */
    /* **** Entity API                  **** */
    /* ************************************* */

    /**
     * @brief Asynchronously load an entity from a template and publish a notification to a stream when done
     * 
     * @param template_name The name of the template load the entity from
     * @param stream The stream on which to notify when the entity is loaded (0 to disable notification)
     */
    virtual void loadEntity (serval::Id template_name, serval::Id stream=0) = 0;

    /**
     * @brief Asynchronously load an entity from a template and publish a notification to a stream when done
     * 
     * @param template_name The name of the template load the entity from
     * @param entity_name The name to assign to the new entity
     * @param stream The stream on which to notify when the entity is loaded (0 to disable notification)
     * @return serval::ID A ID of the name
     */
    virtual serval::Id loadNamedEntity (serval::Id template_name, const char* entity_name, serval::Id stream=0) = 0;

    /**
     * @brief Asynchronously load an actor from an template
     * 
     * @param actor_template The name of the template load the actor from
     * @param actor_name The name to assign to the actor
     * @return serval::ID A ID of the name
     */
    virtual serval::Id loadActor (serval::Id actor_template, const char* actor_name) = 0;

    /**
     * @brief Queue entity to be createed, asynchronously calling ctor(Runtime, entt::handle) on the created entity
     * 
     * @param ctor Delegate to call when the entity is created and can be safely set up
     */
    virtual void createEntity (serval::EntityConstructor ctor) = 0;

    /**
     * @brief Queue a named entity to be createed, asynchronously calling ctor(Runtime, entt::handle) on the created entity
     * 
     * @param name The name to give the entity
     * @param ctor Delegate to call when the entity is created and can be safely set up
     */
    virtual void createEntity (serval::Id name, serval::EntityConstructor ctor) = 0;

    /**
     * @brief Queue entity to be destroyed at the end of the frame
     * 
     * @param entity The entity to destroy
     */
    virtual void destroyEntity (entt::entity entity) = 0;

    /**
     * @brief Lookup a named entity by name
     * 
     * @param name The name of the entity
     * @return entt::enitty The entity (or entt::null if not found)
     */
    virtual entt::entity lookup (serval::Id name) const = 0;

    /**
     * @brief Queue a tag to be added to an entity
     * 
     * @param entity 
     * @param tag 
     */
    virtual void tagEntity (entt::entity entity, serval::Id tag) = 0;


    /* ************************************* */
    /* **** Game State API              **** */
    /* ************************************* */

    /**
     * @brief Push a new state onto the state stack
     * 
     * @param state_id ID of the state to push
     */
    virtual void pushState (serval::Id state_id) = 0;

    /**
     * @brief Pop the top state from the stack
     * 
     */
    virtual void popState () = 0;

    /**
     * @brief Set the top state on the stack (equivalent to popping once and then pushing the new state)
     * 
     * @param state_id 
     */
    virtual void setState (serval::Id state_id) = 0;

    /**
     * @brief Return the current top state on the stack
     * 
     * @return serval::ID 
     */
    virtual serval::Id currentState () const = 0;

    /**
     * @brief Check if a state is on the state stack
     * 
     * @param state_id 
     * @return true state_id is on the state stack
     * @return false state_id is not on the state stack
     */
    virtual bool inState (serval::Id state_id) const = 0;


    /* ************************************* */
    /* **** Messaging API               **** */
    /* ************************************* */

    // TODO: Commands will probably be removed in favour of direct function call API's
    // These API's may use internal command buffers for thread safety, but these
    // will not use a global API like here (rather an "allocate buffer object" call)

    /**
     * @brief Create a command on the command queue
     * 
     * @tparam Command 
     * @param target The id of the target to send the command to
     * @return Command& 
     */
    template <typename Command>
    [[nodiscard]] Command& command (serval::Id target) {
        static_assert(std::is_trivial_v<Command>, "Commands must be trivial types");
        std::byte* ptr = make_command(target, Command::CommandTypeID, sizeof(Command));
        if EXPECT_TAKEN(ptr) {
            return *reinterpret_cast<Command*>(ptr);
        }
        warn("Command does not exist or does not match size");
        throw std::runtime_error("Command does not exist or does not match size");
    }

    /**
     * @brief Create a tag-only command on the command queue
     * 
     * @param target The id of the target to send the command to
     * @param command_id The id of the command to send
     */
    template <typename T=void>
    void command (serval::Id target, serval::Id command_id) {
        make_command(target, command_id, 0);
    }

    /**
     * @brief Create a simple tag/argument command on the command queue
     * 
     * @param target The id of the target to send the command to
     * @param command_id The id of the command to send
     * @param argument The hashed_string id to send as the argument
     */
    template <typename T=void>
    void command (serval::Id target, serval::Id command_id, serval::Id argument) {
        send_simple_command(target, command_id, argument);
    }

    /**
     * @brief Send a message to an actor, to be executed immediately on the calling thread
     * If the target_actor entity is not an actor, the message will sitll be sent, but will be ignored for processing 
     * 
     * @tparam Params 
     * @param target_actor The actor to send the message to
     * @param message The hashed string message type
     * @param params Zero to Five arguments of types supported by variant::Type
     */
    template <typename... Params>
    void message (entt::entity target_actor, serval::Id message, Params... params) {
        static_assert(sizeof...(params) <= 5, "Maximum of 5 parameters supported by messages");
        if constexpr (sizeof...(params) == 0) {
            send_message(target_actor, message, 0);
        } else {
            constexpr std::size_t buffer_size = (variant::size(variant::type_of<Params>()) + ...);
            serval::ParametersBuffer parameters;
            get_parameters_buffer(buffer_size, &parameters);
            msghelpers::populate_parameters_buffer(parameters.buffer, std::forward<Params>(params)...);
            constexpr std::uint32_t metadata = msghelpers::generate_metadata<sizeof...(Params), 0, Params...>();
            send_message(target_actor, message, parameters.metadata | metadata);
        }
    }

    template <auto Field, typename... Params>
    void post (entt::entity target_actor, Params&&... params) {
        using Component = serval::class_of_t<decltype(Field)>;
        const auto& registry = this->registry();
        if (registry.all_of<Component>(entity)) {
            const auto& component = registry.get<Component>(entity);
            const auto message = component.*Field;
            if (message != serval::Id::INVALID) {
                message(target_actor, message, std::forward<Params>(params)...);
            }
        }
    }
    // api.post<&CollisionSensor::onCollide>(entity, 1, 2, 3, 4, 5);

    /**
     * @brief Get a stream for reading
     * Will alrways read the previous frames notifications
     * 
     * @param stream_name The name of the stream to read
     * @return const StreamReader& 
     */
    virtual const serval::StreamReader& stream (serval::Id stream_name) = 0;


    /* ************************************* */
    /* **** Timekeeping API             **** */
    /* ************************************* */

    /**
     * @brief Get the current timeline
     * 
     * @return const serval::Timeline& 
     */
    virtual const serval::Timeline& timeline () = 0;


    /* ************************************* */
    /* **** Resource Management API     **** */
    /* ************************************* */

    /**
     * @brief Retrieve a resource from a handle
     * 
     * @tparam Resource The type of resource to retrieve
     * @param handle The handle to the resource
     * @return Resource* A pointer to the resource
     */
    template <typename Resource>
    Resource* resolve (serval::Handle handle) {
        static_assert(serval::is_resource_v<Resource>, "Must be a resource type");
        return reinterpret_cast<Resource*>(get_resource_ptr(handle, Resource::ResourceTypeID));
    }

private:
    virtual std::byte* make_command (serval::Id target_id, serval::Id command_id, std::size_t size) = 0;
    virtual void send_simple_command (serval::Id target_id, serval::Id command_id, serval::Id parameter) = 0;
    virtual void send_message (entt::entity target, serval::Id type, std::uint32_t metadata) const = 0;
    virtual void get_parameters_buffer (std::size_t size, serval::ParametersBuffer* info) const = 0;
    virtual void* get_resource_ptr (serval::Handle handle, serval::Id resource_id) const = 0;
    virtual entt::registry& registry () = 0;
};

#endif