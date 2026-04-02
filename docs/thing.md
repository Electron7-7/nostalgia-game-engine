|[../../return to wiki home](https://github.com/Electron7-7/nostalgia-game-engine/wiki/Home) | [../return to things index](things_index.md)|
|---|---|

### Index
- [Description](#description)<br>
- [Super](#super)
- [Macros](#macros)
- [Thing_t](#thing_t)
- [Virtual Functions](#virtual-functions)
- [Non-Virtual Functions](#non-virtual-functions)
- [Function Descriptions](#function-descriptions)

---

# Thing

**Inherited By:** [Resource](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/resource.md), [Thinker](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/thinker.md)

The base class for everything else that lives in a [Theatre]().

## Description

Since Nostalgia is partially based on Godot, **Thing** can be compared to Godot's Object class, albeit less advanced. Most, if not all, of the most important virtual functions that your classes will override come from **Thing**. There is an important note to make on overriding virtual functions: when a virtual function is overridden, the super class' implementation should be called at some point to avoid blocking important functionality. Speaking of which...

## Super
`Super` is a keyword which can be defined by using the [`SET_SUPER`](../Nostalgia/things/thing.hpp#L11) macro and should always be set to the current class' base class (e.g: `Texture::Super` is [Resource](resource.md), `Thinker::Super` is **Thing**, etc). when overriding virtual functions; the super class' implementation should be called at either the beginning or end of the new implementation, to avoid blocking potentially important functionality. For example, [Thinker::SetVariables]() calls `Super::SetVariables` immediately before doing anything else. You can "skip over" the immediate super class by directly calling an inherited class (e.g: if a class inherits from [Resource](resource.md), calling `Thing::Ready` would skip `Resource::Ready`), but a more elegant solution would be to use `Super::Super::` (theoretically, you could chain `Super::` until you get back to **Thing**).

## Macros
Most of the macros defined in [thing.hpp](../Nostalgia/things/thing.hpp) are simply shorthand for overriding virtual functions; the others, however, are highly important and should always be used when creating your own **Thing**-derived class.

| Macro | Description |
|  ---  |     ---     |
| [SET_SUPER(SUPER)](../Nostalgia/things/thing.hpp#L11) | Defines the [`Super`](#super) keyword and inherits all of its constructors. The `SUPER` argument should be the immediate base class, so for [Thinker](thinker.md) it would be **Thing**, for [Texture](texture.md) it would be [Resource](resource.md), etc.
| [SET_TYPEID(TYPE_PID)](../Nostalgia/things/thing.hpp#L14) | Overwrites the [`Type`](#virtual-constexpr-fpid-type-const) function, having it return `TYPE_PID`. This is one of the most important functions to overwrite, as it will always return `TYPE_PID`, even if called on a pointer that's been casted to another type (most commonly, `std::shared_ptr<Thing>`).
| [DEFINE_TYPEID(CLASS, VAR_NAME)](../Nostalgia/things/thing.hpp#L18) | Defines a `static const PID` variable which can be used by `SET_TYPEPID`. `CLASS` should be the name of the class and `VAR_NAME` will be the name of the type id variable. Since all **Thing** type ids are hashes of their string names, I personally prefer to define them all as constants in a global namespace, to avoid calling that `PID` constructor every time. This macro exists for people who don't care about doing allat.

## Thing_t
A [concept](https://en.cppreference.com/w/cpp/language/constraints.html) used in function templates to constrain a typename. It requires the type be derived from **Thing** (either directly or indirectly).

## Virtual Functions
| Return Type | Signature |
|    ----     |    ---    |
| constexpr const [PID]()& | [Type](#virtual-constexpr-const-pid-type-const)() const |
| void | [Init](#virtual-void-init)() |
| void | [Ready](#virtual-void-ready)() |
| void | [Shutdown](#virtual-void-shutdown)() |
| void | [Tick](#virtual-void-tick)() |
| void | [Update](#virtual-void-update)() |
| void | [Input](#virtual-void-inputinputevent)([InputEvent]()\*) |
| void | [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata)(const[TheatreFile::ThingData]()&) |
| std::shared_ptr<[TheatreFile::ThingData]()> | [GetVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |

## Non-Virtual Functions
| Return Type | Signature |
|    ----     |    ---    |
| bool | [DerivedFrom](#bool-derivedfrom-const-pid-const)(const [PID]()&) const |
| bool | [IsThinker](#bool-isthinker-const)() const |
| bool | [IsResource](#bool-isresource-const)() const |
| [TheatreFile::ThingData]() | [GetStartingVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |
| [ID]() | [uid](#id-uid-const)() const |
| const std::string& | [name](#const-stdstring-name-const)() const |
| const char* const | [c_name](#const-char-const-c_name-const)() const |

## Variables
| Scope | Type | Name | Default Value |
|  ---  | ---- | ---- |      ---      |
| public | bool | mIsHoveredInDebugger | `false` |
| protected | std::string | mName | `""` |
| protected | std::shared_ptr<[TheatreFile::ThingData]()> | m_pStartingData | `nullptr` |
| private | [ID]() | mUID | `ID::Invalid` |


## Variable Descriptions
### bool mIsHoveredInDebugger{`false`}
When `true`, a highlight is applied when rendering this **Thing** if it derives from either [Visual3D]() or [Visual2D]().

**Note:** This value is only toggled by the editor's [Theatre]() debug menu and will be removed/hidden in a future update.

---
### std::string mName{`""`}
The name for this **Thing**. This variable is set by the [Theatre]() or [ResourceDatabase]() when creating/registering a **Thing**. If a **Thing** hasn't had its name set and [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata) is called, **Thing** will set its name to [TheatreFile::ThingData::name]().

---
### std::shared_ptr<[TheatreFile::ThingData]()> m_pStartingData{`nullptr`}
When [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata) is first called, a new `std::shared_ptr<[TheatreFile::ThingData]()>` is created with the function argument and assigned to **m_pStartingData**.

---
### [ID]() mUID{[`ID::Invalid`](../Nostalgia/core/id.hpp#L45)}
The uid for this **Thing**. This variable is set by the [Theatre]() or [ResourceDatabase]() when creating/registering a **Thing**. If a **Thing** hasn't had its uid set and [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata) is called, **Thing** will set its name to [TheatreFile::ThingData::_uid]().


## Function Descriptions
### virtual constexpr const [PID]()& Type() const
Should always be overridden to return the type id of this class. You can do this easily with either the [SET_TYPEID](../Nostalgia/things/thing.hpp#L14) or [DEFINE_TYPEID](../Nostalgia/things/thing.hpp#L18) macros. See [Thinker](../Nostalgia/things/thinkers/thinker.hpp) for an example of overriding the function with a type id variable not from the same class. See [EditorPlayer3D](../Editor/things/player.hpp) for an example of creating a type id variable in the class and overriding the function to use that variable.

---
### virtual void Init()
When a **Thing** is created by the [Theatre]() or created by/registered with the [ResourceDatabase](), **Init** gets called immediately after; this means that **Init** will _always_ be called before [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata). However, it's still recommended to treat **Init** more like a pre-init function and leave the heavier setup to [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata).

---
### virtual void Ready()
Called by [Theatre::CreateThing]() after the current **Thing** has been instantiated and [SetVariables()](#virtual-void-setvariablesconst-theatrefilethingdata) has been called. When a new [Theatre]() is first loaded, **Ready** is called on every starting **Thing** after they've all been fully initialized and set up. [Resource]() purposefully removes the ability to override this function and all setup should be done in either [Init](#virtual-void-init) or [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata).

---
### virtual void Shutdown()
Called by the **Thing** destructor. Override this to perform any cleanup for your class but **always** remember to call the [Super](#super) class' implementation at the very end (this is most important for [Resource]()-derived classes).

---
### virtual void Tick()
Called every game tick via [Theatre::Tick]() for more information on the Tick system, see [Manager::Tick]().

[Resource]() purposefully removes the ability to override this function.

---
### virtual void Update()
Called every frame via [Theatre::Update]() for more information on the Update system, see [Manager::Update]().

[Resource]() purposefully removes the ability to override this function.

---
### virtual void Input([InputEvent]()\*)
Called on every input event via [Theatre::Input]() for more information on the Input system, see [InputManager]().

[Resource]() purposefully removes the ability to override this function.

---
### virtual void SetVariables(const [TheatreFile::ThingData]()&)
Called by [Theatre::CreateThing]() to initialize the current **Thing** with some [ThingData](). When overridden, it's very important that the [Super](#super) class' implementation is called at some point. Doing this properly will result in all the inherited classes setting their own variables instead of requiring the new derived class to do so for every single inherited variable. If you want to override a specific variable assignment (like in the case of most of the [Resource]()-derived classes), you can still do so after calling the [Super](#super) class' implementation.

---
### virtual std::shared_ptr<[TheatreFile::ThingData]()> GetVariables() const
This is used to query the state of the current **Thing** and/or to save its current state. When overridden, it's **_very_** important that the [Super](#super) class' implementation is called _first_ and its output is used as the output variable. Doing this properly will result in all the relevant variables for a class being saved to a single [ThingData]() variable, which is then returned. See [Thinker::GetVariables](../Nostalgia/things/thinkers/thinker.cpp#L17) for an example.

---
### bool DerivedFrom(const [PID]()&) const
Calls [ThingFactory::IsDerivedFrom]() with the result of calling [Type]() and the passed type id.

---
### bool IsThinker() const
Calls [ThingFactory::IsThinker]() with the result of calling [Type]().

---
### bool IsResource() const
Calls [ThingFactory::IsResource]() with the result of calling [Type]().

---
### [TheatreFile::ThingData]() GetStartingVariables() const
Returns the underlying data at [m_pStartingData](#stdshared_ptrtheatrefilethingdata-m_pstartingdata) by dereferencing.

---
### [ID]() uid() const
Returns [mUID](#id-muididinvalid).

---
### const std::string& name() const
Returns [mName](#stdstring-mname).

---
### const char* const c_name() const
Returns [mName](#stdstring-mname) as a `const char*` via `std::string::data`.
