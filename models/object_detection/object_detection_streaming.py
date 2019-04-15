import numpy as np
import os
import sys
import tensorflow as tf
import serial
import msvcrt

from imutils.video import WebcamVideoStream
from imutils.video import FPS
import imutils
import time

import cv2 as cv
cap = cv.VideoCapture(0 + cv.CAP_DSHOW)
# cap = WebcamVideoStream(src=0).start()
fps = FPS().start()

print("[INFO] Opening serial port...")
ser = serial.Serial('COM3', baudrate=19200, timeout=5)
time.sleep(5)

# This is needed since the notebook is stored in the object_detection folder
sys.path.append("..")

# ##Object Detection Imports
# Here are the imports from the object detection module

from object_detection.utils import label_map_util

from object_detection.utils import visualization_utils as vis_utils

# # Model preparation

# ## Variables
#
# Any model exported using the `export_inference_graph.py` tool can be loaded here simply by changing `PATH_TO_CKPT` to
# point to a new .pb file.

MODEL_NAME = 'output_final'

# Path to frozen inference graph. This is the actual model that is used for the object detection.
PATH_TO_CKPT = MODEL_NAME + '/frozen_inference_graph.pb'

# List of the strings that is used to add the correct labels
PATH_TO_LABELS = os.path.join('annotations', 'label_map.pbtxt')

NUM_CLASSES = 1

# ## Load a (frozen) Tensorflow graph into memory

print("[INFO] Loading frozen Tensorflow graph...")

detection_graph = tf.Graph()
with detection_graph.as_default():
    od_graph_def = tf.GraphDef()
    with tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
        serialized_graph = fid.read()
        od_graph_def.ParseFromString(serialized_graph)
        tf.import_graph_def(od_graph_def, name='')

# ## Loading label map
# Label maps map indices to category names, so that when our convolution network predicts `5`, we know that this
# corresponds to `airplane`.  Here we use internal utility functions, but anything that returns a dictionary mapping
# integers to appropriate string labels would be fine

label_map = label_map_util.load_labelmap(PATH_TO_LABELS)
categories = label_map_util.convert_label_map_to_categories(label_map, max_num_classes=NUM_CLASSES, use_display_name=True)
category_index = label_map_util.create_category_index(categories)


# ## Helper Code
def load_image_into_numpy_array(image):
    (im_width, im_height) = image.size
    return np.array(image.getdata()).reshape(
        (im_height, im_width, 3)).astype(np.uint8)


# # Detection
# PATH_TO_TEST_IMAGES_DIR = '../test_images'
# TEST_IMAGES_PATH = [os.path.join(PATH_TO_TEST_IMAGES_DIR, 'image{}.jpg'.format(i)) for i in range(1, 3)]

# # Size in inches, of the output images
# IMAGE_SIZE = (12, 8)

# Initialize record of coordinates
coord = []
calibration_stage = 0
calibration_state = 0
save_calibration = []
calibration_count = 0
grip_count = 0

with detection_graph.as_default():
    with tf.Session(graph=detection_graph) as sess:
        print("[INFO] Running Object Detector...")
        while True:
            # image_np = cap.read()
            ret, image_np = cap.read()
            image_np = cv.flip(image_np, 0)
            # Expand dimensions since the model expects images to have shape: [1, None, None, 3]
            image_np_expanded = np.expand_dims(image_np, axis=0)
            image_tensor = detection_graph.get_tensor_by_name('image_tensor:0')
            # Each box represents a part of the image where a particular object was detected.
            boxes = detection_graph.get_tensor_by_name('detection_boxes:0')
            # Each score represents how level of confidence for each of the objects
            # Score is shown on the result image, together with the class label
            scores = detection_graph.get_tensor_by_name('detection_scores:0')
            classes = detection_graph.get_tensor_by_name('detection_classes:0')
            num_detections = detection_graph.get_tensor_by_name('num_detections:0')
            # Actual detection
            (boxes, scores, classes, num_detections) = sess.run(
                [boxes, scores, classes, num_detections],
                feed_dict={image_tensor: image_np_expanded})
            # Visualization of the results of a detection
            # vis_utils.visualize_boxes_and_labels_on_image_array(
            #     image_np,
            #     np.squeeze(boxes),
            #     np.squeeze(classes).astype(np.int32),
            #     np.squeeze(scores),
            #     category_index,
            #     use_normalized_coordinates=True,
            #     line_thickness=8)

            # Rewriting calculations found in the visualization_utils.py to obtain normalized coordinates
            im_height, im_width = image_np.shape[:2]

            # Positions are written in (ymin, xmin, ymax, xmax) and its corresponding boxes array
            # Calculations found in visualization_utils.py under draw_bounding_box_on_image function
            position = boxes[0][0]

            """
                position[0]: ymin of bounding box
                position[1]: xmin of bounding box
                position[2]: ymax of bounding box
                position[3]: xmax of bounding box
            """
            # Get normalized points of bounding box
            (left, right, top, bottom) = (position[1] * im_width, position[3] * im_width,
                                          position[0] * im_height, position[2] * im_height)

            # Get the midpoints of the detection box
            xcenter = (int(left) + int(right)) / 2
            ycenter = (int(top) + int(bottom)) / 2

            # Append midpoint coordinates to front of list
            # Ignore coordinate <0, 0> for non-existing bounding box or a blink recorded by the frame
            coord.insert(0, [int(xcenter), int(ycenter)])

            # Concatenate string of integers from each sub-array into <xxx, xxx>, using bytes() to send to serial
            # then parse data in Arduino
            serialFormat = "<{0:d},{1:d}>".format(coord[0][0], coord[0][1])

            if calibration_state == 0 and msvcrt.kbhit():
                key = ord(msvcrt.getch())
                if key == 99:
                    if calibration_stage == 0 or calibration_stage == 1:
                        save_calibration.append(coord[0][0])
                        print(save_calibration)
                    elif calibration_stage == 2 or calibration_stage == 3:
                        save_calibration.append(coord[0][1])
                        print(save_calibration)
                    elif calibration_stage == 4:
                        print(save_calibration)
                        serial_calibration_format = "<{0:d},{1:d},{2:d},{3:d}>".format(save_calibration[0],
                                                                                       save_calibration[1],
                                                                                       save_calibration[2],
                                                                                       save_calibration[3])
                        print(serial_calibration_format)
                        ser.write(bytes(serial_calibration_format, 'utf-8'))

                        while calibration_count < 10:
                            print(ser.readline())
                            calibration_count += 1

                    if calibration_stage < 4:
                        calibration_stage += 1
                    elif calibration_stage == 4 and calibration_state == 0:
                        calibration_state = 1

            elif calibration_state == 1:
                if serialFormat != "<0,0>":  # ignore detection coordinates that does not exist or a blink is recorded
                    ser.write(bytes(serialFormat, 'utf-8'))
                    print(ser.readline())
                    print(coord)

            # Truncate last element if length is > 5. To clear up buffer and memory.
            if len(coord) > 5:
                coord.pop()

            time.sleep(0.5)

            cv.rectangle(image_np, (int(left), int(top)), (int(right), int(bottom)), (0, 0, 255), thickness=2)

            cv.circle(image_np, (int(xcenter), int(ycenter)), radius=2, color=(0, 0, 255), thickness=2)

            # cv.putText(image_np, "{}".format(scores * 100), (int(left), int(top)), cv.FONT_HERSHEY_SIMPLEX, 0.7,
            #            (0, 0, 255))

            # for i in range(len(coord) - 1):
            #    cv.line(image_np, (coord[i][0], coord[i][1]), (coord[i+1][0], coord[i+1][1]), (0, 0, 255), thickness=2)

            cv.imshow('object_detection', imutils.resize(image_np, width=800, height=600))  # cv.resize(image_np, (800, 600)))
            if cv.waitKey(25) & 0xFF == ord('r'):
                calibration_state, calibration_stage, calibration_count = 0, 0, 0
                save_calibration = []
                reset_count = 0

                if calibration_state == 1:
                    ser.write(b'R')

                    while reset_count < 2:
                        print(ser.readline())
                        reset_count += 1

                # if calibration_state == 0:
                #     calibration_state, calibration_stage, calibration_count = 0, 0, 0
                #     save_calibration = []

            elif cv.waitKey(25) & 0xFF == ord('g'):
                ser.write(b'G')

                while grip_count < 3:
                    print(ser.readline())
                    grip_count += 1

                grip_count = 0

            elif cv.waitKey(25) & 0xFF == ord('d'):
                ser.write(b'D')

            elif cv.waitKey(25) & 0xFF == ord('a'):
                ser.write(b'A')

                time.sleep(0.01)

            elif cv.waitKey(25) & 0xFF == ord('p'):
                if calibration_state == 1:
                    calibration_state = 2

                elif calibration_state == 2:
                    calibration_state = 1

            elif cv.waitKey(25) & 0xFF == ord('q'):
                cv.destroyAllWindows()
                fps.stop()
                break

            fps.update()

print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))
