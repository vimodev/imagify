# imagify

Turn files into images and back. Lossy image compression will of course render the image useless.

![This README file as an image](/README.png "This README file as an image")

## Usage

`imagify <file> <outputfile>`

Imagify will encode if `file` does not have a `.png` extension, and will decode if it does. Hence `imagify image.png doc.pdf` will decode, and `imagify doc.pdf image.png` will encode.

## Building

Run `make`.

Or alternatively run `gcc -I . main.c stb_image/stb_image.h stb_image/stb_image_write.h -lm -o imagify`