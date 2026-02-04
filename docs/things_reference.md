# Things Reference
[return to the wiki](/wiki/Home)

# Thing

**Inherited By:** [Resource](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/resource.md), [Thinker](https://github.com/Electron7-7/nostalgia-game-engine/docs/things/thinker.md)

The base class for everything else that lives in a [Theatre]().

## Description

Since Nostalgia is partially based on Godot, **Thing** can be compared to Godot's Object class, albeit less advanced. Most, if not all, of the most important virtual functions that your classes will override come from **Thing**. There is an important note to make on overriding virtual functions; when a virtual function is overridden, the original function should be called on the super class at either the beginning or end of the new implementation. For example, if [Thinker]() overrides [Ready()](), it should call `Thing::Ready()` so that it doesn't override potentially important functionality (or if a class that derives from [Thinker]() overrides [Ready()](), it should call `Thinker::Ready()`). This is more important to do for certain functions, especially [SetVariables()]() and [GetVariables()]().

## Functions

| Return Type | Function Signature |
| --- | --- |
| void | virtual [Free](#virtual-void-free)() |
| void | virtual [Ready](#virtual-void-ready)() |
| void | virtual [Shutdown](#virtual-void-shutdown)() |
| void | virtual [Tick](#virtual-void-tick)() |
| void | virtual [Update](#virtual-void-update)() |
| void | virtual [Input](#virtual-void-inputinputevent)([InputEvent]()\*) |
| void | virtual [SetVariables](#virtual-void-setvariablesconst-theatrefilethingdata)(const[TheatreFile::ThingData]()&) |
| std::shared_ptr<[TheatreFile::ThingData]()> | virtual [GetVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |
| [TheatreFile::ThingData]() | [GetStartingVariables](#virtual-stdshared_ptrtheatrefilethingdata-getvariables-const)() const |
| [ID]() | [uid](#id-uid-const)() const |
| const std::string& | [name](#const-stdstring-name-const)() const |
| void | [set_name](#void-set_namesarg)(Sarg) |
| const char* const | [c_name](#const-char-const-c_name-const)() const |
| const [PID]()& | [type](#const-pid-type-const)() const |

## Function Descriptions
### virtual void Free()
When called, will shutdown and destroy the current **Thing**.

----
### virtual void Ready()
----
### virtual void Shutdown()
----
### virtual void Tick()
----
### virtual void Update()
----
### virtual void Input([InputEvent]()\*)
----
### virtual void SetVariables(const [TheatreFile::ThingData]()&)
----
### virtual std::shared_ptr<[TheatreFile::ThingData]()> GetVariables() const
----
### [TheatreFile::ThingData]() GetStartingVariables() const
----
### [ID]() uid() const
----
### const std::string& name() const
----
### void set_name(Sarg)
----
### const char* const c_name() const
----
### const [PID]()& type() const
