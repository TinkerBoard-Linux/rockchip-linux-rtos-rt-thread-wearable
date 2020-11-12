Import('RTT_ROOT')
Import('rtconfig')
from building import *

cwd = GetCurrentDir()
src = Glob('*.c')
src += Glob('resource/*.c')
src += Glob('func/*.c')
CPPPATH = [cwd] + [cwd + '/func']

group = DefineGroup('Applications', src, depend = ['RT_USING_APP_WEARABLE'], CPPPATH = CPPPATH)

Return('group')
