import os.path
import os
import sys
repository_root = os.path.split(os.path.split(os.path.abspath(__file__))[0])[0]
sys.path = [repository_root] + sys.path

from .c_api import make_c_api
from .node_reference import make_node_reference
