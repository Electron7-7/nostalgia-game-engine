[return to the wiki](../../../wiki/Home)

_This documentation is heavily inspired by/plagiarizes Godot's [GDScript documentation](https://docs.godotengine.org/en/stable/classes) (specifically, the class reference)._

# Index
- [Thing](thing.md)
    - [Resource](resource.md)
        - [Font](font.md)
        - [Mesh](mesh.md)
        - [Texture](texture.md)
        - [Material](material.md)
    - [Thinker](thinker.md)
        - [Viewport](viewport.md)
        - [Actor3D](actor_3d.md)
            - [NostalgiaPlayer3D](nostalgia_player_3d.md)
            - [Camera3D](camera_3d.md)
            - [Collider3D](collider_3d.md)
            - [Visual3D](visual_3d.md)
                - [MeshInstance3D](mesh_instance_3d.md)
                - [Light3D](light_3d.md)
                    - [PointLight3D](point_light_3d.md)
                    - [SpotLight3D](spot_light_3d.md)
                    - [DirectionalLight3D](directional_light_3d.md)
        - [Actor2D](actor_2d.md)
            - [Camera2D](camera_2d.md)
            - [Visual2D](visual_2d.md)
                - [Sprite2D](sprite_2d.md)
                - [Text2D](text_2d.md)
                - [MeshInstance2D](mesh_instance_2d.md)

----

# Thing

**Inherited By:** [Resource](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/resource.md), [Thinker](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/thinker.md)

The base class for everything else that lives in a [Theatre]().

## Description

Since Nostalgia is partially based on Godot, **Thing** can be compared to Godot's Object class, albeit less advanced. Most, if not all, of the most important virtual functions that your classes will override come from **Thing**. There is an important note to make on overriding virtual functions; when a virtual function is overridden, the original function should be called on the super class at either the beginning or end of the new implementation. For example, if [Thinker]() overrides [Ready()](), it should call `Thing::Ready()` so that it doesn't override potentially important functionality (or if a class that derives from [Thinker]() overrides [Ready()](), it should call `Thinker::Ready()`). This is more important to do for certain functions, especially [SetVariables()]() and [GetVariables()]().

## Virtual Functions
| Scope | Return Type | Function Signature |
| --- | --- | --- |
| public | void | [Ready](#virtual-void-ready)() |
| public | void | [Shutdown](#virtual-void-shutdown)() |
| public | void | [Tick](#virtual-void-tick)() |
| public | void | [Update](#virtual-void-update)() |
| public | void | [Input](#virtual-void-inputinputevent)([InputEvent]()\*) |
| public | void | [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata)(const[TheatreFile::ThingData]()&) |
| public | std::shared_ptr<[TheatreFile::ThingData]()> | [GetVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |

## Non-Virtual Functions
| Scope | Return Type | Function Signature |
| --- | --- | --- |
| public | void | [Free](#void-free)() |
| public | [TheatreFile::ThingData]() | [GetStartingVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |
| public | [ID]() | [uid](#id-uid-const)() const |
| public | const std::string& | [name](#const-stdstring-name-const)() const |
| public | void | [set_name](#void-set_namesarg)(Sarg) |
| public | const char* const | [c_name](#const-char-const-c_name-const)() const |
| public | const [PID]()& | [type](#const-pid-type-const)() const |
| protected | [Theatre]()\* | [my_theatre](#theatre-my_theatre-const)() const |

## Function Descriptions
### virtual void Ready()
Called by [Theatre::CreateThing()]() after the current **Thing** has been instantiated and [SetVariables()](#virtual-void-setvariablesconst-theatrefilethingdata) has been called; this ensures that important variables such as [m_pRootTheatre](), [mName](), [mUID](), etc all have values and the current **Thing** is inside a [Theatre]() when this function is called.

---
### virtual void Shutdown()
Called by [Theatre::DestroyThing()]() right before destructing the current **Thing**.

---
### virtual void Tick()
Called every game tick via [Theatre::Tick()]() (see [Manager::Tick()]() for more information).

---
### virtual void Update()
Called every frame via [Theatre::Update()]() (see [Manager::Update()]() for more information).

---
### virtual void Input([InputEvent]()\*)
Called on every input event via [Theatre::Input()]() (see [InputManager]() for more information on the input system).

---
### virtual void SetVariables(const [TheatreFile::ThingData]()&)
Called by [Theatre::CreateThing()]() to initialize the current **Thing** with its [ThingData](). This can be thought of like an "init" function. When overridden, it's very important that the base class' implementation is called at some point in the function (e.g: [Actor3D]() calls `Thinker::SetVariables(data)` at the very beginning of its implementation). Doing this properly will result in all the base classes setting their own variables instead of requiring the new derived class to do so for every single inherited variable. If you want to override a specific variable assignment (like in the case of most of the [Resource]()-derived classes), you can still do so after calling the base class' implementation. 

---
### virtual std::shared_ptr<[TheatreFile::ThingData]()> GetVariables() const
This is used to query the state of the current **Thing** and/or to save its current state. When overridden, it's **_very_** important that the base class' implementation is called _first_ and that its output is used as the output variable. For example, [Actor3D]() captures the output from `Thinker::GetVariables()` at the very beginning of its implementation; afterwards, [Actor3D]()'s state is saved to that output variable using [ThingData::set_variable()](), and the output variable is returned. Doing this properly will result in all the relevant variables for a class being saved to a single [ThingData]() variable, which is then returned.

---
### void Free()
Calls [Theatre::DestroyThing()]() on itself. Will most likely be expanded in the future.

---
### [TheatreFile::ThingData]() GetStartingVariables() const
Returns the data at [m_pStartingData]() via dereferencing.

---
### [ID]() uid() const
Returns [mUID]().

---
### const std::string& name() const
Returns [mName]().

---
### void set_name(const std::string& inName)
Sets [mName]() to `inName`.

---
### const char* const c_name() const
Returns [mName]() as a `const char*` via `std::string::data`.

---
### const [PID]()& type() const
Returns [mType]().

----
### [Theatre]()\* my_theatre() const
Returns [m_pRootTheatre]() if it has a valid value; if it's `nullptr`, an empty `static Theatre*` variable is returned instead.

---
