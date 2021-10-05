# Video Motion Compensation
The program can run at the command line as: xxx.exe framen.rgb framen+1.rgb k
Given two consecutive image frames – frame(n) and frame(n+1), the program will creates and display two images
1. a predicted or reconstructed frame for frame(n+1) built using motion compensation techniques from frame(n) and,
2. error difference frame.

## Input format
1. Two consecutive rgb image frames of size 640x480 (first parameter is previous frame, second parameter is frame to predict).
2. Search parameter k, which will define the search area to search into, k can have values from 1 to 32.

## Output format
1. Predicted Y channel for frame(n+1)
2. Error difference for Y channel

## Implementation details
Compute motion vectors only on the Y channel: Conversion of RGB to YUV.