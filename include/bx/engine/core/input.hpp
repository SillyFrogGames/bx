#pragma once

#include "bx/engine/core/type.hpp"

// TODO: Move this to window module
// TODO: Review this code, needs it's own backend cpp file for GLFW, and similar/merged logic to the Window module

/// <summary>
/// An enum listing all possible gamepad buttons with digital input values.
/// This is the same numbering as in GLFW input, so a GLFW-based implementation can use it directly without any further mapping.
/// </summary>
ENUM(GamepadButton,
	BUTTON_SOUTH = 0,
	BUTTON_EAST = 1,
	BUTTON_WEST = 2,
	BUTTON_NORTH = 3,

	SHOULDER_LEFT = 4,
	SHOULDER_RIGHT = 5,

	BUTTON_SELECT = 6,
	BUTTON_START = 7,

	// Button 8 is not used

	STICK_LEFT = 9,
	STICK_RIGHT = 10,

	DPAD_UP = 11,
	DPAD_RIGHT = 12,
	DPAD_DOWN = 13,
	DPAD_LEFT = 14
);

/// <summary>
/// An enum listing all possible gamepad axes with analog input values.
/// This is the same numbering as in GLFW input, so a GLFW-based implementation can use it directly without any further mapping.
/// </summary>
ENUM(GamepadAxis,
	/// Represents the horizontal axis of the left gamepad stick, with an analog input value between -1 (left) and 1 (right).
	STICK_LEFT_X = 0,
	/// Represents the vertical axis of the left gamepad stick, with an analog input value between -1 (down) and 1 (up).
	STICK_LEFT_Y = 1,
	/// Represents the horizontal axis of the right gamepad stick, with an analog input value between -1 (left) and 1 (right).
	STICK_RIGHT_X = 2,
	/// Represents the vertical axis of the right gamepad stick, with an analog input value between -1 (down) and 1 (up).
	STICK_RIGHT_Y = 3,
	/// Represents the left trigger of a gamepad, with an analog input value between 0 (not pressed) and 1 (fully pressed).
	TRIGGER_LEFT = 4,
	/// Represents the right trigger of a gamepad, with an analog input value between 0 (not pressed) and 1 (fully pressed).
	TRIGGER_RIGHT = 5
);

ENUM(MouseButton,
	MOUSE_BUTTON_LEFT = 0,
	MOUSE_BUTTON_RIGHT = 1,
	MOUSE_BUTTON_MIDDLE = 2
);

// TODO: add more keys
ENUM(Key,
	RIGHT = 262,
	LEFT = 263,
	DOWN = 264,
	UP = 265,
	SPACE = 32,
	ESCAPE = 256,
	ENTER = 257,

	A = 65,
	B = 66,
	C = 67,
	D = 68,
	E = 69,
	F = 70,
	G = 71,
	H = 72,
	I = 73,
	J = 74,
	K = 75,
	L = 76,
	M = 77,
	N = 78,
	O = 79,
	P = 80,
	Q = 81,
	R = 82,
	S = 83,
	T = 84,
	U = 85,
	V = 86,
	W = 87,
	X = 88,
	Y = 89,
	Z = 90
);

class Input
{
public:
	/// <summary>
	/// Returns the current floating-point value of a given gamepad axis.
	/// </summary>
	/// <param name="button">The ID of the axis to check.</param>
	/// <returns>The current value of the given axis. For the possible ranges per axis, see the documentation of xs::input::gamepad_axis.</returns>
	static float GetAxis(GamepadAxis axis);

	/// <summary>
	/// Checks and returns whether a given gamepad button is currently being held down.
	/// </summary>
	/// <param name="button">The ID of the button to check.</param>
	/// <returns>true if the given button is being held down; false otherwise.</returns>
	static bool GetButton(GamepadButton button);

	/// <summary>
	/// Checks and returns whether a given gamepad button is being pressed in the current frame without having been pressed in the previous frame.
	/// </summary>
	/// <param name="button">The ID of the button to check.</param>
	/// <returns>true if the given button is being pressed in this frame and not in the previous; false otherwise.</returns>
	static bool GetButtonOnce(GamepadButton button);

	/// <summary>
	/// Checks and returns whether a given keyboard key is currently being held down.
	/// </summary>
	/// <param name="key">The keycode of the key to check.</param>
	/// <returns>true if the given key is being held down; false otherwise.</returns>
	static bool GetKey(Key key);

	/// <summary>
	/// Checks and returns whether a given keyboard key is being pressed in the current frame without having been pressed in the previous frame.
	/// </summary>
	/// <param name="key">The keycode of the key to check.</param>
	/// <returns>true if the given key is being pressed in this frame and not in the previous; false otherwise.</returns>
	static bool GetKeyOnce(Key key);

	/// <summary>
	/// Checks and returns whether mouse input is currently available.
	/// </summary>
	static bool GetMouse();

	/// <summary>
	/// Checks and returns whether a given mouse button is currently being held down.
	/// </summary>
	/// <param name="button">The keycode of the mouse button to check.</param>
	/// <returns>true if the given button is being held down; false otherwise.</returns>
	static bool GetMouseButton(MouseButton button);

	/// <summary>
	/// Checks and returns whether a given mouse button is being pressed in the current frame without having been pressed in the previous frame.
	/// </summary>
	/// <param name="button">The keycode of the mouse button to check.</param>
	/// <returns>true if the given button is being pressed in this frame and not in the previous; false otherwise.</returns>
	static bool GetMouseButtonOnce(MouseButton button);

	/// <summary>
	/// Gets the current X coordinate of the mouse position in game coordinates.
	/// </summary>
	static float GetMouseX();

	/// <summary>
	/// Gets the current Y coordinate of the mouse position in game coordinates.
	/// </summary>
	static float GetMouseY();

	/// <summary>
	/// Gets the current numbers of point where the touchscreen is being touched.
	/// </summary>
	static int GetNumTouches();

	/// <summary>
	/// Gets the unique ID corresponding to a touch array index, or -1 if that touch does not exist.
	/// This can be used to track touches of the same finger across multiple frames.
	/// The array index may change per frame, but the ID will not.
	/// </summary>
	/// <param name="index">The array index of the touch point to check.</param>
	static int GetTouchId(int index);

	/// <summary>
	/// Gets the current X coordinate (in game coordinates) of the touch position at the given array index.
	/// Returns 0.0 if the given touch does not exist.
	/// </summary>
	/// <param name="index">The array index of the touch point to check.</param>
	static float GetTouchX(int index);

	/// <summary>
	/// Gets the current Y coordinate (in game coordinates) of the touch position at the given array index.
	/// Returns 0.0 if the given touch does not exist.
	/// </summary>
	/// <param name="index">The array index of the touch point to check.</param>
	static float GetTouchY(int index);

	/// <summary>
	/// Turns on vibration for the left and right motor if the used controller supports this
	/// </summary>
	/// <param name="lightRumble">Value between 0 (off) and 1 (max rotation) for the left motor</param>
	/// <param name="heavyRumble">Value between 0 (off) and 1 (max rotation) for the right motor</param>
	static void SetPadVibration(int leftRumble, int rightRumble);

	/// <summary>
	/// Sets the color value for the lightbar on Dualshock controllers (4 and up)
	/// <para> Currently only implemented for the PS5 and dualshock 4+ controllers</para>
	/// </summary>
	/// <param name="r, g, b"> The new coulour values in RGB format ranging between 0 and 255</param>
	static void SetPadLightbarColor(float r, float g, float b);

	/// <summary>
	/// Turns off the light bar
	/// <para> Currently only implemented for the PS5 and dualshock 4+ controllers</para> 
	/// </summary>
	static void ResetPadLightbarColor();

private:
	friend class Runtime;
	friend class Module;

	static bool Initialize();
	static void Reload();
	static void Shutdown();

	static void Poll();
};