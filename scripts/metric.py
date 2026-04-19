import os
import sys
import cv2
from skimage.metrics import mean_squared_error, peak_signal_noise_ratio, structural_similarity


def metric(origin, altered):
    img_orig = cv2.imread(origin, 0)
    img_alt = cv2.imread(altered, 0)

    mse = mean_squared_error(img_orig, img_alt)
    psnr = peak_signal_noise_ratio(img_orig, img_alt, data_range=255)
    ssim = structural_similarity(img_orig, img_alt, data_range=255)

    print("MSE: ", round(mse, 3))
    print("PSNR: ", round(psnr, 3))
    print("SSIM: ", round(ssim, 3))

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python metric.py <path_to_origin_image> <path_to_altered_image>")
        sys.exit(1)

    if not os.path.exists(sys.argv[1]):
        print("Error: incorrect path to origin image")
        sys.exit(1)

    if not os.path.exists(sys.argv[2]):
        print("Error: incorrect path to altered image")
        sys.exit(1)

    metric(sys.argv[1], sys.argv[2])
    