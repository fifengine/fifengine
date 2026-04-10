# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""Logging utilities for FIFE engine."""

from fife import fife


class LogManager:
    """
    Provide convenient APIs to access engine logging functionality.

    You can set log targets individually (prompt, file). You can also adjust
    things like visible modules through log manager.
    """

    def __init__(self, engine, promptlog=True, filelog=False):
        """
        Construct a new log manager.

        Parameters
        ----------
        engine : fife.Engine
            Engine to hook into.
        promptlog : bool
            If True, logs to prompt.
        filelog : bool
            If True, logs to file (fife.log).
        """
        self.engine = engine
        self.lm = engine.getLogManager()
        self.lm.setLogToPrompt(promptlog)
        self.lm.setLogToFile(filelog)
        self.mod2name = {}
        for k, v in list(fife.__dict__.items()):
            if k.startswith("LM_") and k not in ("LM_CORE", "LM_MODULE_MAX"):
                self.mod2name[v] = self.lm.getModuleName(v)
        self.name2mod = {v.lower(): k for k, v in list(self.mod2name.items())}

    def addVisibleModules(self, *names):
        """
        Add modules that are visible in logs.

        By default, all modules are disabled. Does not remove previously visible modules.

        Parameters
        ----------
        names : tuple[str]
            Module names to set visible.

        See Also
        --------
        modules.h
            File for available modules in the engine.
        """
        names = [n.lower() for n in names]
        if "all" in names:
            for k in list(self.mod2name.keys()):
                self.lm.addVisibleModule(k)
        else:
            for m in names:
                try:
                    self.lm.addVisibleModule(self.name2mod[m])
                except KeyError:
                    print(f'Tried to enable non-existing log module "{m}"')

    def removeVisibleModules(self, *names):
        """
        Remove modules that are visible in logs.

        By default, all modules are disabled.

        Parameters
        ----------
        names : tuple[str]
            Module names to set invisible.

        See Also
        --------
        addVisibleModules
        """
        names = [n.lower() for n in names]
        if "all" in names:
            for k in list(self.mod2name.keys()):
                self.lm.removeVisibleModule(k)
        else:
            for m in names:
                self.lm.removeVisibleModule(self.name2mod[m])

    def getVisibleModules(self):
        """Get currently visible modules.

        Returns
        -------
        list
            List of currently visible module names.

        See Also
        --------
        addVisibleModules
        """
        mods = []
        for k in list(self.mod2name.keys()):
            if self.lm.isVisible(k):
                mods.append(self.mod2name[k])
        return mods

    def setVisibleModules(self, *names):
        """Set visible modules, clearing previously set modules.

        Parameters
        ----------
        names : tuple[str]
            Module names to set visible.

        See Also
        --------
        addVisibleModules
        """
        self.lm.clearVisibleModules()
        self.addVisibleModules(*names)

    def setLevelFilter(self, fltr):
        """
        Set the minimum log level to view.

        Parameters
        ----------
        fltr : int
            The filter level.

        Notes
        -----
        Valid values include the constants on :class:`fife.LogManager`, for
        example ``fife.LogManager.LEVEL_DEBUG``, ``fife.LogManager.LEVEL_LOG``,
        ``fife.LogManager.LEVEL_WARN``, and ``fife.LogManager.LEVEL_ERROR``.
        """
        self.lm.setLevelFilter(fltr)

    def setLogToPrompt(self, promptlog):
        """Set whether to log to prompt.

        Returns
        -------
        bool
            Result of setting the prompt log state as reported by the engine.
        """
        return self.lm.setLogToPrompt(promptlog)

    def getLogToPrompt(self):
        """Get whether logging to prompt is enabled.

        Returns
        -------
        bool
            True if prompt logging is enabled, False otherwise.
        """
        return self.lm.isLogToPrompt()

    logToPrompt = property(getLogToPrompt, setLogToPrompt)

    def setLogToFile(self, filelog):
        """Set whether to log to file.

        Returns
        -------
        bool
            Result of setting the file log state as reported by the engine.
        """
        return self.lm.setLogToFile(filelog)

    def getLogToFile(self):
        """Get whether logging to file is enabled.

        Returns
        -------
        bool
            True if file logging is enabled, False otherwise.
        """
        return self.lm.isLogToFile()

    logToFile = property(getLogToFile, setLogToFile)

    def log_debug(self, message):
        """Log a debug message."""
        self.lm.log(0, self.name2mod["script"], message)

    def log_log(self, message):
        """Log a standard message."""
        self.lm.log(1, self.name2mod["script"], message)

    def log_warn(self, message):
        """Log a warning message."""
        self.lm.log(2, self.name2mod["script"], message)

    def log_error(self, message):
        """Log an error message."""
        self.lm.log(3, self.name2mod["script"], message)
