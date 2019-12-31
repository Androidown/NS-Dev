#!/usr/bin/env python
# -*- coding: UTF-8 -*-


class BitMask(object):
    def __init__(self, mask, bin_map={}):
        self.mask = mask
        self.bin_map = bin_map
        self.added = False

    def add(self, val):
        if isinstance(val, str):
            val = self.bin_map.get(val)
        if not isinstance(val, int):
            raise ValueError(f"Unsupported input type: {type(val)}")
        elif self.added:
            self.mask |= val
        else:
            self.mask = val
            self.added = True
        return self

    def __repr__(self):
        return bin(self.mask)
