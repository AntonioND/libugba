-- Test that runs a program for 60 frames, presses START, takes a screenshot and
-- exits.

run_frames_and_pause(60)
keys_hold("START")

run_frames_and_pause(5)

screenshot()
exit()

return 0
