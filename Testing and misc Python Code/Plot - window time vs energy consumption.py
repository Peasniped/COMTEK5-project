import matplotlib.pyplot as plt

# Data
x_values = [1049.56, 504.31, 147.34]
y_values = [9.27, 11.52, 15.00]

# Custom shades of AAU Blue
colors = [
    (33/255, 26/255, 82/255),   # AAU Blue (Darkest)
    (70/255, 66/255, 120/255),  # Lighter Shade 1
    (120/255, 116/255, 170/255) # Lighter Shade 2
]
labels = ["Boat-borne", "Drone-borne", "Car-borne"]

# Plotting
plt.figure(figsize=(8, 6))
bars = plt.bar(x_values, y_values, color=colors, width=75)

# Add legend
plt.legend(bars, labels, title="Time in Range (s)", loc="upper right", fontsize=12)

# Labels and titles
plt.xlabel("w[t] - time in range of sensor (s)", fontsize=16)
plt.ylabel("Avg. Energy Consumption (µW)", fontsize=16)

# Display plot
plt.tight_layout()
plt.show()
