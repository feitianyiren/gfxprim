"core.Pixmap tests"

from unittest import SkipTest
from testutils import *

from gfxprim.core import Pixmap
from gfxprim import gfx, core


def test_gfx_submodule_loads():
  "gfx is present in a Pixmap"
  c = Pixmap(1, 1, core.C.PIXEL_RGB888)
  assert c.gfx


def test_gfx_submodule_has_C():
  "gfx contains C"
  c = Pixmap(1, 1, core.C.PIXEL_RGB888)
  assert c.gfx.C
  assert gfx.C


# These set the param types of the functions in GFX
gfx_params = {
    'arc_segment': 'IIIIIFFP',
    'circle': 'IIIP',
    'ellipse': 'IIIIP',
    'fill': 'P',
    'fill_circle': 'IIIP',
    'fill_ellipse': 'IIIIP',
    'fill_polygon': ([(0,0),(1,1),(1,0)], 0, {}),
    'fill_rect': 'IIIIP',
    'fill_ring': 'IIIIP',
    'fill_tetragon': 'IIIIIIIIP',
    'fill_triangle': 'IIIIIIP',
    'hline': 'IIIP',
    'hline_aa': 'IIIP', # Fixpoint, originally 'FFFP'
    'line': 'IIIIP',
    'line_aa': 'IIIIP', # Fixpoint, originally 'FFFFP'
    'polygon': ([(0,0),(1,1),(1,0)], 0, {}),
    'putpixel_aa': 'IIP', # Fixpoint, originally 'FFP'
    'rect': 'IIIIP',
    'ring': 'IIIIP',
    'tetragon': 'IIIIIIIIP',
    'triangle': 'IIIIIIP',
    'vline': 'IIIP',
    'vline_aa': 'IIIP', # Fixpoint, originally 'FFFP'
    }


def test_all_methods_are_known():
  "All methods of gfx submodule have known param types in this test"
  c = Pixmap(1, 1, core.C.PIXEL_RGB888)
  for name in dir(c.gfx):
    if name[0] != '_' and name not in ['C', 'ctx']:
      assert name in gfx_params

def gen_dummy_args(params):
  """
  Generate dummy parameter tuple  according to characters in the given string.

  0 - 0
  S - String ("")
  I - Int (1)
  F - Float (0.5)
  P - Pixel (0)
  """
  args = []
  for t in params:
    if t == '0':
      args.append(0)
    elif t == 'I':
      args.append(1)
    elif t == 'P':
      args.append(0)
    elif t == 'F':
      args.append(0.5)
    elif t == 'S':
      args.append("")
    else:
      assert False
  return tuple(args)

@for_each_case(gfx_params)
def test_method_call(n, params):
  "Calling with dummy parameters:"
  c = PixmapRand(10, 10, core.C.PIXEL_RGB888)
  if isinstance(params, str):
    c.gfx.__getattribute__(n)(*gen_dummy_args(params))
  else:
    assert isinstance(params, tuple) and isinstance(params[-1], dict)
    c.gfx.__getattribute__(n)(*params[:-1], **params[-1])

def test_Polygon():
  "Polygon() works"
  c0 = PixmapRand(13, 12, core.C.PIXEL_RGB888, seed=42)
  c1 = PixmapRand(13, 12, core.C.PIXEL_RGB888, seed=42)
  c2 = PixmapRand(13, 12, core.C.PIXEL_RGB888, seed=42)
  assert c1 == c0
  c1.gfx.polygon([1,2,0,4,7,9,5,4,3,2], 43)
  c2.gfx.polygon([(1,2),(0,4),(7,9),(5,4),(3,2)], 43)
  assert c1 == c2
  assert c1 != c0

def test_FillPolygon():
  "FillPolygon() works"
  c0 = PixmapRand(13, 9, core.C.PIXEL_RGB888, seed=41)
  c1 = PixmapRand(13, 9, core.C.PIXEL_RGB888, seed=41)
  c2 = PixmapRand(13, 9, core.C.PIXEL_RGB888, seed=41)
  assert c1 == c0
  c1.gfx.fill_polygon([1,2,0,4,7,9,5,4,3,2], 0)
  c2.gfx.fill_polygon([(1,2),(0,4),(7,9),(5,4),(3,2)], 0)
  assert c1 == c2
  assert c1 != c0
