#ifndef ANIMATION_H_INCLUDED
#define ANIMATION_H_INCLUDED

struct animation
{
	Rectangle startRect; /**< The starting source rectangle of the animation. Animations of this type are stored as equally sized rectangles, so when actually changing the sprite for the animation only the x or y value of the startRect will be changed (depending on the value of horizontal sheet) */
	bool horizontalSheet; /**< True when the animation is a horizontal line of sprites moving left to right. false when it is intended to be read as a vertical animation */
	unsigned short framesPerState; /**< the startRect will be translated every framesPerState # of frames (i.e. set 1 for a full 60 frames, or 10 for a 6 frame animation */
};

#endif // ANIMATION_H_INCLUDED
