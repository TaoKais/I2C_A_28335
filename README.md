# I2C_A_28335

This repository contains the implementation and example code for using the I2C (Inter-Integrated Circuit) protocol with the Texas Instruments TMS320F28335 microcontroller. The primary goal of this project is to provide an easy-to-use and well-documented codebase for interfacing various I2C devices with the TMS320F28335.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Software Requirements](#software-requirements)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Examples](#examples)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Introduction

I2C (Inter-Integrated Circuit) is a widely used communication protocol that allows multiple devices to communicate with each other over a two-wire bus. This repository provides a comprehensive guide and example code for implementing I2C communication on the Texas Instruments TMS320F28335 microcontroller.

## Features

- Easy-to-use I2C driver for TMS320F28335
- Support for multiple I2C devices
- Example code for common I2C peripherals
- Well-documented codebase

## Hardware Requirements

- Texas Instruments TMS320F28335 microcontroller
- I2C compatible devices (e.g., sensors, EEPROMs, etc.)
- Breadboard and jumper wires
- Power supply (3.3V or 5V depending on the I2C devices)

## Software Requirements

- Code Composer Studio (CCS) IDE
- Texas Instruments C2000ware
- Git (for cloning the repository)

## Getting Started

1. Clone the repository to your local machine:
   ```bash
   git clone https://github.com/TaoKais/I2C_A_28335.git
   ```
2. Open Code Composer Studio (CCS) IDE.
3. Import the project into CCS:
   - Go to `File` -> `Import` -> `Code Composer Studio` -> `CCS Projects`.
   - Select the cloned repository folder.
   - Click `Finish` to import the project.
4. Build the project by clicking the `Build` button in the toolbar.
5. Connect your TMS320F28335 microcontroller to your computer using a USB cable.
6. Load the program onto the microcontroller by clicking the `Debug` button in the toolbar.

## Usage

The repository includes example code for various I2C peripherals. Each example is located in the `examples` folder. To use a specific example, open the corresponding source file and follow the instructions provided in the comments.

## Examples

- **Example 1:** Reading temperature data from an I2C temperature sensor.
- **Example 2:** Writing and reading data to/from an I2C EEPROM.
- **Example 3:** Interfacing with an I2C LCD display.

## Contributing

Contributions are welcome! If you have any suggestions, improvements, or bug fixes, please create a pull request. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Acknowledgements

- Texas Instruments for providing the TMS320F28335 microcontroller and development tools.
- The open-source community for their valuable contributions and support.
