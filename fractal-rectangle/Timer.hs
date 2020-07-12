module Timer (
    Timer(),
    TimerTicks,
    TimerSeconds,
    defaultTimer,
    getTicksElapsed,
    stop,
    timerTicksToSeconds,
    secondsToTimerTicks
) where

import qualified Graphics.UI.SDL.Time as SDLTime
import Data.Word

data Timer = Timer { startTicks :: TimerTicks }
type TimerTicks = Word32
type TimerSeconds = Float

defaultTimer :: Timer
defaultTimer = Timer { startTicks=0 }

getTicksElapsed :: Timer -> IO (TimerTicks, Timer)
getTicksElapsed Timer { startTicks=0 } = do
    currentTicks <- SDLTime.getTicks
    return (0, Timer { startTicks=currentTicks })
getTicksElapsed Timer { startTicks=startTicks } = do
    currentTicks <- SDLTime.getTicks
    let elapsed = currentTicks - startTicks
        newTimer = Timer { startTicks=currentTicks }
    return (elapsed, newTimer)

stop :: Timer -> Timer
stop timer = timer { startTicks=0 }

timerTicksToSeconds :: TimerTicks -> TimerSeconds
timerTicksToSeconds ticks = (fromIntegral ticks) / 1000.0

secondsToTimerTicks :: TimerSeconds -> TimerTicks
secondsToTimerTicks seconds = round $ 1000 * seconds
