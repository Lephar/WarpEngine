#!/bin/bash

textureExtension=.ktx2
imageExtension={.png,.jpg}
images=../data/assets/*/textures/*$imageExtension

for image in $images
do
    fileName=${image%$imageExtension}
    texture=$fileName$textureExtension

    ../external/bin/ktx create --assign-tf SRGB --format R8G8B8A8_SRGB --encode uastc --generate-mipmap $image $texture
done
