#!/usr/bin/env python
# -*- coding: UTF-8 -*-
from pokemon import PokeMon, PokeMonTemplate
import configparser

U32 = 2**32 - 1
U64 = 2**64 - 1
base_seed = 0x82A2B175229D6A5B

conf = configparser.ConfigParser()
conf.read("ini/config.ini")

def rotl(x, k):
    return ((x << k) | (x >> (64 - k))) & U64


def search_back(seed, count=5):
	for _ in range(count):
		seed = ((seed|(U64+1)) - base_seed) & U64
	return hex(seed)

class XoroShiro(object):
    def __init__(self, seed):
        self.seed = [seed, base_seed]

    def __iter__(self):
        return self

    def __next__(self):
        s0, s1 = self.seed
        rslt = sum(self.seed) & U64

        s1 ^= s0
        self.seed[0] = rotl(s0, 24) ^ s1 ^ ((s1 << 16) & U64)
        self.seed[1] = rotl(s1, 37)

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

    def next_int(self, max_int=U32):
        mask = self.next_p2(max_int)
        rslt = next(self) & mask

        while rslt >= max_int:
            rslt = next(self) & mask
        return rslt


class PMGenerator(object):
    def __init__(self, seed, iv_count, allow_hidden, random_gender, gender_ratio=127):
        self.xor = XoroShiro(seed)
        self.next_seed = sum(self.xor.seed) & U64

        self.pm = None
        self.iv_cnt = iv_count
        self.hidden = allow_hidden
        self.random_gender = random_gender
        self.gender_ratio = gender_ratio

    def _pm_set_shiny(self):
        otid = self.xor.next_int()
        self.pm.pid = pid = self.xor.next_int()
        otsv = ((otid >> 16) ^ (otid & 0xffff)) >> 4
        psv = ((pid >> 16) ^ (pid & 0xffff)) >> 4

        if otsv == psv:  # Shiny
            if (otid >> 16) ^ (otid & 0xffff) ^ (pid >> 16) ^ (pid & 0xffff):
                self.pm.shiny_type = "star"
            else:
                self.pm.shiny_type = "square"
        #     if psv != realTSV:  # Force PID to be shiny from the real TID/SID
        #         high = (pid & 0xFFFF) ^ realTID ^ realSID ^ (shinyType == 1)
        #         pid = (high << 16) | (pid & 0xFFFF)
        # else:
        #     if psv == realTSV:  # Force PID to be not shiny from the real TID/SID
        #         pid ^= 0x10000000

    def _pm_set_ivs(self):
        ivs = self.pm.IVs
        i = 0
        while i < self.iv_cnt:
            stat = self.xor.next_int(6)
            if ivs[stat] == -1:
                ivs[stat] = 31
                i += 1

        for i in range(6):
            if ivs[i] == -1:
                ivs[i] = self.xor.next_int(32)

    def _pm_set_ability(self):
        if self.hidden:
            self.pm.ability = self.xor.next_int(3)
        else:
            self.pm.ability = self.xor.next_int(2)

    def _pm_set_gender(self):
        if self.random_gender:
            gender = self.xor.next_int(252) + 1
            self.pm.gender = int(gender < self.gender_ratio)

    def _pm_set_nature(self):
        self.pm.nature = self.xor.next_int(25)

    def __iter__(self):
        return self

    def __next__(self):
        self.pm = PokeMon()
        self.pm.ec = self.xor.next_int()
        self._pm_set_shiny()
        self._pm_set_ivs()
        self._pm_set_ability()
        self._pm_set_gender()
        self._pm_set_nature()

        self.xor = XoroShiro(self.next_seed)
        self.next_seed = sum(self.xor.seed) & U64
        return self.pm


class PMFinder(object):
    def __init__(self, seed, iv_count, allow_hidden, random_gender, tmpl: PokeMonTemplate, gender_ratio=127):
        self.xor = None
        self.seed = seed & U64

        self.pm_tmpl = tmpl
        self.iv_cnt = iv_count
        self.hidden = allow_hidden
        self.random_gender = random_gender
        self.gender_ratio = gender_ratio


    def _pm_check_shiny(self):
        otid = self.xor.next_int()
        pid = self.xor.next_int()
        otsv = ((otid >> 16) ^ (otid & 0xffff)) >> 4
        psv = ((pid >> 16) ^ (pid & 0xffff)) >> 4
        shiny_type = "null"

        if otsv == psv:  # Shiny
            if (otid >> 16) ^ (otid & 0xffff) ^ (pid >> 16) ^ (pid & 0xffff):
                shiny_type = "star"
            else:
                shiny_type = "square"
        elif self.pm_tmpl.shiny_type:
            return False
        #     if psv != realTSV:  # Force PID to be shiny from the real TID/SID
        #         high = (pid & 0xFFFF) ^ realTID ^ realSID ^ (shinyType == 1)
        #         pid = (high << 16) | (pid & 0xFFFF)
        # else:
        #     if psv == realTSV:  # Force PID to be not shiny from the real TID/SID
        #         pid ^= 0x10000000
        
        return shiny_type in self.pm_tmpl.shiny_type

    def _pm_check_ivs(self):
        xor = self.xor
        ivs = [-1] * 6
        i = 0
        while i < self.iv_cnt:
            stat = xor.next_int(6)
            if self.pm_tmpl.IVs_max[stat] != 31:
                return False
            elif ivs[stat] == -1:
                ivs[stat] = 31
                i += 1

        for i in range(6):
            if ivs[i] == -1:
                iv = xor.next_int(32)
                if not (self.pm_tmpl.IVs_min[i] <= iv <= self.pm_tmpl.IVs_max[i]):
                    return False

        return True

    def _pm_check_ability(self):
        if self.hidden:
            ability = self.xor.next_int(3)
        else:
            ability = self.xor.next_int(2)
        return (not self.pm_tmpl.ability) or ability in self.pm_tmpl.ability

    def _pm_check_gender(self):
        if self.random_gender:
            gender = int(self.xor.next_int(252) + 1  < self.gender_ratio)
            return (not self.pm_tmpl.gender) or gender in self.pm_tmpl.gender
        else:
            return True

    def _pm_check_nature(self):
        nature = self.xor.next_int(25)
        return (not self.pm_tmpl.nature) or nature in self.pm_tmpl.nature 

    def __iter__(self):
        return self

    def __next__(self):
        self.xor = XoroShiro(self.seed)
        self.xor.next_int()
        if self._pm_check_shiny() and \
                self._pm_check_ivs() and \
                self._pm_check_ability() and \
                self._pm_check_gender() and \
                self._pm_check_nature():
            return True

        self.seed = (self.seed + base_seed) & U64
        return False


if __name__ == '__main__':
    import time
    start = time.time()
    
    pmtpl = PokeMonTemplate()
    pmtpl.set_iv_min_all(31)
    # pmtpl.set_iv_min_spa(0)
    pmtpl.shiny_type.add("star")
    pmtpl.ability.add(2)
    pmtpl.gender.add(1)
    pmf = PMFinder(0x12314, 4, True, True, pmtpl)

    cnt = 0

    while next(pmf) is False:
    	cnt += 1
    	if cnt % 1000000 == 0:
    		print(cnt)
    print(search_back(pmf.seed))

    print(f"costs {time.time() - start}")