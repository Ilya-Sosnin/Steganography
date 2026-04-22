import cv2
from skimage.metrics import peak_signal_noise_ratio

psnr_all = 0
for i in range(1, 101):
    path_orig = f"../datasets/3/{i}.bmp"
    path_embed = f"../results/3/{i}_embed_bit_num_3.bmp"

    img_orig = cv2.imread(path_orig, 0)
    img_embed = cv2.imread(path_embed, 0)

    psnr = peak_signal_noise_ratio(img_orig, img_embed, data_range=255)
    print(str(psnr).replace('.', ','))

    psnr_all += psnr

print("Average PSNR set: ", psnr_all / 100)
