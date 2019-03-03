from ikpy.chain import Chain
from ikpy.link import OriginLink, URDFLink, DHLink, Link
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D


"""
    translation_vector gives the location of joint in terms of the previous link in terms of [x, y, z]
    orientation is the rotations of the xyz axis with respect to the previous link expressed in [roll, pitch, yaw]
    rotation is the rotation at which the joint rotates expressed in [x, y, z]
    
    Inverse kinematics matrix is expressed using the 4x4 + translation matrix
    
    pos. is the desired position
    rot. is the 3x3 rotational matrix to achieve the position
    
    [x]   [rot., rot., rot., pos.]
    [y] = [rot., rot., rot., pos.]
    [z]   [rot., rot., rot., pos.]
    [w]   [   0,    0,    0,    1] 
"""

# defining robot arm chain structure
# arm_chain = Chain(name="left_arm", links=[
#     OriginLink(),
#     URDFLink(
#         name="base",
#         translation_vector=[0, 0, 1],
#         orientation=[0, 0, 0],
#         rotation=[0, 1, 0],
#     ),
#     URDFLink(
#         name="tilt",
#         translation_vector=[0, 0, 2],
#         orientation=[1.57, 0, 0],
#         rotation=[0, 1, 0]
#     ),
#     URDFLink(
#         name="tile2",
#         translation_vector=[0.5, 0, 0.5],
#         orientation=[0, 0, 0],
#         rotation=[0, 0, 0]
#     ),
#     URDFLink(
#         name="tilt2",
#         translation_vector=[1, 0, 0],
#         orientation=[0, 0, 0],
#         rotation=[0, 0, 0]
#     ),
#     URDFLink(
#         name="tilt",
#         translation_vector=[0, 0, 2],
#         orientation=[0, 1.57, 0],
#         rotation=[0, 1, 0],
#     ),
#     URDFLink(
#         name="elbow",
#         translation_vector=[0, 0, 8],
#         orientation=[0, -1.57, -1.57],
#         rotation=[0, 0, 1],
#     ),
#     URDFLink(
#         name="claw_elbow",
#         translation_vector=[8, 0, 0],
#         orientation=[0, -1.57, 0],
#         rotation=[0, 1, 0]
#     ),
#     URDFLink(
#         name="claw_rotate",
#         translation_vector=[5, 0, 0],
#         orientation=[0, 0, 0],
#         rotation=[1, 0, 0],
#     ),
# ])

claw_chain = Chain.from_urdf_file("arm_chain.urdf")

ax = plt.figure().add_subplot(111, projection="3d")

# plot_chain = claw_chain.plot(claw_chain.inverse_kinematics([
#     [1, 0, 0, 0],
#     [0, 1, 0, 0],
#     [0, 0, 1, 0],
#     [0, 0, 0, 1]
# ]), ax)

print(claw_chain.inverse_kinematics([
    [1, 0, 0, 2],
    [0, 1, 0, 2],
    [0, 0, 1, 1],
    [0, 0, 0, 1]
]))

# plt.show(plot_chain)






