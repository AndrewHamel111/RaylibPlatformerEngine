#ifndef BUTTON_H_
#define BUTTON_H_

#include "raylib.h"
#include "operators.h"

#define SLIDER_HANDLE_HEIGHT 30
#define SLIDER_HEIGHT 20

/**
* @brief Draws described button and returns true when clicked.
* @param bounds The bounds of the button {x,y,width,height} where x,y denote the top left corner
* @param atlas The Texture2D sprite atlas the button image will come from.
* @param source The Rectangle describing the portion of the atlas containing the sprite.
* @param altSource Same as above, but for the alternate sprite that the button will switch to when hovered.
* @returns True iff the button is clicked on this frame.
* specializes ImageButtonEx by offering the chance to include two source rects: one for what should ordinarily be shown
* and an alternate for what should be shown when hovering.
*/
bool ImageButtonSpriteSwap(Rectangle bounds, Texture2D atlas, Rectangle source, Rectangle altSource);

/**
* @brief Draws described button and returns true when clicked.
* @param bounds The bounds of the button {x,y,width,height} where x,y denote the top left corner
* @param atlas The Texture2D sprite atlas the button image will come from.
* @param source The Rectangle describing the portion of the atlas containing the sprite.
* @returns True iff the button is clicked on this frame.
* A replacement for raygui::GuiImageButtonEx (which was not very good at all).
*/
bool ImageButtonEx(Rectangle bounds, Texture2D atlas, Rectangle source);

/**
* @brief Creates an invisible button that returns true when clicked.
* @param bounds Defines the area of the hidden button.
* @returns True iff the button is pressed this frame.
*/
bool HiddenButton(Rectangle bounds);

/**
* @brief Draws described slider bar, which is impacted by the user's mouse interaction and on change updates the value pointed to by percent.
* @param start Left side of slider bar
* @param end Right side of slider bar
* @param percent A pointer to where the "result" of the bar should be stored.
* @param scale multiplier applied to the regular percentage
* @returns the value of percent times the optional parameter scale = 1.0
* The function changes the value pointed to by percent, but also returns a scaled version of the percentage
* (which by default will be the same as the value stored at the pointer).
*/
float SliderBar(Vector2 start, float length, float* percent, float scale = 1.0);

/**
* @brief Specialization of SliderBar that takes the center of the bar instead, to take some of the weight off UI design.
* @param start Left side of slider bar
* @param end Right side of slider bar
* @param percent A pointer to where the "result" of the bar should be stored.
* @param scale multiplier applied to the regular percentage
* @returns the percentage of the bar that is full times the optional parameter scale = 1.0
*/
float SliderBarCenter(Vector2 center, float length, float* percent, float scale = 1.0);

#endif
