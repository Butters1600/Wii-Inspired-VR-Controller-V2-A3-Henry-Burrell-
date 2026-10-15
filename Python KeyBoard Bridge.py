#!/usr/bin/env python3

"""
Arduino Nano 33 BLE Sense Rev2
Hollow Knight Motion Controller

Movement = Motion controls
Attack = Physical button
"""

import time
import serial
from pynput.keyboard import Controller, Key

keyboard = Controller()

# CONFIGURATION
PORT = "COM11"
BAUD_RATE = 115200

# KEY BINDINGS
KEY_MAP = {
    "LEFT": Key.left,
    "RIGHT": Key.right,
    "UP": "z",
    "DOWN": Key.down,
    "ATTACK": "x",
}

active_keys = set()


def press_key(action):

    key = KEY_MAP.get(action)

    if key is not None and key not in active_keys:
        keyboard.press(key)
        active_keys.add(key)


def release_key(action):

    key = KEY_MAP.get(action)

    if key is not None and key in active_keys:
        keyboard.release(key)
        active_keys.remove(key)


def release_all():

    for key in list(active_keys):
        try:
            keyboard.release(key)
        except Exception:
            pass

    active_keys.clear()


def main():

    ser = None

    print(f"Connecting to Arduino on {PORT}...")

    try:

        ser = serial.Serial(PORT, BAUD_RATE, timeout=1)
        time.sleep(1.5)

        print("Controller connected!")
        print("Tilt to move, press button to attack.")
        print("Press Ctrl+C to exit.")

        while True:

            line = ser.readline().decode(
                "utf-8", errors="ignore"
            ).strip()

            if not line:
                continue

            if line.startswith("PRESS "):

                action = line.split(" ", 1)[1]
                press_key(action)
                print(f"{action} PRESSED")

            elif line.startswith("RELEASE "):

                action = line.split(" ", 1)[1]
                release_key(action)
                print(f"{action} RELEASED")

    except serial.SerialException as e:
        print(f"Serial error: {e}")

    except KeyboardInterrupt:
        print("\nStopping controller...")

    finally:

        release_all()

        if ser is not None and ser.is_open:
            ser.close()

        print("Controller disconnected.")


if __name__ == "__main__":
    main()
