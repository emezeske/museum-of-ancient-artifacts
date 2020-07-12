{-# LANGUAGE FlexibleContexts #-}
module Main where

import Control.Monad.State
import Control.Monad.Reader
import qualified Graphics.UI.SDL as SDL
import qualified Graphics.Rendering.OpenGL as GL
import Timer

screenWidth  = 1024
screenHeight = 1024
screenBpp    = 32

type Vector2D = (GL.GLfloat, GL.GLfloat)
data Rectangle = 
    Rectangle { pos :: Vector2D
              , vel :: Vector2D
              , angle :: GL.GLfloat
              , angleVel :: GL.GLfloat
              , size :: Vector2D
              , color :: GL.Color4 GL.GLfloat
              }

rectangleSpeed :: GL.GLfloat
rectangleSpeed = 500.0

defaultRectangle :: Rectangle
defaultRectangle =
    Rectangle { pos=((fromIntegral screenWidth) / 2.0, (fromIntegral screenHeight) / 2.0)
              , vel=(0.0, 0.0)
              , angle=0.0
              , angleVel=5.0
              , size=(fromIntegral screenWidth, fromIntegral screenHeight)
              , color=(GL.Color4 0.1 0.1 1.0 1.0)
              }

recursiveRectangles :: Rectangle -> [Rectangle]
recursiveRectangles parent@Rectangle { pos=(x, y), size=(sx, sy), angleVel=angleVel, color=(GL.Color4 r g b a) } =
    child:recursiveRectangles child
 where
    child = parent { pos=(x, y), size=(sx / (1.01), sy / (1.01)), angleVel=(1.01 * angleVel), color=childColor }
    childColor = GL.Color4 (1.01 * r) (1.00 * g) (0.99 * b) a
--    childColor
--        | b == 1.0 = GL.Color4 0.0 0.0 0.0 a
--        | otherwise = GL.Color4 0.0 0.0 1.0 a

defaultRectangles :: [Rectangle]
defaultRectangles = defaultRectangle:(take 10000 $ recursiveRectangles defaultRectangle)

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
    fdelta = realToFrac delta
    applyVelocity a da = a + da * fdelta
    x' = applyVelocity x dx
    y' = applyVelocity y dy
    t' = applyVelocity t dt

data AppData = AppData { rectangles :: [Rectangle], timer :: Timer }
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

getRectangles :: (MonadState AppData m) => m [Rectangle]
getRectangles = liftM rectangles get

putRectangles :: (MonadState AppData m) => [Rectangle] -> m ()
putRectangles rectangles = modify $ \s -> s { rectangles = rectangles }

modifyRectangles :: (MonadState AppData m) => ([Rectangle] -> [Rectangle]) -> m ()
modifyRectangles action = liftM action getRectangles >>= putRectangles

simulateRectangles :: TimerSeconds -> [Rectangle] -> [Rectangle]
simulateRectangles delta rectangles = map (simulateRectangle delta) rectangles

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
    return $ AppData defaultRectangles defaultTimer

drawRectangle :: Rectangle -> IO ()
drawRectangle Rectangle { pos=(x,y), size=(sx,sy), angle=theta, color=color } = do
    GL.preservingMatrix $ do
        GL.translate $ GL.Vector3 x y 0.0
        GL.rotate theta $ GL.Vector3 0.0 0.0 1.0
        GL.renderPrimitive GL.Quads $ do
            GL.color color
            let lx = 0.0 - 0.5 * sx
                ly = 0.0 - 0.5 * sy
                ux = 0.5 * sx
                uy = 0.5 * sy
            GL.vertex $ GL.Vertex2 lx ly
            GL.vertex $ GL.Vertex2 lx uy
            GL.vertex $ GL.Vertex2 ux uy
            GL.vertex $ GL.Vertex2 ux ly

loop :: AppState ()
loop = do
    ticksElapsed <- getTimerTicksElapsed
    quit <- whileEvents

    let secondsElapsed = timerTicksToSeconds ticksElapsed
    modifyRectangles $ simulateRectangles secondsElapsed
    rectangles <- getRectangles
    
    liftIO $ do
        GL.clear [GL.ColorBuffer]
        mapM_ drawRectangle rectangles
        GL.flush
        SDL.glSwapBuffers
        
        when (ticksElapsed < frameTicks) $ do
            SDL.delay $ frameTicks - ticksElapsed

    unless quit loop
 where
    framesPerSecond = 60
    frameTicks = (secondsToTimerTicks 1.0) `div` framesPerSecond

whileEvents :: MonadIO m => m Bool
whileEvents = do
    event <- liftIO SDL.pollEvent
    case event of
        SDL.Quit -> return True
        SDL.NoEvent -> return False
        _ -> whileEvents

main = SDL.withInit [SDL.InitEverything] $ do
    state <- initApp
    evalStateT loop state
