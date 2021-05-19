"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

import numpy as np
from gnuradio import gr


class python_debugger(gr.basic_block):  # other base classes are basic_block, decim_block, interp_block
    """Embedded Python Block example - a simple multiply const"""

    def __init__(self):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
        gr.basic_block.__init__(
            self,
            name='Python Bit Debugger',   # will show up in GRC
            in_sig=[(np.int8)],
            out_sig=None
        )

    def general_work(self, input_items, output_items):
        """example: multiply with constant"""
        in0=input_items[0]
        print("\nByte debug:\n"+str(in0))
        print("Length : " + str(len(in0)))
        self.consume(0, len(in0))
        return len(in0)