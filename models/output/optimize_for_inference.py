import tensorflow as tf

inputGraph = tf.GraphDef()
with tf.gfile.Open('frozen_inference_graph.pb', "rb") as f:
    data2read = f.read()
    inputGraph.ParseFromString(data2read)

outputGraph = optimize_for_inference_lib.optimize_for_inference(

)