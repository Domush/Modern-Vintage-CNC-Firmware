#!/usr/bin/env python3
#
# Modern Vintage CNC Firmware
# Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]
#
# Based on Marlin and grbl.

#

# Generate mvCNC TFT Images from bitmaps/PNG/JPG

import sys,re,struct
from PIL import Image,ImageDraw

def image2bin(image, output_file):
	if output_file.endswith(('.c', '.cpp')):
		f = open(output_file, 'wt')
		is_cpp = True
		f.write("const uint16_t image[%d] = {\n" % (image.size[1] * image.size[0]))
	else:
		f = open(output_file, 'wb')
		is_cpp = False
	pixs = image.load()
	for y in range(image.size[1]):
		for x in range(image.size[0]):
			R = pixs[x, y][0] >> 3
			G = pixs[x, y][1] >> 2
			B = pixs[x, y][2] >> 3
			rgb = (R << 11) | (G << 5) | B
			if is_cpp:
				strHex = '0x{0:04X}, '.format(rgb)
				f.write(strHex)
			else:
				f.write(struct.pack("B", (rgb & 0xFF)))
				f.write(struct.pack("B", (rgb >> 8) & 0xFF))
		if is_cpp:
			f.write("\n")
	if is_cpp:
		f.write("};\n")
	f.close()

if len(sys.argv) <= 2:
	print("Utility to export a image in mvCNC TFT friendly format.")
	print("It will dump a raw bin RGB565 image or create a CPP file with an array of 16 bit image pixels.")
	print("Usage: gen-tft-image.py INPUT_IMAGE.(png|bmp|jpg) OUTPUT_FILE.(cpp|bin)")
	print("Author: rhapsodyv")
	exit(1)

output_img = sys.argv[2]
img = Image.open(sys.argv[1])
image2bin(img, output_img)
