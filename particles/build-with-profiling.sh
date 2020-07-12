#!/bin/bash

ghc -O2 --make Test.hs -prof -auto-all && ./Test +RTS -p
