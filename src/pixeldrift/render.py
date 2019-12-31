import cairo
from math import sqrt, ceil

def _get_img_and_mask(cells, x0, y0, w0, h0, padding):
    x, y, w, h = cells.get_bbox()
    if x0 is not None: x = x0
    if y0 is not None: y = y0
    if w0 is not None: w = w0
    if h0 is not None: h = h0
    x -= padding
    y -= padding
    w += 2*padding
    h += 2*padding
    img = cells.get_data(x, y, w, h)
    mask = cells.get_mask(x, y, w, h)
    odd = 1 if y % 2 else 0
    return img, mask, odd

def render_cells(cells, filename, x=None, y=None, w=None, h=None,
                 padding=3, scale=5, show_partial=True, grid=True, fog=True):

    # We need one more padding to also render cells that are outside.
    hidden_padding = 1 if show_partial else 0
    img, mask, odd = _get_img_and_mask(cells, x, y, w, h, padding+hidden_padding)

    # naming conventions (pointy variant):
    # https://www.redblobgames.com/grids/hexagons/#size-and-spacing
    size = scale  # pixels
    w = sqrt(3)
    h = 2

    W = size * w * ((img.shape[1] - 2*hidden_padding) + 1/2)
    H = size * h * ((img.shape[0] - 2*hidden_padding) * 3/4 + 1/4)
    surf = cairo.ImageSurface(cairo.FORMAT_RGB24, int(ceil(W)), int(ceil(H)))

    cr = cairo.Context(surf)

    # background
    fog = 0.4
    cr.rectangle(0, 0, int(ceil(W)), int(ceil(H)))
    cr.set_source_rgba(*[fog]*3, 1.0)
    cr.fill()

    cr.scale(size, size)
    cr.translate(-hidden_padding*w, -hidden_padding*h*3/4)
    cr.translate(w/2, h/2)

    for y in range(img.shape[0]):
        for x in range(img.shape[1]):
            alpha = 1.0 if (not fog or mask[y, x]) else 0.4
            if img[y, x] != 0:
                cr.set_source_rgba(*[0.8*alpha + fog*(1-alpha)]*3, 1.0)
            else:
                cr.set_source_rgba(*[0.2*alpha + fog*(1-alpha)]*3, 1.0)
            cr.save()
            # move to center of hex
            cr.translate((x+(0.5 if (y+odd)%2 else 0))*w, y*3/4*h)

            fac = (1 - 0.4/size) if grid else (1 + 0.4/size)
            cr.scale(fac, fac)

            cr.move_to(0, -1/2*h)
            cr.line_to(+1/2*w, -1/4*h)
            cr.line_to(+1/2*w, +1/4*h)
            cr.line_to(0, +1/2*h)
            cr.line_to(-1/2*w, +1/4*h)
            cr.line_to(-1/2*w, -1/4*h)
            # if grid:
            #     cr.fill_preserve()
            #     cr.set_line_width(0.05*w)
            #     cr.set_source_rgba(0.4, 0.4, 0.4, alpha)
            #     cr.stroke()
            # else:
            cr.fill()
            cr.restore()

    surf.write_to_png(filename)
