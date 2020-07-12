module Test where

fibonacci :: (Num a) => a -> a
fibonacci 0 = 0
fibonacci 1 = 1
fibonacci x = fibonacci (x-1) + fibonacci (x-2)

data Message = Hello String | Goodbye String deriving(Show)

send :: Message -> String
send (Hello x)   = x ++ " says hello"
send (Goodbye x) = x ++ " says goodbye"

sendGoodbye :: String -> String
sendGoodbye = send . Goodbye
