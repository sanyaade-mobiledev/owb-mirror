list(APPEND WEBCORE_SRC
    page/animation/AnimationBase.cpp
    page/animation/AnimationController.cpp
    page/animation/CompositeAnimation.cpp
    page/animation/ImplicitAnimation.cpp
    page/animation/KeyframeAnimation.cpp
    page/BarInfo.cpp
    page/Chrome.cpp
    page/ContextMenuController.cpp
    page/DOMSelection.cpp
    page/DOMTimer.cpp
    page/DOMWindow.cpp
    page/DragController.cpp
    page/EventHandler.cpp
    page/FocusController.cpp
    page/Frame.cpp
    page/FrameTree.cpp
    page/FrameView.cpp
    page/History.cpp
    page/Location.cpp
    page/MouseEventWithHitTestResults.cpp
    page/Navigator.cpp
    page/NavigatorBase.cpp
    page/OriginAccessEntry.cpp
    page/Page.cpp
    page/PageGroup.cpp
    page/PageGroupLoadDeferrer.cpp
    page/PluginHalter.cpp
    page/PrintContext.cpp
    page/Screen.cpp
    page/SecurityOrigin.cpp
    page/Settings.cpp
    page/UserContentURLPattern.cpp
    page/WindowFeatures.cpp
    page/XSSAuditor.cpp
)
list(APPEND IDL_SRC
    page/BarInfo.idl
    page/Coordinates.idl
    page/DOMSelection.idl
    page/DOMWindow.idl
    page/History.idl
    page/Location.idl
    page/Navigator.idl
    page/PositionError.idl
    page/Screen.idl
    page/WebKitPoint.idl
)

if(ENABLE_WORKERS)
    list(APPEND WEBCORE_SRC
        page/WorkerNavigator.cpp
    )
    list(APPEND IDL_SRC
        page/WorkerNavigator.idl
    )
endif(ENABLE_WORKERS)

if(ENABLE_INSPECTOR)
    list(APPEND WEBCORE_SRC
        page/Console.cpp
    )
    list(APPEND IDL_SRC
        page/Console.idl
    )    
endif(ENABLE_INSPECTOR)

if(ENABLE_GEOLOCATION)
    list(APPEND WEBCORE_SRC
        page/Geolocation.cpp
    )
    list(APPEND IDL_SRC
        page/Geoposition.idl
        page/Geolocation.idl
    )
endif(ENABLE_GEOLOCATION)

if(ENABLE_EVENTSOURCE)
    list(APPEND WEBCORE_SRC
        page/EventSource.cpp
    )
    list(APPEND IDL_SRC
        page/EventSource.idl
    )
endif(ENABLE_EVENTSOURCE)
