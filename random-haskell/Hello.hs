{-# LANGUAGE BangPatterns #-}
import Control.Concurrent (forkIO, killThread, yield)
import Control.Concurrent.Chan (Chan, newChan, readChan, writeChan)
import Control.Monad (forever, replicateM, when)
import Control.Monad.Trans (liftIO)
import Data.Array.IO (IOUArray, newArray, readArray, writeArray, inRange)
import Data.Complex (Complex((:+)))
import Data.IORef (IORef, newIORef, readIORef, writeIORef)

import Data.PriorityQueue (PriorityQueue, newPriorityQueue, enqueue, dequeue)

import Foreign hiding (newArray)

import Graphics.UI.Gtk
import Graphics.UI.Gtk.OpenGL
import qualified Graphics.Rendering.OpenGL as GL
import Graphics.Rendering.OpenGL (($=), GLfloat)

type B = Word8
type N = Int
type R = Double
type C = Complex R

hsv2rgb :: R -> R -> R -> (R, R, R)
hsv2rgb !h !s !v
  | s == 0 = (v, v, v)
  | h == 1 = hsv2rgb 0 s v
  | otherwise =
      let !i = floor (h * 6) `mod` 6 :: N
          !f = (h * 6) - fromIntegral i
          !p = v * (1 - s)
          !q = v * (1 - s * f)
          !t = v * (1 - s * (1 - f))
      in  case i of
            0 -> (v, t, p)
            1 -> (q, v, p)
            2 -> (p, v, t)
            3 -> (p, q, v)
            4 -> (t, p, v)
            5 -> (v, p, q)
            _ -> (0, 0, 0)

colour :: C -> C -> N -> (B, B, B)
colour !z@(zr:+zi) !dz !n =
  let !il2 = 1 / log 2
      !zd2  = magnitude2 z
      !dzd2 = magnitude2 dz
      !d = (fromIntegral n :: R) - log (log zd2 / log escapeR2) * il2
      !dwell = fromIntegral (floor d :: N)
      !finala = atan2 zi zr
      !de = (log zd2 * il2) * sqrt zd2 / sqrt dzd2
      !dscale = log de * il2 + 32
      !hue = log d * il2 / 3
      !saturation = 0 `max` (log d * il2 / 8) `min` 1
      !value = 0 `max` (1 - dscale / 48) `min` 1
      !h = hue - fromIntegral (floor hue :: N)
      !k = dwell / 2
      !satf = if k - fromIntegral (floor k :: N) >= (0.5 :: R) then 0.9 else 1
      !valf = if finala < 0 then 0.9 else 1
      (!r, !g, !b) = hsv2rgb h (satf * saturation) (valf * value)
      !rr = floor $ 0 `max` 255 * r `min` 255
      !gg = floor $ 0 `max` 255 * g `min` 255
      !bb = floor $ 0 `max` 255 * b `min` 255
  in  (rr, gg, bb)

data Job = Job !N !N !N !C !C !C !N

priority :: Job -> (N, N)
priority (Job k _ _ _ _ _ n) = (n, k)

display :: C -> N -> PriorityQueue IO Job -> Chan Job -> IO ()
display c zoom todo done = do
  let rng = ((0, 0), (height - 1, width - 1))
  sync <- newArray rng False
  count <- newIORef 0
  mapM_ (\(j, i) -> e count sync i j) border
  forever $ do
    Job _ i j _ _ _ _ <- readChan done
    sequence_
      [ e count sync x y
      | u <- spread
      , v <- spread
      , let x = i + u
      , let y = j + v
      , inRange rng (y, x)
      ]
  where
    e count sync = addJob count c zoom todo sync

addJob :: IORef N -> C -> N -> PriorityQueue IO Job -> IOUArray (N,N) Bool -> N -> N -> IO ()
addJob count c zoom todo sync i j = do
  already <- readArray sync (j, i)
  when (not already) $ do
    writeArray sync (j, i) True
    k <- readIORef count
    writeIORef count $! k + 1
    enqueue todo $ Job k i j (coords c zoom i j) 0 0 0

renderer :: (N -> N -> B -> B -> B -> IO ()) -> C -> N -> IO (IO ())
renderer output c zoom = do
  queue <- newPriorityQueue priority
  done <- newChan
  displayt  <- forkIO $ display c zoom queue done
  computets <- replicateM 4 (forkIO $ compute output done queue)
  return $ do
    mapM_ killThread computets
    killThread displayt

coords :: C -> N -> N -> N -> C
coords c zoom i j = c + (fromIntegral (i - width`div`2) / 2^^zoom :+ fromIntegral (height`div`2 - j) / 2^^zoom)

border :: [(N, N)]
border = concat
  [ [ (0, i) | i <- [ 0 .. width  - 1 ] ]
  , [ (j, 0) | j <- [ 1 .. height - 2 ] ]
  , [ (height - 1, i) | i <- [ 0 .. width  - 2 ] ]
  , [ (j,  width - 1) | j <- [ 1 .. height - 1 ] ]
  ]

compute :: (N -> N -> B -> B -> B -> IO ()) -> Chan Job -> PriorityQueue IO Job -> IO ()
compute output done todo = forever $ do
  mjob <- dequeue todo
  case mjob of
    Just (Job k i j c z dz n) -> do
      let done' z' dz' n' = do
            writeChan done $ Job k i j c z' dz' n'
            let (r, g, b) = colour z' dz' n'
            output i j r g b
          todo' z' dz' n' = enqueue todo $ Job k i j c z' dz' n'
      calculate c limit z dz n done' todo'
    Nothing -> yield

calculate :: C -> N -> C -> C -> N -> (C -> C -> N -> IO ()) -> (C -> C -> N -> IO ()) -> IO ()
calculate !c !m0 !z0 !dz0 !n0 done todo = go m0 z0 dz0 n0
  where
    go !m !z !dz !n
      | not (magnitude2 z < escapeR2) = done z dz n
      | m <= 0                        = todo z dz n
      | otherwise = go (m - 1) (z * z + c) (2 * z * dz + 1) (n + 1)

magnitude2 :: C -> R
magnitude2 (r :+ i) = r * r + i * i

main :: IO ()
main = do
  _ <- unsafeInitGUIForThreadedRTS
  _ <- initGL
  glconfig <- glConfigNew [ GLModeRGBA, GLModeDouble ]
  canvas <- glDrawingAreaNew glconfig
  widgetSetSizeRequest canvas width height
  imgdata <- mallocBytes $ width * height * 3
  let output x y r g b = do
        let p = imgdata `plusPtr` ((y * width + x) * 3)
        pokeByteOff p 0 r
        pokeByteOff p 1 g
        pokeByteOff p 2 b
  window <- windowNew
  eventb <- eventBoxNew
  set window [ containerBorderWidth := 0, containerChild := eventb ]
  set eventb [ containerBorderWidth := 0, containerChild := canvas ]
  stop0 <- renderer output c0 zoom0
  sR <- newIORef (c0, zoom0, stop0)
  _ <- eventb `on` buttonPressEvent $ {-# SCC "cb/event" #-} tryEvent $ do
    LeftButton <- eventButton
    (x, y) <- eventCoordinates
    liftIO $ do
      (c, zoom, stop) <- readIORef sR
      stop
      pokeArray imgdata (replicate (height * width * 3) (255 :: B))
      let c' = c + ((x :+ (-y)) - (fromIntegral width :+ fromIntegral (-height))/2) / 2^^zoom
          zoom' = zoom + 1
      stop' <- renderer output c' zoom'
      writeIORef sR (c', zoom', stop')
  _ <- onRealize canvas $ {-# SCC "cb/realize" #-}withGLDrawingArea canvas $ \_ -> do
    GL.clearColor $= (GL.Color4 0.0 0.0 0.0 0.0)
    GL.matrixMode $= GL.Projection
    GL.loadIdentity
    GL.ortho 0.0 1.0 0.0 1.0 (-1.0) 1.0
    GL.drawBuffer $= GL.BackBuffers
    [tex] <- GL.genObjectNames 1
    GL.texture GL.Texture2D $= GL.Enabled
    GL.textureBinding GL.Texture2D $= Just tex
    GL.texImage2D Nothing GL.NoProxy 0 GL.RGB' (GL.TextureSize2D (fromIntegral width) (fromIntegral height)) 0 (GL.PixelData GL.RGB GL.UnsignedByte nullPtr)
    GL.textureFilter GL.Texture2D $= ((GL.Nearest, Nothing), GL.Nearest)
    GL.textureWrapMode GL.Texture2D GL.S $= (GL.Repeated, GL.ClampToEdge)
    GL.textureWrapMode GL.Texture2D GL.T $= (GL.Repeated, GL.ClampToEdge)
  _ <- onExpose canvas $ {-# SCC "cb/expose" #-} \_ -> do
    withGLDrawingArea canvas $ \glwindow -> do
      let v :: GLfloat -> GLfloat -> GLfloat -> GLfloat -> IO ()
          v tx ty vx vy = GL.texCoord (GL.TexCoord2 tx ty) >> GL.vertex (GL.Vertex2 vx vy)
          w = fromIntegral width
          h = fromIntegral height
      GL.clear [ GL.ColorBuffer ]
      GL.texSubImage2D Nothing 0 (GL.TexturePosition2D 0 0) (GL.TextureSize2D w h) (GL.PixelData GL.RGB GL.UnsignedByte imgdata)
      GL.renderPrimitive GL.Quads $ do
        v 0 1 0 0 >> v 0 0 0 1 >> v 1 0 1 1 >> v 1 1 1 0
      glDrawableSwapBuffers glwindow
    return True
  _ <- onDestroy window mainQuit
  _ <- timeoutAdd (widgetQueueDraw canvas >> return True) 100
  widgetShowAll window
  mainGUI

spread :: [ N ]
spread = [ -1, 0, 1 ]

limit :: N
limit = (2^(12::N)-1)

width, height :: N
width = 512
height = 512

c0 :: C
c0 = 0 :+ 0

zoom0 :: N
zoom0 = 8

escapeR, escapeR2 :: R
escapeR = 65536
escapeR2 = escapeR * escapeR

