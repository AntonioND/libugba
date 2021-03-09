-- Test that runs a program for 30 frames, takes a screenshot and exits.

run_frames_and_pause(30)
screenshot("screenshot-1.png")

keys_hold("SELECT")
run_frames_and_pause(3)
keys_release("SELECT")
run_frames_and_pause(3)

screenshot("screenshot-2.png")

keys_hold("SELECT")
run_frames_and_pause(3)
keys_release("SELECT")
run_frames_and_pause(3)

screenshot("screenshot-3.png")

exit()

return 0
