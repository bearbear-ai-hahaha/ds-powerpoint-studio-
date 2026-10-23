# DS PowerPoint Studio (starter homebrew project)

A starter project for a Nintendo DS Lite + DS PiCO workflow.

## What is included

- `ds_app/main.c`: libnds C source for a dual-screen slide editor/viewer.
- `ds_app/Makefile`: devkitPro/devkitARM build recipe.
- `tools/export_pptx.py`: converts the portable `.dspres` JSON project format to a real `.pptx`.
- `examples/demo.dspres`: example presentation.
- `BUILDING.md`: setup and limitations.

## Important status

This is source code, not a precompiled/tested `.nds` ROM. This environment does not have the Nintendo DS devkitARM/libnds toolchain installed, so I cannot honestly claim the ROM has been compiled or tested here. Build it with devkitPro's Nintendo DS toolchain, then test in an emulator before using the DS PiCO.

The DS app saves project files to `/data/ds-powerpoint/` on the flashcart filesystem when FAT support is available. Actual path availability depends on the flashcart's DLDI driver and firmware. The DS program uses a compact JSON-like `.dspres` format. The exporter runs on a computer or Android device with Python and `python-pptx` installed.

## Controls in the DS app

- D-pad Left/Right: previous/next slide
- A: edit title
- B: edit body text
- X: toggle presentation/view mode
- Y: save project
- L/R: move cursor through editable text
- Touchscreen: tap a slide's title/body region to select it; tap the on-screen controls at the bottom to navigate, edit, or save
- START: help/controls

Text editing uses a simple character picker rather than a full keyboard. This is intentionally lightweight for DS hardware.

## Export to PowerPoint

On a computer with Python:
```bash
python -m pip install python-pptx
python tools/export_pptx.py examples/demo.dspres demo.pptx
```

Copy your `.dspres` file from the flashcart to the computer/phone, then run the same command with your file. The exporter creates a 16:9 presentation with titles, body text, and a simple background color per slide.

## Project format

A `.dspres` file is UTF-8 JSON:
```json
{
  "title": "My presentation",
  "slides": [
    {"title": "First slide", "body": "Hello!", "bg": "navy"}
  ]
}
```

Supported background names: `navy`, `black`, `white`, `blue`, `green`, `red`, `purple`, `yellow`.
