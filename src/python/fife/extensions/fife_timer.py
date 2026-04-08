# SPDX-License-Identifier: LGPL-2.1-or-later
# SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

"""
Provide convenient timers.

Usage::
  import fife.extensions.fife_timer

  fife_timer.init( my_fife_engine.getTimeManager() )

  def spam():
     print "SPAM SPAM ",

  repeater = fife_timer.repeatCall(500,spam)

  def stop_spam():
     repeater.stop()
     print "BACON EGGS AND SPAM"

  delayed = fife_timer.delayCall(50000,stop_spam)

"""

from fife import fife

# global time manager
_manager = None


def init(timemanager):
    """
    Initialize timers.

    Parameters
    ----------
    timemanager : fife.TimeManager
        A fife.TimeManager as returned by ``fife.Engine.getTimeManager``.
    """
    global _manager
    _manager = timemanager


class Timer(fife.TimeEvent):
    """
    Timer.

    This class wraps the fife.TimeEvent class to make it easily usable from Python
    It allows for a TimeEvent to be executed once or multiple times.

    Remember FIFE::TimeManager does NOT delete the timer so make sure you keep a reference
    to this timer to ensure python doesnt delete the timer prematurely.
    """

    def __init__(self, delay=0, callback=None, repeat=0):
        """
        Initialize the timer.

        Parameters
        ----------
        delay : int
            The delay in milliseconds to execute the callback.
        callback : callable
            The function to execute when the time delay has passed.
        repeat : int
            The number of times to execute the callback. 1=once, 0=forever.
        """
        super().__init__(delay)
        self._active = False
        self._callback = callback
        self._manager = _manager
        self.setPeriod(delay)
        self._repeat = repeat
        self._executed = 0

    def start(self):
        """
        Call this to start the timer.

        This registers the timer with the time manager.  The time manger then
        calls the timers updateEvent() function when the delay time has passed.
        """
        if self._active:
            return
        self._active = True

        self._executed = 0

        self.setLastUpdateTime(self._manager.getTime())
        self._manager.registerEvent(self)

    def stop(self):
        """
        Stop the timer.

        Unregister the timer from the time manager.
        """
        if not self._active:
            return

        self._active = False
        self._manager.unregisterEvent(self)

    def updateEvent(self, delta):
        """
        Respond to time manager callback.

        Called by FIFE::TimeManager when the delay has passed. Should not be called directly.
        """
        if self._repeat != 0:
            self._executed += 1
            if self._executed >= self._repeat:
                self.stop()

        if callable(self._callback):
            self._callback()

    def _setDelay(self, delay):
        """
        Set the delay in milliseconds.

        The timer must not be active to change this value.

        Parameters
        ----------
        delay : int
            Number of milliseconds to wait before executing the callback.
        """
        if not self._active:
            self.setPeriod(delay)

    def _getDelay(self):
        """
        Get the delay in milliseconds.

        Returns
        -------
        int
            Number of milliseconds.
        """
        return self.getPeriod()

    def _setCallback(self, callback):
        self._callback = callback

    def _getCallback(self):
        return self._callback

    def _setRepeat(self, repeat):
        """
        Set the number of repeats.

        The timer must not be active to change its repeat value.

        Parameters
        ----------
        repeat : int
            Number of times to repeat the timer. 0=forever, 1=once.
        """
        if not self._active:
            self._repeat = repeat

    def _getRepeat(self):
        """
        Get the number of repeats.

        Returns
        -------
        int
            Number of times the timer will be executed.
        """
        return self._repeat

    def _getActive(self):
        """
        Get the active state.

        Returns
        -------
        bool
            True if timer is active, False otherwise.
        """
        return self._active

    def _getNumExecuted(self):
        """
        Get the number of executions.

        Returns
        -------
        int
            Number of times the timer has been executed.
        """
        return self._executed

    delay = property(_getDelay, _setDelay)
    callback = property(_getCallback, _setCallback)
    repeat = property(_getRepeat, _setRepeat)
    active = property(_getActive)
    numexecuted = property(_getNumExecuted)


def delayCall(delay, callback):
    """
    Delay a function call by a number of milliseconds.

    Remember to keep a reference to the timer this function returns.  If you
    do not python will delete the timer prematurely which may case a segfault.

    Parameters
    ----------
    delay : int
        Delay in milliseconds.
    callback : callable
        The function to call.

    Returns
    -------
    Timer
        The timer.
    """
    timer = Timer(delay, callback, 1)
    timer.start()
    return timer


def repeatCall(period, callback):
    """
    Repeat a function call. The call is repeated until the timer is stopped.

    Remember to keep a reference to the timer this function returns.  If you
    do not python will delete the timer prematurely which may case a segfault.

    Parameters
    ----------
    period : int
        Period between calls in milliseconds.
    callback : callable
        The function to call.

    Returns
    -------
    Timer
        The timer.
    """
    timer = Timer(period, callback, 0)
    timer.start()
    return timer


__all__ = ["init", "Timer", "delayCall", "repeatCall"]
