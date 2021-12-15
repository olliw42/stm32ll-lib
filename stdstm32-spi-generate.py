import re
import datetime


templatefname = "stdstm32-spi-template.h"
    
F = open(templatefname, "r")
templatecode = F.read()
F.close()

def generate(code, periphname):
    outfname = "src\stdstm32-"+periphname+".h"
    print('generate', outfname)
    code1 = code.replace('SPI$',periphname.upper())
    code2 = code1.replace('spi$',periphname)
    code3 = code2.replace('CONV$','0')
    F = open(outfname, "w")
    F.write(code3)
    F.close() 

generate(templatecode, 'spi')
generate(templatecode, 'spib')
generate(templatecode, 'spic')
