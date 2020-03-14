import numpy as np
import matplotlib.pyplot as plt
plt.style.use('seaborn')

labels = [
    "100\n5",
    "300\n10",
    "500\n20",
    "1000\n30",
    "2000\n40",
    "4000\n50",
    "7500\n60",
    "10000\n70"
]

ecos_times = np.array([
    [0.00174351],
    [0.00689504],
    [0.017456],
    [0.0551517],
    [0.163868],
    [0.493795],
    [1.80399],
    [3.81043]
])

eicos_times = np.array([
    [0.00247334],
    [0.0109077],
    [0.0214008],
    [0.0555743],
    [0.169381],
    [0.467405],
    [1.26279],
    [2.80828]
])

plt.yscale('log')
plt.plot(labels, ecos_times, ".", linestyle="-", label="ECOS")
plt.plot(labels, eicos_times, ".", linestyle="-", label="EiCOS")

plt.xlabel('factors/assets')
plt.ylabel('avg. solve time [s]')
plt.legend()

plt.show()
