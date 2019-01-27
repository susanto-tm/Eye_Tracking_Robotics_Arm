import cv2 as cv

cvNet = cv.dnn.readNetFromTensorflow('frozen_inference_graph.pb', 'opt_graph.pbtxt')

img = cv.imread('4.jpg')
rows = img.shape[0]
cols = img.shape[1]
cvNet.setInput(cv.dnn.blobFromImage(img, size=(300, 300), swapRB=True, crop=False))
cvOut = cvNet.forward()

for detection in cvOut[0,0,:,:]:
    score = float(detection[2])
    if score > 0.9:
        left = detection[3] * cols
        top = detection[4] * rows
        right = detection[5] * cols
        bottom = detection[6] * rows
        cv.rectangle(img, (int(left), int(top)), (int(right), int(bottom)), (0, 0, 250), thickness=2)
        label = "{:.2f}%".format(score * 100)
        cv.putText(img, label, (0, 50), cv.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 250), 2)

cv.imshow('img', img)
cv.waitKey()
cv.destroyAllWindows()
