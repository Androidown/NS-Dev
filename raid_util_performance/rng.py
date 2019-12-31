#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from pokemon import PokeMonTemplate
import configparser


U32 = 2**32 - 1
U64 = 2**64 - 1
base_seed = 0x82A2B175229D6A5B

conf = configparser.ConfigParser()
conf.read("ini/config.ini")


def search_back(seed, count=4):
	for _ in range(count):
		seed = ((seed|(U64+1)) - base_seed) & U64
	return hex(seed)


class XoroShiro(object):
    __slots__ = 'seed', 'next_seed'

    def __init__(self, seed):
        self.seed = [seed & U64, base_seed]
        self.next_seed = (seed + base_seed) & U64

    def next_frame(self):
        self.seed = [self.next_seed, base_seed]
        self.next_seed = (self.next_seed + base_seed) & U64

    def __iter__(self):
        return self

    def __next__(self):
        s0, s1 = self.seed
        rslt = (s0 + s1) & U64

        s1 ^= s0
        self.seed = [((s0 << 24) | (s0 >> 40)) & U64 ^ s1 ^ ((s1 << 16) & U64),
                     ((s1 << 37) | (s1 >> 27)) & U64]

        return rslt

    @staticmethod
    def next_p2(x):
        x -= 1
        if x <= 31:
            anchor = 0
            while x > anchor:
                anchor = (anchor << 1) + 1
            return anchor

        for i in range(6):
            x |= x >> (1 << i)
        return x

    def next_int(self, max_int):
        mask = self.next_p2(max_int)
        rslt = next(self) & mask

        while rslt >= max_int:
            rslt = next(self) & mask
        return rslt

    def next_u32(self):
        rslt = next(self) & U32

        while rslt >=U32:
            rslt = next(self) & U32
        return rslt


class PMFinder(object):
    def __init__(self, seed, iv_count, allow_hidden, random_gender, pm_tmpl: PokeMonTemplate, gender_ratio=127):
        self.xor = XoroShiro(seed)

        pm_tmpl.numerize()
        for attr in pm_tmpl.__slots__:
            setattr(self, attr, getattr(pm_tmpl, attr))
        del pm_tmpl

        self.iv_cnt = iv_count
        self.hidden = allow_hidden
        self.random_gender = random_gender
        self.gender_ratio = gender_ratio

    def _pm_check_shiny(self):
        otid = self.xor.next_u32()
        pid = self.xor.next_u32()
        otsv = (otid >> 16) ^ (otid & 0xffff)
        psv = (pid >> 16) ^ (pid & 0xffff)

        if otsv == psv:  # Shiny
            return self.shiny_type & 4
        elif otsv >> 4 == psv >> 4:
            return self.shiny_type & 2
        else:
            return self.shiny_type & 1

    def _pm_check_ivs(self):
        ivs = [-1] * 6
        i = 0
        while i < self.iv_cnt:
            stat = self.xor.next_int(6)
            if self.IVs_max[stat] != 31:
                return False
            elif ivs[stat] == -1:
                ivs[stat] = 31
                i += 1

        for i in range(6):
            if ivs[i] == -1:
                iv = self.xor.next_int(32)
                if not (self.IVs_min[i] <= iv <= self.IVs_max[i]):
                    return False

        return True

    def _pm_check_ability(self):
        if self.hidden:
            ability = 1 << self.xor.next_int(3)
        else:
            ability = 1 << self.xor.next_int(2)
        return ability & self.ability

    def _pm_check_gender(self):
        if self.random_gender:
            gender = 1 << int(self.xor.next_int(252) + 1  < self.gender_ratio)
            return gender & self.gender
        else:
            return True

    def _pm_check_nature(self):
        nature = 1 << self.xor.next_int(25)
        return nature & self.nature 

    def __iter__(self):
        return self

    def __next__(self):
        self.xor.next_u32()
        if self._pm_check_shiny() and \
                self._pm_check_ivs() and \
                self._pm_check_ability() and \
                self._pm_check_gender() and \
                self._pm_check_nature():
            return True

        self.xor.next_frame()
        return False


if __name__ == '__main__':
    import time
    start = time.time()
    
    pmtpl = PokeMonTemplate()
    pmtpl.set_iv_min_all(31)
    # pmtpl.set_iv_min_spa(0)
    pmtpl.shiny_type.add("star")
    pmtpl.ability.add("H")
    pmtpl.gender.add("female")
    pmf = PMFinder(0x12314, 4, True, True, pmtpl)

    cnt = 0

    while next(pmf) is False:
    	cnt += 1
    	if cnt % 1000000 == 0:
    		print(cnt)
    print(search_back(pmf.xor.next_seed))

    print(f"costs {time.time() - start}")
    