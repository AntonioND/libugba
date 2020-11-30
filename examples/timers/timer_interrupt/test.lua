-- Test that runs a program for 70 frames, takes a screenshot, presses A to
-- show the results screen, takes a screenshot and exits.

run_frames_and_pause(70)
-- This screenshot is used as reference when the test fails, it isn't used for
-- the test checks.
screenshot("timer-values.png")
keys_hold("A")

run_frames_and_pause(5)

screenshot()
exit()

return 0
