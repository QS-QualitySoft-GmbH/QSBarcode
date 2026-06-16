# Native Runtime Assets

Release packaging copies signed native runtime assets here under `native/<rid>/`.

Expected examples:

- `native/win-x64/qs_barcode_loader_sdk.dll`
- `native/win-x64/pdfium.dll`
- `native/linux-x64/libqs_barcode_loader_sdk.so`
- `native/linux-x64/libpdfium.so`
- `native/osx-arm64/libqs_barcode_loader_sdk.dylib`
- `native/osx-arm64/libpdfium.dylib`

Runtime binaries are not stored in the public source repository. They are attached to GitHub Releases after the monorepo build signs and verifies them.
