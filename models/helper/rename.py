import os

x = 578
for filename in os.listdir(os.path.dirname(os.path.abspath(__file__))):
    ext = filename.split('.')
    extension = ext[1]
    os.rename(filename,'0' + str(x) + '.' + extension)
    x = x + 1

