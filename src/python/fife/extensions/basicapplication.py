# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
The basic application and main loop.

See the L{ApplicationBase} documentation.
"""

from fife import fife
from fife.extensions import fifelog
from fife.extensions.fife_settings import Setting


class ExitEventListener(fife.IKeyListener):
    """
    Default, rudimentary event listener.

    Will cause the application to quit on pressing ESC.
    """

    def __init__(self, app):
        self.app = app
        self.engine = app.engine
        eventmanager = self.engine.getEventManager()
        # eventmanager.setNonConsumableKeys([fife.Key.ESCAPE])
        fife.IKeyListener.__init__(self)
        eventmanager.addKeyListener(self)
        self.quitRequested = False

    def keyPressed(self, evt):
        """Handle a key press event.

        Parameters
        ----------
        evt : fife.KeyEvent
            The key event that was received.
        """
        keyval = evt.getKey().getValue()
        if keyval == fife.Key.ESCAPE:
            self.app.quit()

    def keyReleased(self, evt):
        """Handle a key release event.

        Parameters
        ----------
        evt : fife.KeyEvent
            The key event that was released.
        """
        pass


class ApplicationBase:
    """
    ApplicationBase is an extendable class that provides a basic environment for a FIFE-based client.

    This kind of application base does not offer GUI support.

    The unextended application reads in and initializes engine settings, sets up a simple event
    listener, and pumps the engine while listening for a quit message. Specialized applications can
    modify settings.py to change initial engine settings. They can provide their own event listener
    by overriding L{createListener}. And they can override the L{_pump} method
    to define runtime behavior of the application.
    """

    def __init__(self, setting=None):
        if setting:
            self._setting = setting
        else:
            self._setting = Setting(app_name="", settings_file="./settings.xml")

        self.engine = fife.Engine()

        self.initLogging()
        self.loadSettings()

        self.engine.init()

        self._lastWindowW = 0
        self._lastWindowH = 0
        self._lastDPIScale = 1.0

        """
		we are giving users a valid screen resolution option that is supported
		"""
        display_count = fife.Display.getDisplayCount()
        resolutions = []
        common = [
            (1024, 768),
            (1280, 720),
            (1366, 768),
            (1600, 900),
            (1920, 1080),
            (2560, 1440),
            (3840, 2160),
        ]

        for d in range(max(1, display_count)):
            mw = fife.Display.getDisplayWidth(d)
            mh = fife.Display.getDisplayHeight(d)
            if mw > 0 and mh > 0:
                resolutions.append(f"{mw}x{mh}")
                for w, h in common:
                    if w <= mw and h <= mh and f"{w}x{h}" not in resolutions:
                        resolutions.append(f"{w}x{h}")

        if not resolutions:
            resolutions = ["1280x720"]

        self._setting.setValidResolutions(resolutions)

        displays = list(range(max(1, display_count)))

        # Some SDL/drivers only list display 0, even with multiple monitors.
        # Ensure we have at least 2 entries so users can pick display 2 manually.
        if len(displays) <= 1:
            min_count = max(2, display_count)
            displays = list(range(min_count))

        self._setting.setValidDisplays(displays)

        self.quitRequested = False
        self.breakRequested = False
        self.returnValues = []

    def loadSettings(self):
        """
        Load the settings from a python file and load them into the engine.

        Called in the ApplicationBase constructor.
        """
        # get finalSetting (from the xml file, or if absent the default value)
        self._finalSetting = self._setting.getSettingsFromFile("FIFE", self._log)

        engineSetting = self.engine.getSettings()

        engineSetting.setBitsPerPixel(self._finalSetting["BitsPerPixel"])
        engineSetting.setInitialVolume(self._finalSetting["InitialVolume"])
        engineSetting.setSDLRemoveFakeAlpha(self._finalSetting["SDLRemoveFakeAlpha"])
        engineSetting.setGLCompressImages(self._finalSetting["GLCompressImages"])
        engineSetting.setGLUseFramebuffer(self._finalSetting["GLUseFramebuffer"])
        engineSetting.setGLUseNPOT(self._finalSetting["GLUseNPOT"])
        engineSetting.setGLUseMipmapping(self._finalSetting["GLUseMipmapping"])
        engineSetting.setGLUseMonochrome(self._finalSetting["GLUseMonochrome"])
        engineSetting.setGLUseDepthBuffer(self._finalSetting["GLUseDepthBuffer"])
        engineSetting.setGLAlphaTestValue(self._finalSetting["GLAlphaTestValue"])
        if self._finalSetting["GLTextureFiltering"] == "None":
            engineSetting.setGLTextureFiltering(fife.TEXTURE_FILTER_NONE)
        elif self._finalSetting["GLTextureFiltering"] == "Bilinear":
            engineSetting.setGLTextureFiltering(fife.TEXTURE_FILTER_BILINEAR)
        elif self._finalSetting["GLTextureFiltering"] == "Trilinear":
            engineSetting.setGLTextureFiltering(fife.TEXTURE_FILTER_TRILINEAR)
        elif self._finalSetting["GLTextureFiltering"] == "Anisotropic":
            engineSetting.setGLTextureFiltering(fife.TEXTURE_FILTER_ANISOTROPIC)
        width, height = self._finalSetting["ScreenResolution"].split("x")
        engineSetting.setScreenWidth(int(width))
        engineSetting.setScreenHeight(int(height))
        engineSetting.setRenderBackend(self._finalSetting["RenderBackend"])
        engineSetting.setFullScreen(self._finalSetting["FullScreen"])
        engineSetting.setRefreshRate(self._finalSetting["RefreshRate"])
        engineSetting.setDisplay(self._finalSetting["Display"])
        engineSetting.setWindowPositionX(self._finalSetting["WindowPositionX"])
        engineSetting.setWindowPositionY(self._finalSetting["WindowPositionY"])
        engineSetting.setVSync(self._finalSetting["VSync"])
        engineSetting.setWindowResizable(self._finalSetting["WindowResizable"])
        if self._finalSetting["WindowScalingMode"] == "Linear":
            engineSetting.setWindowScalingMode(0)
        elif self._finalSetting["WindowScalingMode"] == "Integer":
            engineSetting.setWindowScalingMode(1)
        elif self._finalSetting["WindowScalingMode"] == "Nearest":
            engineSetting.setWindowScalingMode(2)
        engineSetting.setVideoDriver(self._finalSetting["VideoDriver"])
        engineSetting.setSDLDriver(self._finalSetting["RenderDriver"])
        engineSetting.setLightingModel(self._finalSetting["Lighting"])
        engineSetting.setNativeImageCursorEnabled(self._finalSetting["NativeImageCursor"])
        engineSetting.setJoystickSupport(self._finalSetting["JoystickSupport"])

        engineSetting.setColorKeyEnabled(self._finalSetting["ColorKeyEnabled"])
        engineSetting.setColorKey(
            self._finalSetting["ColorKey"][0],
            self._finalSetting["ColorKey"][1],
            self._finalSetting["ColorKey"][2],
        )
        engineSetting.setWindowTitle(self._finalSetting["WindowTitle"])
        engineSetting.setWindowIcon(self._finalSetting["WindowIcon"])
        engineSetting.setFrameLimitEnabled(self._finalSetting["FrameLimitEnabled"])
        engineSetting.setFrameLimit(self._finalSetting["FrameLimit"])
        engineSetting.setMouseSensitivity(self._finalSetting["MouseSensitivity"])
        engineSetting.setMouseAccelerationEnabled(self._finalSetting["MouseAcceleration"])

        engineSetting.setFontPaths(self._finalSetting["FontPaths"])

    def initLogging(self):
        """Initialize the LogManager.

        Raises
        ------
        RuntimeError
            If engine settings are unavailable during logging initialization.
        """
        engineSetting = self.engine.getSettings()
        if not engineSetting:
            raise RuntimeError(
                "Engine settings unavailable during logging initialization"
            )
        logmodules = self._setting.get("FIFE", "LogModules", ["controller"])

        # log to both the console and log file
        self._log = fifelog.LogManager(
            self.engine,
            self._setting.get("FIFE", "LogToPrompt", False),
            self._setting.get("FIFE", "LogToFile", False),
        )

        self._log.setLevelFilter(
            self._setting.get("FIFE", "LogLevelFilter", fife.LogManager.LEVEL_DEBUG)
        )

        if logmodules:
            self._log.setVisibleModules(*logmodules)

    def createListener(self):
        """
        Create a default event listener.

        The listener will just close the program after pressing ESC.

        Override this method to provide your own event handling.

        Returns
        -------
        ExitEventListener
            A default event listener instance that quits on ESC.
        """
        return ExitEventListener(self)

    def run(self):
        """Initialize the event listener and event loop - and start it.

        Returns
        -------
        Any
            The value returned by `mainLoop`.
        """
        self._eventlistener = self.createListener()
        self.engine.initializePumping()
        retval = self.mainLoop()
        self.engine.finalizePumping()
        self.engine.destroy()
        return retval

    def mainLoop(self):
        """
        Run the program's main loop.

        Do not override this, instead provide your own L{_pump} method.
        You can call this recursively, e.g. to provide synchronous
        Dialogs :-) and break out of the current mainLoop by calling
        L{breakFromMainLoop}. It will return the argument passed
        to L{breakFromMainLoop}.

        Returns
        -------
        Any
            The value passed to `breakFromMainLoop` and returned when the
            main loop terminates.
        """
        self.returnValues.append(None)
        while not self.quitRequested:
            try:
                self.engine.pump()
            except fife.Exception as e:
                print(str(e))
                self.quitRequested = True

            self._pump()

            if self.breakRequested:
                self.breakRequested = False
                break

        return self.returnValues.pop()

    def breakFromMainLoop(self, returnValue):
        """
        Break from the currently running L{mainLoop}.

        The passed argument will be returned by the mainLoop.
        """
        self.returnValues[-1] = returnValue
        self.breakRequested = True

    def _pump(self):
        """
        Application pump.

        Derived classes can specialize this for unique behavior.
        This is called every frame.
        """
        win = self.engine.getWindow()
        if win:
            w = win.getWidthInPoints()
            h = win.getHeightInPoints()
            if w != self._lastWindowW or h != self._lastWindowH:
                self._lastWindowW = w
                self._lastWindowH = h
                self._onWindowResized(w, h)
            dpi = win.getDPIScaleFactor()
            if dpi != self._lastDPIScale:
                self._lastDPIScale = dpi
                self._onDPIScaleChanged(dpi)

    def _onWindowResized(self, width, height):
        """Handle window resize events.

        Override in subclasses for custom resize handling.
        """

    def _onDPIScaleChanged(self, scale):
        """Handle DPI scale change events.

        Override in subclasses for custom DPI change handling.
        """

    def quit(self):
        """Quit the application."""
        self.quitRequested = True
