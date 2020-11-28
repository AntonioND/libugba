-- Test that runs a program for 70 frames, takes a screenshot and exits.

run_frames_and_pause(70)
screenshot("timer-values.png")
keys_hold("A")

run_frames_and_pause(5)

screenshot()
exit()

return 0
