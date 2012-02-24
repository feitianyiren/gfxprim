
# HACK to allow loaders_c to find core_c
def import_loaders_c_helper():
  from os.path import dirname
  import sys
  oldpath = sys.path[:]
  sys.path.append(dirname(__file__) + '/../core')
  from . import loaders_c
  sys.path = oldpath
  return loaders_c
loaders_c = import_loaders_c_helper()
del import_loaders_c_helper

# Extend Context with convenience methods
from . import extend_context
from ..core import Context
extend_context.extend_context_class(Context)
del Context
del extend_context

# Import some members from the SWIG module
def import_helper(module):
  from ..utils import import_members

  # Functions
  import_members(loaders_c, module,
    exclude=[
      '^\w+_swigregister$',
      '^core_c$',
      '^_\w+$'])
import_helper(locals())
del import_helper
