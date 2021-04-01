-- Test that resets the saved value, increases it by 10, takes a screenshot and
-- waits for the autosave to save it to disk, increases it by 10 again but
-- crashes the program before autosave has a chance to save it.

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

run_frames_and_pause(180)

screenshot("saved.png")

for i = 1, 10, 1
do
    keys_hold("UP")
    run_frames_and_pause(2)
    keys_release("UP")
    run_frames_and_pause(2)
end

screenshot("not-saved.png")

-- Trigger crash/forced exit
keys_hold("SELECT")

run_frames_and_pause(60)

-- This shouldn't be reached
exit()

return 0
