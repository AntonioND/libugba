-- Test that runs a program for 30 frames, takes a screenshot, presses the A
-- button to swap the framebuffers and other keys to move and rotate the
-- background, and takes another screenshot.

run_frames_and_pause(30)
screenshot("screenshot-1.png")
keys_hold("A", "L", "RIGHT", "DOWN")

run_frames_and_pause(30)
screenshot("screenshot-2.png")

exit()

return 0
