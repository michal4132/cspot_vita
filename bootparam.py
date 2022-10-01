#!/usr/bin/env python
import sys
import struct

# TODO: Replace sys.argv with argparse

def make_bootparam(path, memsize, attribute=False):
    data = bytearray(b"\x00" * 256)

    data[0] = 1

    # attribute
    if(attribute):
        pos = 4
        for b in struct.pack('<I', attribute):
            data[pos] = b
            pos += 1

    # app_memsize
    pos = 12
    for b in struct.pack('<I', memsize):
        data[pos] = b
        pos += 1


    f = open(path, "wb")
    f.write(data)
    f.close()

file_path = sys.argv[-1]
memsize = int(sys.argv[2], 0)

attribute = False
if(len(sys.argv) >= 5):
    attribute = int(sys.argv[4], 0)

if(memsize > 0xFFFFFFFF):
    print("Invalid value")
    sys.exit(1)

if(sys.argv[3] == "attribute"):
    make_bootparam(file_path, memsize, attribute)
elif(sys.argv[1] == "app_memsize"):
    make_bootparam(file_path, memsize)
