from imutils.video import VideoStream
from imutils.video import FPS
import imutils as im
import time
import cv2 as cv

cvNet = cv.dnn.readNetFromTensorflow('frozen_inference_graph.pb', 'opt_graph.pbtxt')

vs = VideoStream(src=0).start()
time.sleep(2.0)
fps = FPS().start()

while True:
    frame = vs.read()
    frame = im.resize(frame, width=400)

    rows = frame.shape[0]
    cols = frame.shape[1]
    cvNet.setInput(cv.dnn.blobFromImage(frame, size=(300, 300), swapRB=True, crop=False))
    cvOut = cvNet.forward()

    for detection in cvOut[0, 0, :, :]:
        score = float(detection[2])
        if score > 0.9:
            left = detection[3] * cols
            top = detection[4] * rows
            right = detection[5] * cols
            bottom = detection[6] * rows
            cv.rectangle(frame, (int(left), int(top)), (int(right), int(bottom)), (0, 0, 250), thickness=2)
            label = "{:.2f}%".format(score * 100)
            cv.putText(frame, label, (0, 50), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 250), 2)

    cv.imshow("Frame", frame)
    key = cv.waitKey(1) & 0xFF

    if key == ord('q'):
        break

    fps.update()

fps.stop()
cv.destroyAllWindows()
vs.stop()
