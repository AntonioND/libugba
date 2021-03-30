-- Test that records a few seconds of audio while holding the START button,
-- takes a screenshot, and exits.

wav_record_start()
run_frames_and_pause(5)
keys_hold("START")
run_frames_and_pause(295)
wav_record_end()
screenshot()
exit()

return 0
