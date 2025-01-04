# Smart Coaster

## About

In this project, I developed a smart coaster to help me track how much water I was drinking, and encourage healthy levels of water consumption. I was inspired to develop this project when my doctor noted that I was not drinking enough water for my size, age, weight, etc. so I wanted to create a technological solution that would help me improve this bad habit, and perhaps be a general purpose tool that anyone could use to help track their own water consumption levels. My project can be broken down into the following:

- Hardware &#8594; I wanted to make the device as 'unobtrusive as possible' (i.e. I did not want the user to have to drink out of a specially made water bottle). This inspired me to create a "coaster" element, using:
    -   **Load Cell**: An electronic component/circuit element that acts as a sensor in response to changes of force exerted on it (namely weight/gravity). It contains flex sensors which change resistance once flexed/bent, even so slightly. When the metal load sensor experiences force from the weight of an object (say a glass of water) the flex sensors are also bent, and produce readings that can be calibrated into actual masses.
    -   **HX711**: This was the ADC (analog-to-digital converter) used for the previously mentioned load cell. It also acts as an amplifier for the signals produced by the load cell. This element was crucial in order to actually use the data produced by the load cell.
    -   **ESP32-S3-WROOM-2**: This was the microcontroller used to perform all the logic and operations on the board, namely getting the readings from the load cell (after calibration, and in a process loop) and then sending those readings to Firebase (after establishing a wifi connection accordingly).

- Software
    - **Firebase**: A real time database to store all readings produced by the coaster at a rate of 0.2 Hz
    - **Express.js**: This is the framework used to build the backend API to process the readings and produce meaningful data to a user/frontend (i.e. the total water consumed, over different, in different units of measurement, etc.).
    - **Swift**: This was the language used to build the iOS app for a user to practically monitor their water consumption.
    - **ESP-IDF**: The framework used to build, flash, monitor firmware on the ESP32 (see https://docs.espressif.com/projects/esp-idf/en/stable/esp32/get-started/index.html for more details)
