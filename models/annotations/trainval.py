import os

with open("trainval.txt", "w") as a:
    for path, subdirs, files in os.walk(r'C:\Users\Timothy\PycharmProjects\Eye_Tracking_ResNet_UE\models\images'):
        for filename in files:
            name = os.path.splitext(filename)[0]
            a.write(str(name) + '\n')
        
