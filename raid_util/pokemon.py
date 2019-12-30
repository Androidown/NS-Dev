#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from functools import partial

stat_map = {
    "hp": 0,
    "atk": 1,
    "def": 2,
    "spa": 3,
    "spd": 4,
    "spe": 5
}


class PokeMon(object):
    def __init__(self):
        self.IVs = [-1] * 6
        self.shiny_type = set()
        self.nature = set()
        self.ability = set()
        self.gender = set()
        self.ec = 0
        self.pid = 0

    def __repr__(self):
        return f"IVs: {self.IVs}\n" \
            f"shiny_type: {self.shiny_type}\n" \
            f"nature: {self.nature}\n" \
            f"ability: {self.ability}\n" \
            f"gender: {self.gender}\n" \
            f"ec: {self.ec}, pid: {self.pid}\n"


class PokeMonTemplate(object):
    def __init__(self):
        self.IVs_min = [0, 0, 0, 0, 0, 0]
        self.IVs_max = [31, 31, 31, 31, 31, 31]
        self.shiny_type = set()
        self.nature = set()
        self.ability = set()
        self.gender = set()

    def set_iv(self, min_max, key, value):
        value = min(31, max(0, value))
        if min_max.lower() == 'min':
            self.IVs_min[stat_map[key.lower()]] = value
        if min_max.lower() == 'max':
            self.IVs_max[stat_map[key.lower()]] = value

    def __contains__(self, item: PokeMon):
        if (self.shiny_type and item.shiny_type not in self.shiny_type) \
            or (self.nature and item.nature not in self.nature) \
                or (self.gender and item.gender not in self.gender) \
                or (self.ability and item.ability not in self.ability):
            return False
        for iv_min, iv, iv_max in zip(self.IVs_min, item.IVs, self.IVs_max):
            if not iv_min <= iv <= iv_max:
                return False
        return True

    def __getattr__(self, item):
        if item.startswith("set_iv_"):
            min_max, key = item[7:].split('_')
            return partial(self.set_iv, min_max, key)


if __name__ == '__main__':
    pmtpl = PokeMonTemplate()
    pmtpl.set_iv_min_atk(10)
    # pmtpl.IVs_min[attr_map['atk'.lower()]] = 10
    print(pmtpl.IVs_min)
    print(pmtpl.IVs_max)
    pm = PokeMon()
    pmtpl.shiny_type.add("square")
    print(pm in pmtpl)
