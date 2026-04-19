import sys
import os
import cv2


def image2bmp(path):

    files = os.listdir(path)

    image_ext = (".jpg", ".jpeg", ".png")

    valid_files = [
        file for file in files
        if file.lower().endswith(image_ext)
    ]

    if not valid_files:
        print("Error: No images found in the directory")
        sys.exit(0)

    output_path = os.path.join(path, "bmp")
    os.makedirs(output_path, exist_ok=True)

    for file in valid_files:
        file_name, ext = os.path.splitext(file)
        print(f"Convertation {file} to {file_name}.bmp")

        img_path = os.path.join(path, file)
        img = cv2.imread(img_path)

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        resized = cv2.resize(gray, (512, 512))

        output_file = os.path.join(output_path, file_name + ".bmp")
        cv2.imwrite(output_file, resized)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python image2bmp.py <image_dir>")
        sys.exit(1)

    path = sys.argv[1]

    if not os.path.exists(path):
        print("Error: Incorrect path")
        sys.exit(1)

    if not os.path.isdir(path):
        print("Error: Path is not a directory")
        sys.exit(1)

    image2bmp(path)
