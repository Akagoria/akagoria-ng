#!/bin/sh

FONT_DIR=/usr/share/fonts/truetype/dejavu
FONTS="DejaVuSans.ttf DejaVuSans-Bold.ttf DejaVuSans-Oblique.ttf DejaVuSans-BoldOblique.ttf"

for FONT in ${FONTS}
do
  echo "Copying $FONT..."
  cp -f "$FONT_DIR/$FONT" "."
done
