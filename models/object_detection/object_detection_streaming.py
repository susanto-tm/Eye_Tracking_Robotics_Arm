import numpy as np
import os
import sys
import tensorflow as tf
import serial

from imutils.video import FileVideoStream
from imutils.video import FPS
import imutils
import time

import cv2 as cv
cap = cv.VideoCapture(0)
fps = FPS().start()

print("[INFO] Initializing serial output...")
ser = serial.Serial('COM3', baudrate=9600, timeout=2)
time.sleep(2)

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
PATH_TO_TEST_IMAGES_DIR = '../test_images'
TEST_IMAGES_PATH = [os.path.join(PATH_TO_TEST_IMAGES_DIR, 'image{}.jpg'.format(i)) for i in range(1, 3)]

# Size in inches, of the output images
IMAGE_SIZE = (12, 8)

# Initialize record of coordinates
coord = []
with detection_graph.as_default():
    with tf.Session(graph=detection_graph) as sess:
        print("[INFO] Running Object Detector...")
        while True:
            ret, image_np = cap.read()
            image_np = cv.flip(image_np, 1)
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

            # Append midpoint coordinates and truncate first element if length is > 5
            coord.insert(0, [int(xcenter), int(ycenter)])

            # xSerCoord = coord[0][0]
            # ySerCoord = coord[0][1]

            # Concatenate string of integers from each sub-array into <xxx, xxx>, using bytes() to send to serial
            # then use substring to extract one by one in Arduino
            serialFormat = "<{0:d},{1:d}>".format(coord[0][0], coord[0][1])
            xFormatCoord = str(int(coord[0][0]))
            # ser.write(bytes(serialFormat, 'utf-8'))
            ser.write(bytes(xFormatCoord, 'utf-8'))

            print(ser.readline())

            if len(coord) > 5:
                coord.pop()

            print(coord)

            cv.rectangle(image_np, (int(left), int(top)), (int(right), int(bottom)), (0, 0, 255), thickness=2)

            cv.circle(image_np, (int(xcenter), int(ycenter)), radius=2, color=(0, 0, 255), thickness=2)

            # cv.putText(image_np, "{}".format(scores * 100), (int(left), int(top)), cv.FONT_HERSHEY_SIMPLEX, 0.7,
            #            (0, 0, 255))

            # for i in range(len(coord) - 1):
            #     cv.line(image_np, (coord[i][0], coord[i][1]), (coord[i+1][0], coord[i+1][1]), (0, 0, 255), thickness=2)

            cv.imshow('object_detection', cv.resize(image_np, (800, 600)))
            if cv.waitKey(25) & 0xFF == ord('q'):
                cv.destroyAllWindows()
                fps.stop()
                break
            fps.update()

print("[INFO] approx. FPS: {:.2f}".format(fps.fps()))
