CONTROLS:

If no controller plugged in, control will default to keyboard
If a controller is plugged in, keyboard control will be turned off


CONTROLLER:

Right Trigger (Accelerate)
Left Trigger (Reverse)
Left Stick (Steer)
X-Button (Handbrake)
B-Button (Turbo Boost)
Start-Button (Pause Game)

KEYBOARD:

W-Key (Accelerate)
A-Key (Steer Left)
D-Key (Steer Right)
S-Key (Reverse)
Left-Shift (Handbrake)
Space (Turbo Boost)
P-Key (Pause Game)

===========================

Configurations:

x86 (debug) - PVD is enabled
x86 (release) - PVD is disabled


CURRENT GAMEPLAY:


-For this milestone, we have a more refined gameplay including the cookie and hot potato, giving rise to more aggressive ai
- using the boost mechanic, bash into the other players to get them to drop their fruit
- score is point based, completing shopping list gives bonus points
-The match will play for 5mins (can pause at any time -- see controls).
-Once the time hits 0, the match is over and the game will just pause for now. 
-The game features you + 2 AI bots. Each equipped with a shopping list of items to check off.
-Try to pickup as many things you see, but prioritize seeking out items on your list.
-Right now, the AI bots do a sufficient job of pathfinding with obstacle avoidance, but simply try to get to their shopping list items without much thought to other items.
-The AI also won't use the bash mechanic right now.
-boost still needs some work as it results in flying shopping carts if you are airborne (especially with the hot potato)
-flipping over is still an issue (we are working on a "reset button")

============================

In case the DLLs are missing from TopShopper/bin/Win32/debug or TopShopper/bin/Win32/release,
they can be found in x86DebugDLLs and x86ReleaseDLLs respectively.

============================











