#!/usr/bin/python
#-*- coding: utf-8 -*-

import Image
import sys
import itertools
import colorsys


############## Palette definitions  ###################

def rainbow(startColor, endColor, nSteps):
    """ Uses the HSV colorspace to generate a rainbow of nSteps
        different colors, starting at color startColor and ending
        at maxColor. """

    assert(startColor >= 0 and startColor < 360)
    assert(endColor >= 0 and endColor <= 360)
    assert(startColor < endColor)
    step = float(endColor - startColor) / nSteps

    colors = [colorsys.hsv_to_rgb(startColor + n*step, 1.0, 1.0) for n in range(nSteps)]
    # Must convert RGB values to int
    colors = [(int(R*255), int(G*255), int(B*255)) for (R, G, B) in colors]

    return colors


def monochrome(color, sat, startLight, endLight, nSteps):

    colors = [(0, 0, 0) for n in range(nSteps)]
    step = (startLight - endLight) / nSteps
    for i in range(nSteps-1):
        colors[i] = colorsys.hls_to_rgb(color, i * step, sat)

    # Black as the color for the inside of the mandelbrot set
    colors[nSteps-1] = (0, 0, 0)
    # Must convert RGB values to int
    colors = [(int(R*255), int(G*255), int(B*255)) for (R, G, B) in colors]
    return colors





########### Actual transformation of the image ###############


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
            color = image[y][x] # we consider x on the horizontal direction
            out_image.putpixel((x, y), palette[color])

    return out_image




if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: %s inFile outFile" % sys.argv[0])
        exit(1)

    in_file_name = sys.argv[1]
    out_file_name = sys.argv[2]

    width, height, image = read_file(in_file_name)
    all_pixels = itertools.chain(*image)
    palette_size = max(all_pixels) + 1 # starts from 0 !
    palette = monochrome(1.0, 1.0, 1.0, 0.2, palette_size)

    rgb_image = to_rgb(width, height, image, palette)
    rgb_image.save(out_file_name)
