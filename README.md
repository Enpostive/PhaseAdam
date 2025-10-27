# PhaseAdam

A simple sampler plugin (VST3 and AU) which takes a sidechain audio input and plays samples in phase with the sidechain. It does this by simply measuring the correlation of the sample with the input audio and flipping the phase of the sample when it correlates badly. Right now it's very basic!

## Features
- Play samples in phase with the sidechain input.
- Turn up and down the volume.
- Change the tuning.
- Zoom in and out of the waveform (click and drag up and down similar to Ableton Live).

## Todo List
At this stage, there is a ton of things that I want to fix up:

- Add CMake files and remove the .jucer file
- Better GUI
- Start, end and loop points
- Envelope controls
- Filter
- Maybe some basic synthesis features like an oscillator or grannular synthesis modes.
- Pay the developer fee and release signed binaries.

If you want to help out, I'm all ears! Create an issue or pull request to get my attention.
