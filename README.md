# QStreamFileUri

Use GStreamer with QtGStreamer extension. Implement the playbin element to get the playback video following the uri file.
You can use:
  -file in local(only perform test with mp4 format video)
  -file in url ( example https://samplelib.com/lib/download/mp4/sample-30s.mp4 )
  
Next step:
Create Server application. Use the appsrc element to inject selected frame in the GStreamer pipeline. Use OpenCV to extract the frames from video.
