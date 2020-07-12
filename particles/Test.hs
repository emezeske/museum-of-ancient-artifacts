module Main where

import Data.Array.Storable
import Foreign.Ptr(nullPtr)
import Unsafe.Coerce(unsafeCoerce)
import Data.List(foldl')
import Control.Monad.State
import qualified Data.Vect.Float.Base as V
import qualified Data.Vect.Float.Instances as V
import qualified Graphics.UI.SDL as SDL
import qualified Graphics.Rendering.OpenGL as GL
import Timer

screenWidth  = 1024
screenHeight = 1024
screenBpp    = 32

data GlobalData = GlobalData { world :: WorldData, timer :: Timer, bufferName :: GL.BufferObject }
type GlobalState = StateT GlobalData IO

-- TODO: Look into Data.Vector.Vector or Data.Array for more efficient particle lists.
data WorldData = WorldData { particles :: [Particle], emitters :: [ParticleEmitter], attractors :: [ParticleAttractor] }
type WorldState = State WorldData

data Particle = 
    Particle { pos :: V.Vec2
             , vel :: V.Vec2
             , radius :: Float
             , color :: GL.Color4 GL.GLfloat
             } deriving (Show)

type ParticleEmitter = TimerSeconds -> [Particle] -> [Particle]

type ParticleAttractor = TimerSeconds -> Particle -> Particle

getParticles :: WorldState [Particle]
getParticles = liftM particles get

putParticles :: [Particle] -> WorldState ()
putParticles particles = modify $ \s -> s { particles = particles }

modifyParticles :: ([Particle] -> [Particle]) -> WorldState ()
modifyParticles action = liftM action getParticles >>= putParticles

-- TODO Use delta
emitParticle :: V.Vec2 -> V.Vec2 -> Float -> GL.Color4 GL.GLfloat -> ParticleEmitter
emitParticle p v r c delta particles = particle:particles
 where
    particle = Particle { pos=p, vel=v, radius=r, color=c }

attractParticle :: V.Vec2 -> Float -> ParticleAttractor
attractParticle pA mA delta particle@Particle { pos=pP, vel=vP } =
    particle { vel=(vP + dvP) }
 where
    vPA = pA - pP           -- Vector from particle to attractor
    dot = V.dotprod vPA vPA -- Square of distance from partical to attractor
    dvP = (delta * mA / dot) `V.scalarMul` vPA

simulateEmitters :: TimerSeconds -> [ParticleEmitter] -> [Particle] -> [Particle]
simulateEmitters delta emitters particles = foldl' (emit delta) particles emitters
 where
    emit delta particles emitter = emitter delta particles

simulateAttractors :: TimerSeconds -> [ParticleAttractor] -> [Particle] -> [Particle]
simulateAttractors delta attractors particles = foldl' (attract delta) particles attractors
 where
    attract delta particles attractor = map (attractor delta) particles

simulateParticle :: TimerSeconds -> Particle -> Particle
simulateParticle delta particle@Particle { pos=p, vel=v } = 
    particle { pos=(p + delta `V.scalarMul` v) } 

initializeSDL :: IO ()
initializeSDL = do
    SDL.glSetAttribute SDL.glDoubleBuffer 1    
    screen <- SDL.setVideoMode screenWidth screenHeight screenBpp [SDL.OpenGL]
    SDL.setCaption "OpenGL Test" []

initializeOpenGL :: IO ()
initializeOpenGL = do
    GL.clearColor GL.$= GL.Color4 (0 :: GL.GLclampf) 0 0 0
    GL.blend GL.$= GL.Enabled
    GL.blendFunc GL.$= (GL.SrcAlpha, GL.OneMinusSrcAlpha)
    GL.matrixMode GL.$= GL.Projection
    GL.loadIdentity    
    GL.ortho (-1.0) 1.0 (-1.0) 1.0 (-1.0) 1.0
    GL.matrixMode GL.$= GL.Modelview 0
    GL.loadIdentity

initializeVBO :: IO (GL.BufferObject)
initializeVBO = do
    [bufferName] <- GL.genObjectNames 1 :: IO [GL.BufferObject]
    GL.bindBuffer GL.ArrayBuffer GL.$= Just bufferName
    array <- newListArray (0, lengthVertices - 1) vertices :: IO (StorableArray Int GL.GLfloat)
    withStorableArray array (\ptr -> GL.bufferData GL.ArrayBuffer GL.$= ((fromIntegral sizeVertices), ptr, GL.StaticDraw))
    GL.vertexAttribArray (GL.AttribLocation 0) GL.$= GL.Enabled
    GL.vertexAttribPointer (GL.AttribLocation 0) GL.$= (GL.ToFloat, GL.VertexArrayDescriptor 2 GL.Float 0 (nullPtr))
    return bufferName
 where
    -- TODO: Insert colors into the VBO as well.
    -- TODO: Note that radius is broken
    vertices = [0.01, 0.01, 0.0, 0.01, 0.01, 0.0, 0.0, 0.0] :: [GL.GLfloat]
    lengthVertices = length vertices
    elementSize = 4
    sizeVertices = lengthVertices * elementSize

initialize :: IO GlobalData
initialize = do
    initializeSDL
    initializeOpenGL
    bufferName <- initializeVBO
    return $ GlobalData WorldData { particles=particles, emitters=emitters, attractors=attractors } defaultTimer bufferName
 where
    -- FIXME: Static particles in use for performanc testing
    particles = []
    emitters = [emitParticle (V.Vec2 0.0 0.0) (V.Vec2 0.20 0.01) 0.01 (GL.Color4 1.0 0.0 0.0 0.2),
                emitParticle (V.Vec2 0.0 0.0) (V.Vec2 0.01 0.20) 0.01 (GL.Color4 0.0 0.0 1.0 0.2),
                emitParticle (V.Vec2 0.0 0.0) (V.Vec2 (-0.20) (-0.01)) 0.01 (GL.Color4 0.0 1.0 0.0 0.2),
                emitParticle (V.Vec2 0.0 0.0) (V.Vec2 (-0.01) (-0.20)) 0.01 (GL.Color4 0.5 0.0 0.5 0.2)]
    -- particles = take 20000 (repeat $ Particle { pos=V.Vec2 0.0 0.0, vel=V.Vec2 0.0 0.05, radius=0.01, color=GL.Color4 0.0 0.0 1.0 0.5 })
    -- emitters = []
    attractors = [attractParticle (V.Vec2 (-0.5) 0.5) 0.1,
                  attractParticle (V.Vec2 0.5 (-0.5)) 0.1,
                  attractParticle (V.Vec2 0.5 0.5) 0.1,
                  attractParticle (V.Vec2 (-0.5) (-0.5)) 0.1]

drawParticle :: GL.BufferObject -> Particle -> IO ()
drawParticle bufferName Particle { pos=(V.Vec2 x y), radius=r, color=color } = do
    GL.preservingMatrix $ do
        GL.translate $ makeVector3 (x - 0.5 * r) (y - 0.5 * r) 0
        GL.color color
        GL.bindBuffer GL.ArrayBuffer GL.$= Just bufferName
        GL.drawArrays GL.TriangleStrip (0 :: GL.GLint) (4 :: GL.GLsizei)
        GL.bindBuffer GL.ArrayBuffer GL.$= Nothing

--    GL.preservingMatrix $ do
--        GL.translate $ makeVector3 (x - r / 2) (y - r / 2) 0
--        GL.color color
--        GL.renderPrimitive GL.TriangleStrip $ do
--            GL.vertex $ makeVertex2 r r
--            GL.vertex $ makeVertex2 0 r
--            GL.vertex $ makeVertex2 r 0
--            GL.vertex $ makeVertex2 0 0
 where
    -- NOTE: The safe way to convert these Float values into GL.GLfloat values is to use
    -- realToFrac.  However, realToFrac is EXTREMELY inefficient.
    -- makeVertex2 a b = GL.Vertex2 (unsafeCoerce a :: GL.GLfloat) (unsafeCoerce b)
    makeVector3 a b c = GL.Vector3 (unsafeCoerce a :: GL.GLfloat) (unsafeCoerce b) (unsafeCoerce c)

simulateWorld :: TimerSeconds -> WorldState ()
simulateWorld secondsElapsed = do
    modifyParticles $ map (simulateParticle secondsElapsed)
    emitters <- liftM emitters get
    attractors <- liftM attractors get
    modifyParticles $ simulateEmitters secondsElapsed emitters
    modifyParticles $ simulateAttractors secondsElapsed attractors

renderWorld :: GL.BufferObject -> WorldData -> TimerTicks -> IO ()
renderWorld bufferName world ticksElapsed = do
    GL.clear [GL.ColorBuffer]
    GL.clientState GL.VertexArray GL.$= GL.Enabled
    mapM_ (drawParticle bufferName) $ particles world
    GL.clientState GL.VertexArray GL.$= GL.Disabled
    GL.flush
    SDL.glSwapBuffers
    when (ticksElapsed < frameTicks) $ do
        SDL.delay $ frameTicks - ticksElapsed
 where
    framesPerSecond = 60
    frameTicks = (secondsToTimerTicks 1.0) `div` framesPerSecond

mainLoop :: GlobalState ()
mainLoop = do
    quit <- liftIO whileEvents
    globalState <- get

    let currentTimer = timer globalState
    (ticksElapsed, newTimer) <- liftIO $ getTicksElapsed currentTimer

    let currentWorld = world globalState
        newWorld = execState (simulateWorld (timerTicksToSeconds ticksElapsed)) currentWorld

    liftIO $ renderWorld (bufferName globalState) newWorld ticksElapsed

    put globalState { timer=newTimer, world=newWorld }
    unless quit mainLoop

whileEvents :: IO Bool
whileEvents = do
    event <- liftIO SDL.pollEvent
    case event of
        SDL.Quit -> return True
        SDL.KeyUp (SDL.Keysym SDL.SDLK_q _ _) -> return True
        SDL.NoEvent -> return False
        _ -> whileEvents

main = SDL.withInit [SDL.InitEverything] $ do
    globalState <- initialize
    evalStateT mainLoop globalState
