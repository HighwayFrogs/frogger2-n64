for i in range(0, 52):
  addr = toAddr(0x801a0cc4 + (4 * i))
  println("2 Debug: " + str(toAddr(getInt(addr))) + ", " + ("_collBank%03dSegmentRomStart" % (i)))
  createLabel(toAddr(getInt(addr)), ("_collBank%03dSegmentRomStart" % (i)), True)
  
  
for i in range(0, 52):
  addr = toAddr(0x801a0e64 + (4 * i))
  println("2 Debug: " + str(toAddr(getInt(addr))) + ", " + ("_scenBank%03dSegmentRomStart" % (i)))
  createLabel(toAddr(getInt(addr)), ("_scenBank%03dSegmentRomStart" % (i)), True)
  

from java.io import File
from java.nio.file import Files
# TODO: There's data before this. Seems to start at 0xb0000000
symbol = getSymbolAt(toAddr(0xb00ab740))
while symbol != None:
  nextSymbol = getSymbolAfter(symbol)
  if nextSymbol is None:
    println("Couldn't export " + symbol.getName());
    break
  
  data = getBytes(symbol.getAddress(), nextSymbol.getAddress().getOffset() - symbol.getAddress().getOffset());
  Files.write(File(symbol.getName()).toPath(), data, [])
  symbol = nextSymbol