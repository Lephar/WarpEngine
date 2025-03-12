#!/bin/bash

textureExtension=.ktx2
imageExtension=.jpg
images=../data/*$imageExtension

for image in $images
do
    fileName=${image%$imageExtension}
    texture=$fileName$textureExtension

    ../external/bin/ktx create --format R8G8B8A8_SRGB --encode uastc --generate-mipmap $image $texture
done
