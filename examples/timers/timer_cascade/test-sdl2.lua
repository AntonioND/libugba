-- Test that runs a program for 120 frames, takes a screenshot and exits.

run_frames_and_pause(120)
keys_hold("A")

run_frames_and_pause(5)

screenshot()
exit()

return 0
