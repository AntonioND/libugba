-- Test that runs a program for 10 frames, presses up 5 times, and then takes a
-- screenshot.

run_frames_and_pause(10)

for i = 1, 5, 1
do
    keys_hold("UP")
    run_frames_and_pause(2)
    keys_release("UP")
    run_frames_and_pause(2)
end

screenshot()

exit()

return 0
