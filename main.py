from PIL import Image
import numpy as np
import matplotlib.pyplot as plt
from threading import Thread
import time
def gen_graph(toPlot):
    plt.figure()
    plt.xlabel("time in second")
    plt.ylabel("Temperature")
    plt.plot(toPlot)
    plt.savefig('test.png')
    plt.close()
    im = Image.open("test.png")
    im.save("test.png",dpi = (128,128))
while True:
    myAry = []
    for _ in range(10):
        dataPoints = np.random.rand(1000)
        result = np.average(dataPoints)
        myAry.append(result * 100)
    gen_graph(myAry)
    time.sleep(10)

