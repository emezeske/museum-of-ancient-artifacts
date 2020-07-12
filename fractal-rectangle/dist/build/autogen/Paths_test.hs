module Paths_test (
    version,
    getBinDir, getLibDir, getDataDir, getLibexecDir,
    getDataFileName
  ) where

import Data.Version (Version(..))
import System.Environment (getEnv)

version :: Version
version = Version {versionBranch = [0,0], versionTags = []}

bindir, libdir, datadir, libexecdir :: FilePath

bindir     = "/home/emezeske/.cabal/bin"
libdir     = "/home/emezeske/.cabal/lib/test-0.0/ghc-6.12.1"
datadir    = "/home/emezeske/.cabal/share/test-0.0"
libexecdir = "/home/emezeske/.cabal/libexec"

getBinDir, getLibDir, getDataDir, getLibexecDir :: IO FilePath
getBinDir = catch (getEnv "test_bindir") (\_ -> return bindir)
getLibDir = catch (getEnv "test_libdir") (\_ -> return libdir)
getDataDir = catch (getEnv "test_datadir") (\_ -> return datadir)
getLibexecDir = catch (getEnv "test_libexecdir") (\_ -> return libexecdir)

getDataFileName :: FilePath -> IO FilePath
getDataFileName name = do
  dir <- getDataDir
  return (dir ++ "/" ++ name)
