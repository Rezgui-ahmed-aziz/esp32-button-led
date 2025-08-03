# Button-Controlled LED with UART Feedback

This project demonstrates a simple ESP-IDF application for the
ESP-WROOM-32 module. It uses the built-in BOOT button (GPIO 0) to toggle
a built-in LED (GPIO 2) on an ESP32 development board (e.g.,
ESP32-DevKitC). Each button press toggles the LED state (ON/OFF) and
sends a message to the serial monitor via UART. The project uses
FreeRTOS tasks, GPIO interrupts, and debouncing for reliable operation.

## Features

-   Toggles a built-in LED (GPIO 2) when the BOOT button (GPIO 0) is
    pressed.
-   Logs LED state changes to the serial monitor (115200 baud).
-   Implements software debouncing (50ms) to handle button noise.
-   Uses FreeRTOS queue for communication between interrupt and task.

## Setup Instructions

1.  **Clone or Create the Project**:
    -   If cloning, use:

        ``` bash
        git clone 
        cd button_led
        ```

    -   Or create a new ESP-IDF project with the empty template and add
        `main/button_led.c`.
        
4.  **Build the Project**:
    -   Click the **Build** button (hammer icon) or run:

        ``` bash
        idf.py build
        ```
5.  **Flash the Project**:
    -   Connect your ESP-WROOM-32 to `/dev/ttyACM0`.

    -   Click the **Flash** button (lightning bolt icon) or run:

        ``` bash
        idf.py -p /dev/ttyACM0 flash
        ```

    -   Hold the BOOT button during "Connecting..." if needed.
6.  **Monitor Output**:
    -   Click the **Monitor** button (plug icon) or run:

        ``` bash
        idf.py -p /dev/ttyACM0 monitor
        ```

## Usage

-   Press the **BOOT button** (GPIO 0) on your ESP-WROOM-32 board.

-   The LED (GPIO 2) toggles ON/OFF with each press.

-   Serial monitor outputs:

        I (xxx) button_led: BOOT button-controlled LED initialized on ESP-WROOM-32
        I (xxx) button_led: BOOT button pressed, LED is now ON
        I (xxx) button_led: BOOT button pressed, LED is now OFF

