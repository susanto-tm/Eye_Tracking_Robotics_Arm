from math import *

link1 = 10
link2 = 15
xCoord = 2
yCoord = 5

"""
    Joints are calculated by descending order. Therefore the last joints are calculated first then used in subsequent
    equations for the next joint. Therefore joint_1 will describe the third, joint_2 will describe the second, and so
    on.
"""


# Calculates joint angles recursively. Angles are counted from last to the first.
# Returns angles in radians
def joint_angles(angle, angle_count=0):
    if angle_count == 0:
        angle1 = pi - acos((xCoord ** 2 + yCoord ** 2 - link2 ** 2 - link1 ** 2) / (-2 * link1 * link2))
        print(get_pwm(angle1))
        return joint_angles(angle1, angle_count + 1)  # angle1 is passed in recursively and used in second if statement
    elif angle_count == 1:
        angle2 = atan(yCoord / xCoord) - atan((link2 * sin(angle)) / (link1 + link2 * cos(angle)))
        print(get_pwm(angle2))
        return joint_angles(angle2, angle_count + 1)
    else:
        return angle  # returns first angle


# Converts angles from radians to Pulse-width Modulation (PWM)
# Returns PWM, which gives the degrees in which a servo turns
def get_pwm(angle):
    pwm2 = (degrees(angle) / 180) + 1  # converts angle to degrees then calculates for PWM in milliseconds
    # pwm2 = (angle / pi) * 2000 + 500
    return pwm2


def main():
    print(joint_angles(None))


if __name__ == "__main__":
    main()

