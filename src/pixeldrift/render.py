import cairo
from math import sqrt, ceil

def _get_img_and_mask(cells, padding):
    x, y, w, h = cells.get_bbox()
    x -= padding
    y -= padding
    w += 2*padding
    h += 2*padding
    img = cells.get_data(x, y, w, h)
    mask = cells.get_mask(x, y, w, h)
    return img, mask

def render_cells(cells, filename, padding=8):
    img, mask = _get_img_and_mask(cells, padding)

    # naming conventions (pointy variant):
    # https://www.redblobgames.com/grids/hexagons/#size-and-spacing
    size = 4  # pixels
    w = sqrt(3)
    h = 2

    W = size * w * (img.shape[0] + 1/2)
    H = size * h * (img.shape[0] * 3/4 + 1/4)
    surf = cairo.ImageSurface(cairo.FORMAT_RGB24, int(ceil(W)), int(ceil(H)))

    cr = cairo.Context(surf)

    # background
    cr.rectangle(0, 0, int(ceil(W)), int(ceil(H)))
    cr.set_source_rgba(0.6, 0.6, 0.6, 1.0)
    cr.fill()

    cr.scale(size, size)
    cr.translate(w/2, h/2)  # center of upper left hex

    # cr.set_source_rgba(0.0, 1.0, 1.0, 0.7)
    cr.set_source_rgba(1.0, 1.0, 1.0, 1.0)

    for y in range(img.shape[0]):
        for x in range(img.shape[1]):
            alpha = 1.0 if mask[y, x] else 0.4
            if img[y, x] != 0:
                cr.set_source_rgba(1.0, 1.0, 1.0, alpha)
            else:
                cr.set_source_rgba(0.2, 0.2, 0.2, alpha)
            cr.save()
            cr.translate((x+(0.5 if y%2 == 1 else 0))*w, y*3/4*h)  # to center of hex
            cr.move_to(0, -1/2*h)
            cr.line_to(+1/2*w, -1/4*h)
            cr.line_to(+1/2*w, +1/4*h)
            cr.line_to(0, +1/2*h)
            cr.line_to(-1/2*w, +1/4*h)
            cr.line_to(-1/2*w, -1/4*h)
            cr.fill()
            cr.restore()

    surf.write_to_png(filename)
