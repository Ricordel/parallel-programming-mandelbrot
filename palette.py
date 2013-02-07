# Color palettes to map the mandelbrot results

import colorsys


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
