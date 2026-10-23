# Building and using the starter project

## Build the DS app

1. Install devkitPro and the Nintendo DS development packages (`devkitARM`, `libnds`, `ndstool`, and the DS toolchain utilities).
2. Open a devkitPro shell in `ds_app/`.
3. Run:
   ```bash
   make
   ```
4. If the build succeeds, copy `ds_powerpoint.nds` to your DS PiCO microSD card.
5. Make sure the flashcart firmware supports DLDI/FAT access. Start the app from the cart menu.

## Export a `.pptx`

The DS does not have a practical built-in ZIP/XML/PowerPoint stack in this starter. Use the companion exporter on a computer or Android Python environment.

```bash
python -m pip install python-pptx
python tools/export_pptx.py path/to/project.dspres output.pptx
```

## Limitations of this first build

- Text-only slides with a small set of background colors.
- The on-device text editor is intentionally basic.
- It does not import existing `.pptx` files.
- This source has not been compiled or hardware-tested in this environment.
- If the PiCO firmware doesn't expose a writable FAT filesystem to libfat, saving will need adapting to the cart's storage API.
