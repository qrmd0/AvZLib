'''
Coding: utf-8
Author: vector-wlc
Date: 2021-12-06 23:44:11
Description: 
'''

import os
import time

extension_name = "avz1"

pack_dir = f"./vector/{extension_name}/"

os.makedirs(pack_dir + "release", exist_ok=True)

# 打包版本文件
avz_zip_name = time.strftime(
    f"./vector/{extension_name}/release/%Y_%m_%d.zip", time.localtime())
cmd = "7z a -tzip " + avz_zip_name + " " + pack_dir + extension_name
print(cmd)
os.system(cmd)
