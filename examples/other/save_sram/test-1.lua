-- Test that runs a program for 10 frames, presses start, and then presses up 10
-- times.

run_frames_and_pause(10)

keys_hold("START")
run_frames_and_pause(2)
keys_release("START")
run_frames_and_pause(2)

for i = 1, 10, 1
do
    keys_hold("UP")
    run_frames_and_pause(2)
    keys_release("UP")
    run_frames_and_pause(2)
end

screenshot("intermediate.png")

exit()

return 0
