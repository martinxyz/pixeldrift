import cairo
from math import sqrt, ceil

def _get_data_and_mask(cells, x0, y0, w0, h0, padding):
    x, y, w, h = cells.get_bbox()
    if x0 is not None: x = x0
    if y0 is not None: y = y0
    if w0 is not None: w = w0
    if h0 is not None: h = h0
    x -= padding
    y -= padding
    w += 2*padding
    h += 2*padding
    data = {
        'particles': cells.get_data(x, y, w, h, channel='particles'),
        'cell_types': cells.get_data(x, y, w, h, channel='cell_types'),
    }
    mask = cells.get_mask(x, y, w, h)
    assert data['particles'].shape == mask.shape
    assert data['cell_types'].shape == mask.shape
    odd = 1 if y % 2 else 0
    return data, mask, odd

def render_cells(cells, filename, x=None, y=None, w=None, h=None,
                 padding=3, scale=5, show_partial=True, grid=True, fog=True):

    # We need one more padding to also render cells that are outside.
    hidden_padding = 1 if show_partial else 0
    data, mask, odd = _get_data_and_mask(cells, x, y, w, h, padding+hidden_padding)

    # naming conventions (pointy variant):
    # https://www.redblobgames.com/grids/hexagons/#size-and-spacing
    size = scale  # pixels
    w = sqrt(3)
    h = 2

    W = size * w * ((mask.shape[1] - 2*hidden_padding) + 1/2)
    H = size * h * ((mask.shape[0] - 2*hidden_padding) * 3/4 + 1/4)
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

    type2color = {
        1: (0.9, 0.3, 0.3),
        2: (0.3, 0.9, 0.3),
        3: (0.3, 0.3, 0.9),
        4: (0.1, 0.8, 0.8),
        5: (0.8, 0.1, 0.8),
        6: (0.8, 0.8, 0.1),
    }

    for y in range(mask.shape[0]):
        for x in range(mask.shape[1]):
            alpha = 1.0 if (not fog or mask[y, x]) else 0.4
            particle = data['particles'][y, x]
            cell_type = data['cell_types'][y, x]
            if particle != 0:
                color = (0.8, 0.8, 0.8)
            else:
                color = (0.2, 0.2, 0.2)
            color = type2color.get(cell_type, color)

            cr.set_source_rgba(color[0]*alpha + fog*(1-alpha),
                               color[1]*alpha + fog*(1-alpha),
                               color[2]*alpha + fog*(1-alpha),
                               1.0)
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
