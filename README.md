# Patterns introduced in Design Patterns

[ET++](http://www.softwarepreservation.org/projects/c_plus_plus/library/et_plus_plus) is
referenced in *Abstract Factory* (p97), Builder, Factory Method,
Prototype, Adapter, *Bridge* (p151), Composite, Decorator, Facade,
Flyweight, Chain of Responsibility, Command, Iterator, Mediator,
Observer, and Strategy in Design Patterns (ISBN-13: 9787111095071).

## Abstract Factory

Factories:

* WindowSystem (abstract)

  * NeWSWindowSystem

  * ServerWindowSystem

  * SunWindowSystem

  * XWindowSystem

Products:

* WindowPort (abstract)

  * NeWSWindowPort
  
  * ServerPort
  
  * SunWindowPort
  
  * XWindowPort

* FontManager (abstract)

  * NeWSFontManager

  * ServerFontManager

  * SunFontManager

  * XFontManager

* DevBitmap (abstract)

  * SunBitmap
  
  * XBitmap

* ClipBoard (abstract)

  * SunClipBoard
  
  * XClipBoard

**How does et++ decide which concrete factory to use?**

In InitWindowSystem, it uses macros to choose the concrete factory.

```c++
WindowSystem    *gWindowSystem= 0;

void InitWindowSystem()
{
    // ...

#ifdef WS_SUNSERVER
    if (gWindowSystem == 0)
	gWindowSystem= NewServerWindowSystem(); // try to connect to SunWindow server
#endif WS_SUNSERVER

#ifdef WS_X
    if (gWindowSystem == 0)
	gWindowSystem= NewXWindowSystem();      // try to connect to X server
#endif WS_X

#ifdef WS_NEWS
    if (gWindowSystem == 0)
	gWindowSystem= NewNeWSWindowSystem();   // try to connect to NeWS server
#endif WS_NEWS
    
#ifdef WS_SUNWINDOW
    if (gWindowSystem == 0)
	gWindowSystem= NewSunWindowSystem();    // try SunWindow System
#endif WS_SUNWINDOW
    
    // ...
}
```

## Bridge

* BlankWin (Abstraction)

  * Window (Refined Abstraction)
  
  * Icon (Refined Abstraction)

* WindowPort (Implementor)

  * NeWSWindowPort (Concrete Implementor)
  
  * ServerPort (Concrete Implementor)
  
  * SunWindowPort (Concrete Implementor)
  
  * XWindowPort (Concrete Implementor)

This pattern is combined with Abstract Factory to make a concrete WindowPort object.

```c++
class WindowPort *BlankWin::MakePort()
{
    if (portDesc == 0) {
        portDesc= gWindowSystem->MakeWindow((InpHandlerFun) &BlankWin::input, this,
                        TestFlag(eBWinOverlay), TestFlag(eBWinBlock), FALSE);
        portDesc->cursor= cursor;
    }
    return portDesc;
}
```
