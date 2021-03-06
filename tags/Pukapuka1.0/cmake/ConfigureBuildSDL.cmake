if(UNIX)
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE PROCESSOR
        ERROR_QUIET
    )
    set(ARCH ${PROCESSOR})
endif(UNIX)

set(ENABLE_DEBUG ON CACHE BOOLEAN "Enable debug support" FORCE)
set(ENABLE_TESTS ON CACHE BOOLEAN "Enable tests" FORCE)

set(ENABLE_3D_RENDERING ON CACHE BOOLEAN "Enable 3d rendering support" FORCE)
set(ENABLE_DATABASE ON CACHE BOOLEAN "Enable HTML5 client-side database storage support" FORCE)
set(ENABLE_DATAGRID ON CACHE BOOLEAN "Enable datagrid support" FORCE)
set(ENABLE_DOM_STORAGE ON CACHE BOOLEAN "Enable HTML5 client-side session and persistent storage support" FORCE)
set(ENABLE_EVENTSOURCE ON CACHE BOOLEAN "Enable HTML5 server-sent events support" FORCE)
set(ENABLE_ICONDATABASE ON CACHE BOOLEAN "Enable icon database support" FORCE)
set(ENABLE_OFFLINE_WEB_APPLICATIONS ON CACHE BOOLEAN "Enable HTML5 offline web applications support" FORCE)
set(ENABLE_OFFLINE_DYNAMIC_ENTRIES ON CACHE BOOLEAN "Enable HTML5 offline dynamic entries support" FORCE)

set(ENABLE_GEOLOCATION ON CACHE BOOLEAN "Enable geoposition support" FORCE)
set(ENABLE_INSPECTOR ON CACHE BOOLEAN "Enable web inspector support" FORCE)

if(ARCH MATCHES "i[3-6]86")
    set(ENABLE_JIT_JSC ON CACHE BOOLEAN "Enable JavascriptCore JIT compilation (for x86 only)" FORCE)
endif(ARCH MATCHES "i[3-6]86")

if(ARCH MATCHES "[ix][3-6]*86*")
    set(ENABLE_YARR ON CACHE BOOLEAN "Enable RegExp compilation (for x86 and x86_64 only)" FORCE)
endif(ARCH MATCHES "[ix][3-6]*86*")

set(ENABLE_MATHML ON CACHE BOOLEAN "Enable MathML support" FORCE)
set(ENABLE_MULTIPLE_THREADS ON CACHE BOOLEAN "Enable multiple threads" FORCE)
set(ENABLE_WORKERS ON CACHE BOOLEAN "Enable workers support" FORCE)
set(ENABLE_SHARED_WORKERS ON CACHE BOOLEAN "Enable shared workers support" FORCE)
set(ENABLE_NOTIFICATIONS ON CACHE BOOLEAN "Enable notification support" FORCE)
set(ENABLE_NPAPI ON CACHE BOOLEAN "Enable Netscape Plugin API support" FORCE)

set(ENABLE_SVG ON CACHE BOOLEAN "Enable SVG support" FORCE)
set(ENABLE_SVG_ANIMATION ON CACHE BOOLEAN "Enable support for SVG animation (EXPERIMENTAL)" FORCE)
set(ENABLE_SVG_AS_IMAGE ON CACHE BOOLEAN "Enable SVG as Image support (EXPERIMENTAL)" FORCE)
set(ENABLE_FILTERS ON CACHE BOOLEAN "Enable support for filters (EXPERIMENTAL)" FORCE)
set(ENABLE_SVG_FONTS ON CACHE BOOLEAN "Enable support for SVG fonts (EXPERIMENTAL)" FORCE)
set(ENABLE_SVG_FOREIGN_OBJECT ON CACHE BOOLEAN "Enable support for SVG foreign objects (EXPERIMENTAL)" FORCE)
set(ENABLE_SVG_USE_ELEMENT ON CACHE BOOLEAN "Enable support for SVG use element (EXPERIMENTAL)" FORCE)

set(ENABLE_VIDEO ON CACHE BOOLEAN "Enable HTML5 video support" FORCE)
set(ENABLE_WEB_SOCKETS ON CACHE BOOLEAN "Enable Web Sockets support" FORCE)
set(ENABLE_WML ON CACHE BOOLEAN "Enable WML support" FORCE)
set(ENABLE_XPATH ON CACHE BOOLEAN "Enable XPath support"FORCE)
set(ENABLE_XSLT ON CACHE BOOLEAN "Enable XSLT support"FORCE)

set(USE_FILESYSTEM_ACCESS "POSIX" CACHE STRING "Choose the filesystem access method, options are: GLIB POSIX" FORCE)
set(USE_FONTS "FREETYPE" CACHE STRING "Choose the font engine, options are: GTK FREETYPE EMBEDDED" FORCE)
set(USE_GRAPHICS "SDL" CACHE STRING "Choose the graphic backend, options are: GTK SDL" FORCE)
set(USE_I18N "ICU" CACHE STRING "Choose the internationalization library, options are: ICU GENERIC GLIB" FORCE)
set(USE_THREADS "PTHREADS" CACHE STRING "Choose the thread backend, options are: GTHREADS PTHREADS NONE" FORCE)
set(USE_TIMER "LINUX" CACHE STRING "Choose the timer backend, options are: GLIB LINUX" FORCE)
