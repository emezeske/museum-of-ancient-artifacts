{-# LANGUAGE FlexibleContexts #-}
module Main where

import Control.Monad.State
import Control.Monad.Reader
import qualified Graphics.UI.SDL as SDL
import qualified Graphics.Rendering.OpenGL as GL
import Timer

screenWidth  = 640
screenHeight = 480
screenBpp    = 32

type Vector2D = (GL.GLfloat, GL.GLfloat)
data Rectangle = Rectangle {
    pos :: Vector2D,
    vel :: Vector2D,
    angle :: GL.GLfloat,
    angleVel :: GL.GLfloat,
    size :: Vector2D
}

rectangleSpeed :: GL.GLfloat
rectangleSpeed = 500.0

defaultRectangle :: Rectangle
defaultRectangle = Rectangle { pos=(0.0, 0.0), vel=(0.0, 0.0), angle=0.0, angleVel=0.0, size=(128.0, 128.0) }

changeRectangleVel :: Rectangle -> Vector2D -> Rectangle
changeRectangleVel rectangle@Rectangle { vel=(dx, dy) } (dx', dy') =
    rectangle { vel=(dx + dx', dy + dy') }

changeRectangleAngleVel :: Rectangle -> GL.GLfloat -> Rectangle
changeRectangleAngleVel rectangle@Rectangle { angleVel=dt } dt' =
    rectangle { angleVel=(dt + dt') }

simulateRectangle :: TimerSeconds -> Rectangle -> Rectangle
simulateRectangle delta rectangle@Rectangle { pos=(x, y), vel=(dx, dy), angle=t, angleVel=dt } = 
    rectangle { pos=(x', y'), angle=t' } 
 where
    applyVelocity a da delta = a + da * (realToFrac delta)
    x' = applyVelocity x dx delta
    y' = applyVelocity y dy delta
    t' = applyVelocity t dt delta

data AppData = AppData { rectangle :: Rectangle, timer :: Timer }
type AppState = StateT AppData IO

getTimer :: AppState Timer
getTimer = liftM timer get

putTimer :: Timer -> AppState ()
putTimer timer = modify $ \s -> s { timer = timer }

getTimerTicksElapsed :: AppState TimerTicks
getTimerTicksElapsed = do
    timer <- getTimer
    (ticksElapsed, newTimer) <- liftIO $ getTicksElapsed timer
    putTimer newTimer
    return ticksElapsed

getRectangle :: (MonadState AppData m) => m Rectangle
getRectangle = liftM rectangle get

putRectangle :: (MonadState AppData m) => Rectangle -> m ()
putRectangle rectangle = modify $ \s -> s { rectangle = rectangle }

modifyRectangleM :: (MonadState AppData m) => (Rectangle -> m Rectangle) -> m ()
modifyRectangleM action = getRectangle >>= action >>= putRectangle

modifyRectangle :: (MonadState AppData m) => (Rectangle -> Rectangle) -> m ()
modifyRectangle action = liftM action getRectangle >>= putRectangle

initGL :: IO ()
initGL = do
    GL.clearColor GL.$= GL.Color4 (0 :: GL.GLclampf) 0 0 0
    GL.matrixMode GL.$= GL.Projection
    GL.loadIdentity    
    GL.ortho 0 (fromIntegral screenWidth) (fromIntegral screenHeight) 0 (-1) 1
    GL.matrixMode GL.$= GL.Modelview 0
    GL.loadIdentity

initApp :: IO AppData
initApp = do
    SDL.glSetAttribute SDL.glDoubleBuffer 1    
    screen <- SDL.setVideoMode screenWidth screenHeight screenBpp [SDL.OpenGL]
    SDL.setCaption "OpenGL Test" []
    initGL
    return $ AppData defaultRectangle defaultTimer

drawRectangle :: Rectangle -> IO ()
drawRectangle Rectangle { pos=(x,y), size=(sx,sy), angle=theta } = do
    GL.preservingMatrix $ do
        GL.translate $ GL.Vector3 x y 0.0
        GL.rotate theta $ GL.Vector3 0.0 0.0 1.0
        GL.renderPrimitive GL.Quads $ do
            GL.color $ GL.Color4 (0.0 :: GL.GLfloat) 0.0 1.0 1.0
            let lx = 0.0 - 0.5 * sx
                ly = 0.0 - 0.5 * sy
                ux = 0.5 * sx
                uy = 0.5 * sy
            GL.vertex $ GL.Vertex2 lx ly
            GL.vertex $ GL.Vertex2 lx uy
            GL.vertex $ GL.Vertex2 ux uy
            GL.vertex $ GL.Vertex2 ux ly

handleInput :: SDL.Event -> Rectangle -> Rectangle
handleInput (SDL.KeyDown (SDL.Keysym SDL.SDLK_w _ _)) rectangle = changeRectangleVel rectangle (0.0, -rectangleSpeed)
handleInput (SDL.KeyDown (SDL.Keysym SDL.SDLK_s _ _)) rectangle = changeRectangleVel rectangle (0.0, rectangleSpeed)
handleInput (SDL.KeyDown (SDL.Keysym SDL.SDLK_a _ _)) rectangle = changeRectangleVel rectangle (-rectangleSpeed, 0.0)
handleInput (SDL.KeyDown (SDL.Keysym SDL.SDLK_d _ _)) rectangle = changeRectangleVel rectangle (rectangleSpeed, 0.0)

handleInput (SDL.KeyUp (SDL.Keysym SDL.SDLK_w _ _)) rectangle = changeRectangleVel rectangle (0.0, rectangleSpeed)
handleInput (SDL.KeyUp (SDL.Keysym SDL.SDLK_s _ _)) rectangle = changeRectangleVel rectangle (0.0, -rectangleSpeed)
handleInput (SDL.KeyUp (SDL.Keysym SDL.SDLK_a _ _)) rectangle = changeRectangleVel rectangle (rectangleSpeed, 0.0)
handleInput (SDL.KeyUp (SDL.Keysym SDL.SDLK_d _ _)) rectangle = changeRectangleVel rectangle (-rectangleSpeed, 0.0)

handleInput (SDL.KeyDown (SDL.Keysym SDL.SDLK_e _ _)) rectangle = changeRectangleAngleVel rectangle rectangleSpeed
handleInput (SDL.KeyDown (SDL.Keysym SDL.SDLK_q _ _)) rectangle = changeRectangleAngleVel rectangle (-rectangleSpeed)

handleInput (SDL.KeyUp (SDL.Keysym SDL.SDLK_e _ _)) rectangle = changeRectangleAngleVel rectangle (-rectangleSpeed)
handleInput (SDL.KeyUp (SDL.Keysym SDL.SDLK_q _ _)) rectangle = changeRectangleAngleVel rectangle rectangleSpeed

handleInput _ rectangle = rectangle

loop :: AppState ()
loop = do
    ticksElapsed <- getTimerTicksElapsed
    quit <- whileEvents $ modifyRectangle . handleInput

    let secondsElapsed = timerTicksToSeconds ticksElapsed
    modifyRectangle $ simulateRectangle secondsElapsed
    rectangle <- getRectangle
    
    liftIO $ do
        GL.clear [GL.ColorBuffer]
        drawRectangle rectangle
        GL.flush
        SDL.glSwapBuffers
        
        when (ticksElapsed < frameTicks) $ do
            SDL.delay $ frameTicks - ticksElapsed

    unless quit loop
 where
    framesPerSecond = 60
    frameTicks = (secondsToTimerTicks 1.0) `div` framesPerSecond

whileEvents :: MonadIO m => (SDL.Event -> m ()) -> m Bool
whileEvents handleEvent = do
    event <- liftIO SDL.pollEvent
    case event of
        SDL.Quit -> return True
        SDL.NoEvent -> return False
        _ -> do
            handleEvent event
            whileEvents handleEvent

main = SDL.withInit [SDL.InitEverything] $ do
    state <- initApp
    evalStateT loop state
