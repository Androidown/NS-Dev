#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from functools import partial
from const import stats_map, natures
from base import BitMask


class PokeMonTemplate(object):
    __slots__ = "IVs_min", "IVs_max", "shiny_type", "nature", "ability", "gender"

    def __init__(self):
        self.IVs_min = [0, 0, 0, 0, 0, 0]
        self.IVs_max = [31, 31, 31, 31, 31, 31]
        self.shiny_type = BitMask(0b111, bin_map={"null":1, "star": 2, "square": 4})
        self.nature = BitMask(2**25 - 1, bin_map=dict(zip(natures, map(lambda x: 2**x, range(25)))))
        self.ability = BitMask(0b111, bin_map={"H": 4})
        self.gender = BitMask(0b111, bin_map={"male": 1, "female": 2, "genderless": 4})

    def set_iv(self, min_max, key, value):
        value = min(31, max(0, value))
        modify_attr = getattr(self, "IVs_" + min_max.lower())
        key = key.lower()
        if key == "all":
            for i in range(6):
                modify_attr[i] = value
        else:
            modify_attr[stats_map[key]] = value

    def numerize(self):
        for key in self.__slots__:
            attr = getattr(self, key)
            if isinstance(attr, BitMask):
                setattr(self, key, attr.mask)
        return self

    def __getattr__(self, item):
        if item.startswith("set_iv_"):
            min_max, key = item[7:].split('_')
            return partial(self.set_iv, min_max, key)

    def __repr__(self):
        return f"IVs_min: {self.IVs_min}\n" \
            f"IVs_max: {self.IVs_max}\n" \
            f"shiny_type: {self.shiny_type}\n" \
            f"nature: {self.nature}\n" \
            f"ability: {self.ability}\n" \
            f"gender: {self.gender}\n" \

if __name__ == '__main__':
    pmtpl = PokeMonTemplate()
    pmtpl.set_iv_min_all(10)
    # print(pmtpl.IVs_min)
    # print(pmtpl.IVs_max)
    pmtpl.shiny_type.add("square")
    pmtpl.nature.add('Adamant').add('Naughty').add('Bold')
    # pmtpl.numerize()
    print(pmtpl)
