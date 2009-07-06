import Text.ParserCombinators.Parsec
import System.Environment
import Control.Concurrent
import Control.Monad
import System.FilePath
import System.IO
import qualified Data.Set as S
import qualified Data.Map as M
import Data.Map((!))
import Data.Binary
import qualified Data.ByteString.Lazy as B
import Data.Word
import Control.Concurrent.MVar

main = do
  files <- getArgs
  waiter <- newEmptyMVar
  mapM (\f -> forkIO $ compress f >> putMVar waiter ()) files
  replicateM (length files) (takeMVar waiter)
  where
    compress file = do
      parsed <- parseFromFile parser file
      case parsed of
        Left err -> print err
        Right tiles -> do
          outfile <- openFile (replaceExtension file ".dmp") WriteMode
          let flatTiles = concat tiles
              munge = M.fromList . flip zip [(1::Word32)..] . S.toList . S.fromList
              bytes = log256 (maximum (map M.size [kinds,values]))
              kinds = munge $ map fst flatTiles
              values = munge $ map snd flatTiles
              code num = case bytes of
                1 -> encode (fromIntegral num :: Word8)
                2 -> encode (fromIntegral num :: Word16)
                _ -> encode num
          mapM_ (hPutStrLn outfile . show) [kinds,values]
          forM_ tiles $ \tile -> do
            forM_ tile $ \(kind,value) -> do
              B.hPut outfile $ code $ kinds ! kind
              B.hPut outfile $ code $ kinds ! kind
            B.hPut outfile $ code (0::Word32) -- A single 0 word terminates a tile. Note that the kinds and values are 1-indexed.
          hClose outfile
          


type Tile = [(String,String)]

parser :: Parser [Tile]
parser = many $ (try hidden <|> element) `manyTill` (string "---" >> newline)
  where element = do  
          kind <- anyChar `manyTill` string ": "
          value <- anyChar `manyTill` newline
          return (kind,value)
        hidden = string "Hidden" >> newline >> return ("Hidden","")

log256 0 = 0
log256 n = 1 + log256 (n `div` 256)
