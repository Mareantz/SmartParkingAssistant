# Smart Parking Assistant

Smart Parking Assistant is a project designed to help clients find free parking spots. It's a project done for computer networks in my 3rd semester at Faculty of Computer Science Iași

## Features

- **Take Slot**: This button allows the client to occupy a free parking slot.
- **Free Slot**: This button allows the client to free up a parking slot that they have previously occupied.
- **Parking Slot Display**: The parking lot is displayed on the screen, with slots marked from A to F and 1-24. The client's spot is highlighted in blue.
- **Parking Slot Status**: Occupied slots are marked in red, and free slots are marked in green.
- **Server and Camera Interaction**: The server receives status updates from a camera which is simulated in the `camera.cpp` file. This camera simulates the process of freeing and occupying slots randomly and sends the updates accordingly to the server and clients.

## Installation

This project was developed on the Windows Subsystem for Linux, and due to the specific libraries used, it's required to run on a Linux environment.

1. Clone the repository to your local machine.
2. Run the `compile` bash script to compile the server code.
**2.1 (Optional)** Open the `camera.cpp` by running `./camera 127.0.0.1 2308`.
3. Run the `compile_client` bash script to compile the client code.
4. Start the server and client.

Note: You can replace `127.0.0.1` and `2308` with your desired IP address and port number, respectively. Make sure to connect to the same PORT as the one defined in the `server.cpp` file

## Usage

To use the Smart Parking Assistant, simply press the "Vreau loc" button to occupy a free slot. Your slot will be highlighted in blue. When you're ready to leave, press the "Elibereaza loc" button to make your slot available to others.

## License

[MIT](https://choosealicense.com/licenses/mit/)