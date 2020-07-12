module Main where

import Control.Applicative
import Data.Complex
import Foreign hiding (newArray)
import Control.Monad.State
import qualified Graphics.UI.SDL as SDL
import qualified Graphics.Rendering.OpenGL as GL
import Timer

-- TODO: might be needed for efficiency:
-- import Unsafe.Coerce(unsafeCoerce)
-- import Data.List(foldl')

screenWidth  = 1024
screenHeight = 1024
screenBpp    = 32

data GlobalData = GlobalData { texture :: GL.TextureObject, textureBuffer :: Ptr Word8, timer :: Timer }
type GlobalState = StateT GlobalData IO

type PixelWriter = Int -> Int -> Word8 -> Word8 -> Word8 -> IO ()

calculateEscape :: (RealFloat a) => Complex a -> Int -> Int
calculateEscape z n
    | m > 4 = n
    | n > 16 = n
    | otherwise = calculateEscape z' n'
 where
    m = magnitude z
    z' = (z**3 + c)
    n' = (n + 1)
    -- c = (0.3 :+ 0.7)
    c = log z

generateFractal :: PixelWriter -> Int -> Int -> IO ()
generateFractal write pixelWidth pixelHeight = do
    mapM_ drawPixel [(x, y) | x <- [0..pixelWidth-1], y <- [0..pixelHeight-1]]
 where
    size :: Complex Double
    size = 3.0 :+ 3.0
    center = 0.0 :+ 0.0
    normalizePixel w z = (fromIntegral w) / (fromIntegral z)
    pixelToComplexOffset x y = (realPart size) * (normalizePixel x pixelWidth) :+ (imagPart size) * (normalizePixel y pixelHeight)
    pixelToComplex x y = pixelToComplexOffset x y - (size - center) / 2
    drawPixel (x, y) = write x y 0 0 (10 + fromIntegral (10 * (calculateEscape (pixelToComplex x y) 0)))

writePixel :: Ptr Word8 -> Int -> PixelWriter
writePixel textureBuffer width x y r g b = do
    let p = textureBuffer `plusPtr` ((y * width + x) * 3)
    pokeByteOff p 0 r
    pokeByteOff p 1 g
    pokeByteOff p 2 b

renderFractal :: GL.TextureObject -> Ptr Word8 -> Int -> Int -> IO ()
renderFractal texture textureBuffer width height = do
    -- pokeArray textureBuffer (replicate (height * width * 3) (122 :: Word8))
    generateFractal (writePixel textureBuffer width) width height
    GL.texImage2D Nothing GL.NoProxy 0 GL.RGB' (GL.TextureSize2D (fromIntegral width) (fromIntegral height)) 0 (GL.PixelData GL.RGB GL.UnsignedByte textureBuffer)
    let v :: GL.GLfloat -> GL.GLfloat -> GL.GLfloat -> GL.GLfloat -> IO ()
        v tx ty vx vy = GL.texCoord (GL.TexCoord2 tx ty) >> GL.vertex (GL.Vertex2 vx vy)
        w = fromIntegral width
        h = fromIntegral height
    GL.renderPrimitive GL.Quads $ do
        v 0 1 0 0 >> v 0 0 0 1 >> v 1 0 1 1 >> v 1 1 1 0

render :: GL.TextureObject -> Ptr Word8 -> IO ()
render texture textureBuffer = do
    GL.clear [GL.ColorBuffer]
    renderFractal texture textureBuffer screenWidth screenHeight
    GL.flush
    SDL.glSwapBuffers

wait :: GlobalState ()
wait = do
    quit <- liftIO whileEvents
    liftIO $ SDL.delay 10
    unless quit wait

mainLoop :: GlobalState ()
mainLoop = do
    globalState <- get
    liftIO $ render (texture globalState) (textureBuffer globalState)
    wait

whileEvents :: IO Bool
whileEvents = do
    event <- liftIO SDL.pollEvent
    case event of
        SDL.Quit -> return True
        SDL.KeyUp (SDL.Keysym SDL.SDLK_q _ _) -> return True
        SDL.NoEvent -> return False
        _ -> whileEvents

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
    GL.ortho 0 1 0 1 (-1) 1
    GL.matrixMode GL.$= GL.Modelview 0
    GL.loadIdentity
    GL.texture GL.Texture2D GL.$= GL.Enabled

initialize :: IO GlobalData
initialize = do
    initializeSDL
    initializeOpenGL
    -- TODO Move?
    [texture] <- GL.genObjectNames 1
    GL.textureBinding GL.Texture2D GL.$= Just texture
    GL.textureWrapMode GL.Texture2D GL.S GL.$= (GL.Repeated, GL.ClampToEdge)
    GL.textureWrapMode GL.Texture2D GL.T GL.$= (GL.Repeated, GL.ClampToEdge)
    GL.textureFilter GL.Texture2D GL.$= ((GL.Linear', Nothing), GL.Linear')
    textureBuffer <- mallocBytes $ screenWidth * screenHeight * 3
    return $ GlobalData texture textureBuffer defaultTimer

main = SDL.withInit [SDL.InitEverything] $ do
    globalState <- initialize
    evalStateT mainLoop globalState
