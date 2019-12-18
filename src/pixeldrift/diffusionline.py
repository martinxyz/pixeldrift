#!/usr/bin/env python3
import numpy as np
from matplotlib import pyplot as plt
import random
import imageio

N = 32
data = np.zeros((N, N), dtype='int8')
cmd = np.zeros((N, N), dtype='int8')
cmd[8:N-1-8,8:13] = 1

def linepass_v1(line, line_cmd):
    res = line.copy()
    a = line[0:-2]
    b = line[1:-1]
    diff = a - b
    diff[diff % 2 == +1] += 1
    diff[diff % 2 == -1] -= 1
    diff //= 2
    diff[np.random.randint(0, 2, size=diff.shape) == 0] = 0
    res[0:-2] -= diff
    res[1:-1] += diff
    return res

def linepass_64(line, line_cmd):
    # would have to ripple... doesn't work because of dependencies
    res = line.copy()
    a = line[0:-2]
    b = line[1:-1]
    diff = np.zeros_like(a)
    diff[(a == 1) & (b == 0)] = 1
    diff[(a == 1) & (b == 1)] = 1
    diff[(a == 2) & (b == 0)] = 1
    diff[(a == 2) & (b == 1)] = 1
    diff[(a == 2) & (b == 2)] = 2
    diff[(a == 4) & (b == 0)] = 2
    diff[(a == 4) & (b == 2)] = 2
    diff[(a == 4) & (b == 4)] = 4
    diff[(a == 8) & (b == 0)] = 4
    diff[(a == 8) & (b == 4)] = 4
    diff[(a == 8) & (b == 8)] = 8
    diff[(a == 16) & (b == 0)] = 8
    diff[(a == 16) & (b == 8)] = 8
    diff[(a == 16) & (b == 16)] = 16
    diff[(a == 32) & (b == 0)] = 16
    diff[(a == 32) & (b == 16)] = 16
    diff[(a == 32) & (b == 32)] = 32
    diff[(a == 64) & (b == 0)] = 32
    diff[(a == 64) & (b == 32)] = 32
    diff[np.random.randint(0, 2, size=diff.shape) == 0] = 0
    res[0:-2] -= diff
    res[1:-1] += diff
    return res

def linepass_v3(line, line_cmd):
    res = line.copy()
    a = line[0:-2]
    b = line[1:-1]
    cmd_a = line_cmd[0:-2]
    cmd_b = line_cmd[1:-1]
    diff_smooth = (a+1)//2  # smooth
    diff_clump = (a*7+7)//8  # clumping
    diff_anti = (a*1+7)//8  # anti-clumping?
    diff = diff_smooth
    # diff[cmd_a == 1] = diff[cmd_a == 1].clip(0, 1) # diff_clump[cmd == 1]

    cond1 = (cmd_a == 1) & (cmd_b == 0)
    # diff[cond1] = diff_clump[cond1]
    diff = np.where(cond1, diff_clump, diff)

    # cond2 = (cmd_a == 0) & (cmd_b == 1)
    cond2 = (cmd_b == 1)
    # diff[cond2] = diff_anti[cond2]  # diff[cond2].clip(0, 1)
    diff = np.where(cond2, diff_anti, diff)

    b_new = (b + diff).clip(0, 15)
    diff = b_new - b

    # diff -= (15 - b - diff).clip(0, 15)
    diff[np.random.randint(0, 4, size=diff.shape) == 0] = 0
    res[0:-2] -= diff
    res[1:-1] += diff
    return res

linepass = linepass_v3

for it in range(800):
    for y in range(N):
        line = data[y, :]
        line_cmd = cmd[y, :]
        # if random.random() < 0.9:
        line = linepass(line, line_cmd)
        line = linepass(line[::-1], line_cmd[::-1])[::-1]
        data[y, :] = line
    for x in range(N):
        line = data[:, x]
        line_cmd = cmd[:, x]
        line = linepass(line, line_cmd)
        line = linepass(line[::-1], line_cmd[::-1])[::-1]
        data[:, x] = line
    if it < 800:
        # if data[N//2, N//2] < 15:
        r = 2
        # data[N//2-r:N//2+r+1, N//2-r:N//2+r+1] = 15
        data[8::16, 8::16] = data[8::16, 8::16].clip(3, 15)
    # else:
    #     data[N//2, N//2] = 0

    im = (data.astype('int32')*17).clip(0, 255).astype('uint8')
    imageio.imsave('/tmp/diffuse%04da.png' % it, im, compress_level=3)

    # print(data.sum())

# plt.imshow(data)
# plt.show()
