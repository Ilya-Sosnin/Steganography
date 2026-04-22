import cv2
import numpy as np

variance_all = 0
for i in range(1, 101):
    path_orig = f"../datasets/3/{i}.bmp"
    bit_num = 1

    img = cv2.imread(path_orig, 0)

    bit_plane = (img >> bit_num) & 1

    var = np.var(bit_plane)

    print(str(var).replace('.', ','))

    variance_all += var

print("Variance: ", variance_all / 100)
