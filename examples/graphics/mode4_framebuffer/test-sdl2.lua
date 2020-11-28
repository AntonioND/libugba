-- Test that runs a program for 10 frames, takes a screenshot, presses thh A
-- button to swap the framebuffers and takes another screenshot.

run_frames_and_pause(10)
screenshot("screenshot-1.png")
keys_hold("A")

run_frames_and_pause(3)
screenshot("screenshot-2.png")

exit()

return 0
