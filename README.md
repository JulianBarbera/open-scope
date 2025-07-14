# Open Scope

Open Scope is an open source computerized telescope project designed to be
extensible and general purpose.

## Building

Use [PlatformIO](https://platformio.org/) to build the project.

The project currently supports ESP32-C3 and Teensy 4.1

```bash
# Build for ESP32C3
platformio run -e seeed_xiao_esp32c3

# Build for Teensy 4.1
platformio run -e teensy41

# Upload to ESP32C3
platformio run -t upload -e seeed_xiao_esp32c3

# Upload to Teensy 4.1
platformio run -t upload -e teensy41

# Open shell
platformio device monitor --raw
```

## Contributing

Pull requests are welcome. For major changes, please open an issue first to
discuss what you would like to change.

Please test on real hardware before opening a PR.

## License

[MIT](https://choosealicense.com/licenses/mit/)
