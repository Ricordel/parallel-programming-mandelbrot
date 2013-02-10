#!/usr/bin/python
#-*- coding: utf-8 -*-

import Image
import sys
import itertools
import colorsys


############## Palette definitions  ###################


def hsv_palette(nSteps, (startHue, startSat, startVal), (endHue, endSat, endVal), reverse=False):
    """ Generate a palette stepping through the HSV colorspace. It allows
        for easy rainbow palettes, or monochrome, ...
        All given values must be in the range 0..1. """
    
    hueStep = (endHue - startHue) / nSteps
    satStep = (endSat - startSat) / nSteps
    valStep = (endVal - startVal) / nSteps

    colors = [ colorsys.hsv_to_rgb(startHue + n * hueStep,
                                   startSat + n * satStep,
                                   startVal + n * valStep) for n in range(nSteps) ]

    # Must convert RGB values to int in 0..255 for the Image module
    colors = [(int(R*255), int(G*255), int(B*255)) for (R, G, B) in colors]
    if reverse:
        colors.reverse()
    # Black as the color for the inside of the mandelbrot set
    colors[nSteps-1] = (0, 0, 0)

    return colors


def rgb_palette(nSteps, (startR, startG, startB), (endR, endG, endB)):
    """ Generate a palette stepping through the RGB colorspace. Allows for
        easy gradients between two colors.
        RGB coordinates taken in argument must be in the range 0..255. """
    def normalize(c):
        return float(c) / 255

    startR = normalize(startR)
    startG = normalize(startG)
    startB = normalize(startB)
    endR = normalize(endR)
    endG = normalize(endG)
    endB = normalize(endB)

    stepR = (endR - startR) / nSteps
    stepG = (endG - startG) / nSteps
    stepB = (endB - startB) / nSteps

    colors = [ (startR + n * stepR, startG + n * stepG, startB + n * stepB)
               for n in range(nSteps) ]

    # Black as the color for the inside of the mandelbrot set
    colors[nSteps-1] = (0, 0, 0)
    # Must convert RGB values to int
    colors = [(int(R*255), int(G*255), int(B*255)) for (R, G, B) in colors]
    return colors






########### Actual transformation of the image ###############


def read_file(filename):
    """ Read a file output by the mandelbrot program and return the width and
        height of the image, and the color indexes. """
    with open(filename) as file:
        lines = file.readlines()
        wh = lines[0].split(" ")
        width, height = int(wh[0]), int(wh[1])
        image = lines[1:]
        # Go only to -1 to remove the trailing \n
        image = [l.strip().split(" ") for l in image]
        image = [[ int(e) for e in l] for l in image]

        return width, height, image



def to_rgb(width, height, image, palette):
    """ Takes an image as read by read_file and returns a PNG image
        (as an image structure for the Image module). """
    out_image = Image.new("RGB", (width, height))
    for x in range(width):
        for y in range(height):
            color = image[y][x] 
            out_image.putpixel((x, y), palette[color])

    return out_image





if __name__ == "__main__":
    if len(sys.argv) != 4:
        print("Usage: %s inFile outFile paletteNumber" % sys.argv[0])
        exit(1)

    in_file_name = sys.argv[1]
    out_file_name = sys.argv[2]

    width, height, image = read_file(in_file_name)
    all_pixels = itertools.chain(*image)
    palette_size = max(all_pixels) + 1 # starts from 0 !

    ### Some nice palettes:
    palettes = [ hsv_palette(palette_size, (0.5, 0.0, 1.0), (0.8, 1.0, 1.0), reverse=True)
               , rgb_palette(palette_size, (0, 181, 255), (255, 0, 82))
               , hsv_palette(palette_size, (15./360, 1.0, 1.0), (15./360, 0, 0.7))
               , hsv_palette(palette_size, (0, 1.0, 0.0), (0, 1.0, 1.0))
               , hsv_palette(palette_size, (0, 1.0, 0.0), (0, 0.8, 1.0))
               , hsv_palette(palette_size, (0.5, 1.0, 1.0), (1.0, 1.0, 1.0))
               , hsv_palette(palette_size, (65./360, 0.0, 0.5), (0.0, 1.0, 1.0), reverse=True) ]

    try:
        palette_number = int(sys.argv[3])
    except ValueError:
        print("The palette must be an int between 0 and %d" % len(palettes - 1))
        exit(1)

    if palette_number >= len(palettes):
        print("The palette must be an int between 0 and %d" % len(palettes - 1))
        exit(1)


    rgb_image = to_rgb(width, height, image, palettes[palette_number])
    rgb_image.save(out_file_name)
