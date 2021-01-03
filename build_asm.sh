#!/usr/bin/env bash
echo "Remember to install 'Pillow==7.1.1', 'argparse'..."
png2sp1sprite cat_sprites.png -m cat_sprites_mask.png -i sprite_protar -f 24 > ./circle_sprite_masked.asm
