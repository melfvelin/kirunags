"""
Embedded Python Blocks:

Each time this file is saved, GRC will instantiate the first class it finds
to get ports and parameters of your block. The arguments to __init__  will
be the parameters. All of them are required to have default values!
"""

#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright 2021 Luleå Tekniska Universitet - Gabriel Sanchez <gabsan-0@student.ltu.se>
#
# This file is part of APTAS_GS software package
#
# SPDX-License-Identifier: GPL-3.0-or-later
#
##############################################################################################
# APTAS GS Software receives telemetry, translates it to readable data and:
# 1. Uploads data to a MySQL database
# 2. Transmits the data through UDP connection to subscribed clients
# 3. Saves a local copy
# Takes the TCs and prepares for transmission
##############################################################################################
# This file contains the data stream handeling top level funtions
##############################################################################################

import numpy as np
import pmt
from gnuradio import gr, blocks, digital

class debugger(gr.basic_block):
    #runs only once when the instance of the block is crated
    def __init__(self, debug_message="MSG Debugger Data"):
        gr.basic_block.__init__(self, name='MSG Debugger',
        in_sig=None,
        out_sig=None)
        
        self.message_port_register_in(pmt.intern("IN"))
        self.set_msg_handler(pmt.intern("IN"), self.asm_golay_this)
        self._debug_message=str(debug_message)
        
    def asm_golay_this(self,msg):
        meta = pmt.car(msg)
        data = pmt.to_python(pmt.cdr(msg))
        bin_data=[]
        for decimal_value in data:
            bits = [int(x) for x in '{:08b}'.format(decimal_value)]
            bin_data = bin_data + bits
        print("\n"+self._debug_message+":"+str(bin_data))
        print("\n")


    def general_work(self, input_items, output_items):
        pass
