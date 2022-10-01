'''
Coding: utf-8
Author: vector-wlc
Date: 2021-12-06 23:44:11
Description: 
'''

import os
import time

pack_dir = "./vector/showme/"

os.makedirs(pack_dir + "release", exist_ok=True)

# 打包版本文件
avz_zip_name = time.strftime(
    "./vector/showme/release/%Y_%m_%d.zip", time.localtime())
cmd = "7z a -tzip " + avz_zip_name + " " + pack_dir + "showme"
print(cmd)
os.system(cmd)
