#!/usr/bin/python
#-*- coding: utf-8 -*-

import Image
import sys
import itertools
from palette import *


def read_file(filename):
    lines = open(filename).readlines()
    wh = lines[0].split(" ")
    width, height = int(wh[0]), int(wh[1])
    image = lines[1:]
    # Go only to -1 to remove the trailing \n
    image = [l.strip().split(" ") for l in image]
    image = [[ int(e) for e in l] for l in image]
    return width, height, image


def to_rgb(width, height, image, palette):
    out_image = Image.new("RGB", (width, height))
    for x in range(width):
        for y in range(height):
            out_image.putpixel((x, y), palette[image[x][y]])

    return out_image




if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: %s inFile outFile nColors" % sys.argv[0])
        exit(1)

    in_file_name = sys.argv[1]
    out_file_name = sys.argv[2]
    nColors = int(sys.argv[3])

    width, height, image = read_file(in_file_name)
    all_pixels = itertools.chain(*image)
    palette_size = max(all_pixels) + 1 # starts from 0 !

    rgb_image = to_rgb(width, height, image, rainbow(0, 360, palette_size))
    rgb_image.save(out_file_name)
