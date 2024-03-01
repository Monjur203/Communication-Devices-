# ESP32 Communication Device with ESP Now Encryption and SD Card Integration

This project utilizes two ESP32 devices to establish a communication system with encrypted messaging using ESP Now protocol. Additionally, it features a mobile application developed with MIT App Inventor to facilitate message input and transmission via Bluetooth. Furthermore, it includes a feature where the owner can selectively allow other phones to pair with the ESP32 by sending a specific command. In this updated version, an SD card module is integrated, allowing the ESP32 to save messages encrypted on the SD card and retrieve them upon command from the mobile application.

## Features:
	
- **ESP32 Communication**: Two ESP32 devices are used to establish a communication link.
- **ESP Now Encryption**: Messages transmitted between the ESP32 devices are encrypted using ESP Now encryption for enhanced security.
- **Mobile Application**: A mobile application built with MIT App Inventor allows users to connect to an ESP32 device via Bluetooth, type messages, and send them.
- **Selective Pairing**: The owner can selectively allow other phones to pair with the ESP32 by sending a specific command.
- **SD Card Integration**: An SD card module is integrated with the ESP32, allowing it to save encrypted messages on the SD card.
- **Message Retrieval and Decryption**: Messages stored on the SD card can be retrieved and decrypted upon command from the mobile application, and then displayed in the app.

![Screenshot](/Reference/Untitled.png)

## Getting Started:

To get started with this project, follow the steps below:

1. **Hardware Setup**:
   - Obtain two ESP32 devices.
   - Connect them according to your desired configuration for communication.
   - Attach an SD card module to one of the ESP32 devices.

2. **Software Setup**:
   - Install the necessary development environment for ESP32 (e.g., Arduino IDE).
   - Upload the provided ESP32 code for establishing communication, encryption, selective pairing, and SD card integration.
   - Customize the code according to your requirements.

3. **Mobile Application Setup**:
   - Download and install the mobile application on your Android device.
   - Connect your Android device to the ESP32 devices via Bluetooth.
   - Follow the instructions provided in the application to send encrypted messages and retrieve decrypted messages from the SD card.

## Important point
-	You must change the broadcastAddress[]. You should write the device mac address you want to sent data. example of a MAC address is: 00-B0-D0-63-C2-26 -> {0x00, 0xB0, 0xD0, 0x63, 0xC2, 0x26};
-	![Screenshot](/Reference/1.png)
-	You will find the aio file and apk file in app folder. If you want to make your custom app you can do it using mit app inventor.
-	![Screenshot](Reference/app.png)


## Dependencies:

- Arduino IDE or any other compatible development environment for ESP32.
- MIT App Inventor for modifying or extending the mobile application.

## Usage:

1. Power on both ESP32 devices and ensure they are properly connected.
2. Launch the mobile application on your Android device.
3. Connect your Android device to one of the ESP32 devices via Bluetooth.
4. Type your message in the application and send it.
5. The message will be encrypted and transmitted to the second ESP32 device.
6. Ensure the second ESP32 device is powered on and connected to the phone via Bluetooth to view the received message.
7. To selectively allow other phones to pair with the ESP32, send the specific command as per the instructions provided in the ESP32 code.
8. To save messages on the SD card, use the appropriate command from the mobile application.
9. To retrieve and decrypt messages from the SD card, send the respective command from the mobile application.

## Contributors:

- Manjur Alam Molla
  
## Acknowledgments:

- MIT App Inventor for providing a platform to develop the mobile application.
- ESP32 community for the development resources and support.

## Notes:

- For any issues or suggestions, please open an issue on GitHub or contact [Your Email Address].

---

Feel free to customize this README file according to your project's specific details and requirements.
