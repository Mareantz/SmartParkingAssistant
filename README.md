# Smart Parking Assistant

Smart Parking Assistant is a project designed to help clients find free parking spots. It's a project done for computer networks in my 3rd semester at Faculty of Computer Science Iași

## Features

- **Take Slot**: This button allows the client to occupy a free parking slot.
- **Free Slot**: This button allows the client to free up a parking slot that they have previously occupied.
- **Parking Slot Display**: The parking lot is displayed on the screen, with slots marked from A to F and 1-24. The client's spot is highlighted in blue.
- **Parking Slot Status**: Occupied slots are marked in red, and free slots are marked in green.
- **Server and Camera Interaction**: The server receives status updates from a camera which is simulated in the `camera.cpp` file. This camera simulates the process of freeing and occupying slots randomly and sends the updates accordingly to the server and clients.

## Usage

To use the Smart Parking Assistant, simply press the "Vreau loc" button to occupy a free slot. Your slot will be highlighted in blue. When you're ready to leave, press the "Elibereaza loc" button to make your slot available to others.

## License

[MIT](https://choosealicense.com/licenses/mit/)