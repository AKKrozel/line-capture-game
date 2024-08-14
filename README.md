# line-capture-game

This project is a videogame in which the goal is to capture all of the creatures on screen by drawing a line using a mouse to enclose them. In order for the player to win, this must be done without the creatures colliding with the drawn line too many times.

## Usage

An easy way to run this project is to add all of the files to a Visual Studio project that has been given proper access to the SFML animation library and font file used then run the project in Visual Studio.

## Animation

Videogame_Anim.mp4 is a video file demonstrating the game mechanics of the line capture game

## Runtime Improvements

Currently, a deque holds the pixels for quick insertion and deletion from the ends. To detect a collision, the deque is converted to an unordered_set using a hash function. Since the unordered_set cannot have duplicates, collision can be checked by seeing if the sizes of the deque and the unordered_set differ. This takes O(n) time. If a collision is detected then an O(n^2) linear search is used to check for the collision location. Then an O(n) process of making a loop -- to check if the monster is enclosed -- and removing pixels is carried out. Since most of the time, there will be no collision, this is better than doing the O(n^2) linear search every frame. To improve runtime further, the lengths of the unordered_set could be checked every time an element of the deque is added. Once the size of the unordered_set does not incriment, a collision must have occured. In which case, the value mapped to by the hashing function could be stored. Then the process could go to the back of the deque and start popping off elements until an element that has the stored hash-value is found. In which case, the process would add the pixel in the back of the deque to a set of Loop-pixels (used to see if the monster is enclosed) and pop that pixel off of the deque until the hash-value is found once-more. This process would allow for detection, loop construction, and removal to all be carried out with one loop over the deque of pixels. Much better than the current implimentation which is O(n^2) and has a number of O(n) sub-processies.

TLDR: The worst-case time-complexity for per-frame set-handeling is O(n^2) currently. It is possible to reduce this to O(n)
