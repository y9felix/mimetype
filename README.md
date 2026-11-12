# mimetype

Fast, lightweight GIO-backed MIME type resolver.

`mimetype` queries the GLib/GIO content-type database to determine exact MIME types and desktop application associations without spawning Python or shell wrappers.

## Requirements

- `glib2` (`gio-2.0`)
- C99 compiler (`cc`)
- GNU Make

## Installation

```sh
make
sudo make install
```

## Usage

```sh
mimetype path/to/file.png
```

## License

MIT License.
